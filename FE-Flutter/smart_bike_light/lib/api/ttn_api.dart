import 'dart:convert';
import 'package:http/http.dart' as http;
import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_server_client.dart';
import 'package:flutter/foundation.dart';

class TTNApi {
  final String baseUrl;
  final String apiKey;
  final String applicationId; // Encapsulate application ID

  TTNApi({
    required this.baseUrl,
    required this.apiKey,
    required this.applicationId,
  });

  // Factory method to create a TTNApi instance with predefined configuration
  factory TTNApi.createDefault() {
    return TTNApi(
      baseUrl: 'https://eu1.cloud.thethings.network',
      apiKey: 'NNSXS.2RRIRHET3ST3AKV7YMQKLQNPWAYDGME5B6ILBHA.EZUBU7VTBNBHP4J45MZYX6674P6SUTTO2Z4YJ2D6CEMFFEYS3B4Q', // Ensure this API key has the correct permissions
      applicationId: 'smart-bike-light-henrikbach1', // Application ID
    );
  }

  // Public method to send a message
  Future<void> sendMessage(String devEui, String hexPrefix, String message) async {
    try {
      // Prepare the payload
      final prefixedMessage = hexPrefix.isNotEmpty ? '$hexPrefix$message' : message;
      final payloadData = _preparePayload(hexPrefix, message); // Encode the payload

      debugPrint('Preparing to send payload data: $prefixedMessage'); // Log the payload data
      await _sendDownlinkMessage(devEui, payloadData); // Send the message
      debugPrint('Downlink message sent successfully.');
    } catch (e) {
      debugPrint('Error sending message: $e');
      throw Exception('Error sending message: $e');
    }
  }

  // Public method to receive messages
  Future<void> receiveMessage(String devEui, Function(String, String) onMessageReceived) async {
    await _connectToMqtt(devEui, (filteredPayload, deviceId) {
      try {
        // Parse the filtered payload as JSON
        final data = jsonDecode(filteredPayload);
        final uplinkMessage = data['uplink_message'];
        final frmPayload = uplinkMessage?['frm_payload'];
        final decodedPayload = frmPayload != null
            ? utf8.decode(base64.decode(frmPayload))
            : 'No payload available';

        debugPrint('Decoded payload: $decodedPayload'); // Log decoded payload
        onMessageReceived(decodedPayload, deviceId); // Pass the decoded payload and device ID to the callback
      } catch (e) {
        debugPrint('Error parsing filtered payload: $e'); // Log parsing errors
      }
    });
  }

  // Private method to send a downlink message
  Future<void> _sendDownlinkMessage(String devEui, String payloadData) async {
    final deviceId = _mapDevEuiToDeviceId(devEui); // Map devEUI to device_id
    if (deviceId == 'unknown-device') {
      throw Exception('Device ID not found for DevEUI: $devEui');
    }

    final endpoint = '$baseUrl/api/v3/as/applications/$applicationId/devices/$deviceId/down/push';
    final payload = {
      "downlinks": [
        {
          "f_port": 1,
          "frm_payload": payloadData,
          "confirmed": false,
        }
      ]
    };

    debugPrint('Sending downlink payload to endpoint: $endpoint'); // Log the endpoint
    await _sendData(endpoint, payload);
  }

  // Private method to map devEUI to device_id
  String _mapDevEuiToDeviceId(String devEui) {
    const devEuiToDeviceIdMap = {
      '0004A30B010458CD': 'iot-course-device-1',
      '0004A30B010458CE': 'iot-course-device-2',
      // Add more mappings as needed
    };

    return devEuiToDeviceIdMap[devEui] ?? 'unknown-device';
  }

  // Private method to send data to TTN
  Future<void> _sendData(String endpoint, Map<String, dynamic> data) async {
    final url = Uri.parse(endpoint);
    debugPrint('Sending data to URL: $url'); // Log the full URL
    debugPrint('Payload: ${jsonEncode(data)}'); // Log the payload
    final response = await http.post(
      url,
      headers: {
        'Authorization': 'Bearer $apiKey',
        'Content-Type': 'application/json',
      },
      body: jsonEncode(data),
    );

    debugPrint('HTTP Response: ${response.statusCode} - ${response.body}'); // Log the response
    if (response.statusCode != 200) {
      debugPrint('Error Details: ${response.body}'); // Log error details
      throw Exception('Failed to send data: ${response.statusCode}');
    }
  }

  // Private method to prepare and encode the payload for TTN
  String _preparePayload(String hexPrefix, String message) {
    final prefixedMessage = hexPrefix.isNotEmpty ? '$hexPrefix$message' : message;
    return base64.encode(utf8.encode(prefixedMessage)); // Encode the payload
  }

  // Private method to connect to MQTT
  Future<void> _connectToMqtt(String devEui, Function(String, String) onFilteredMessage) async {
    final client = MqttServerClient.withPort(
      'eu1.cloud.thethings.network',
      '',
      8883,
    );
    client.secure = true;
    client.onBadCertificate = (dynamic certificate) => true;
    client.logging(on: true);
    client.keepAlivePeriod = 20;
    client.setProtocolV311();

    final connMessage = MqttConnectMessage()
        .withProtocolName('MQTT')
        .withProtocolVersion(4)
        .withClientIdentifier('flutterclient${DateTime.now().millisecondsSinceEpoch}')
        .authenticateAs('$applicationId@ttn', apiKey)
        .startClean()
        .withWillQos(MqttQos.atMostOnce);

    client.connectionMessage = connMessage;

    try {
      debugPrint('Connecting to MQTT broker...');
      await client.connect();
      debugPrint('Connected to MQTT broker.');

      final specificTopic = 'v3/$applicationId@ttn/devices/+/up'; // Subscribe to all devices
      debugPrint('Subscribing to topic: $specificTopic');
      client.subscribe(specificTopic, MqttQos.atLeastOnce);

      client.updates!.listen((List<MqttReceivedMessage<MqttMessage>> messages) {
        debugPrint('Message received from broker.');
        final MqttPublishMessage message = messages[0].payload as MqttPublishMessage;
        final payload = MqttPublishPayload.bytesToStringAsString(message.payload.message);
        debugPrint('Raw message received on topic ${messages[0].topic}: $payload');

        try {
          // Parse the JSON payload
          final data = jsonDecode(payload);

          // Extract the `devEUI` and `device_id` from the message
          final endDeviceIds = data['end_device_ids'];
          final messageDevEui = endDeviceIds?['dev_eui'];
          final deviceId = endDeviceIds?['device_id'];

          if (messageDevEui == devEui) {
            debugPrint('Message matches DevEUI: $messageDevEui'); // Log matching DevEUI
            onFilteredMessage(payload, deviceId); // Pass the filtered message and device_id to the callback
          } else {
            debugPrint('Message ignored. DevEUI does not match: $messageDevEui');
          }
        } catch (e) {
          debugPrint('Error parsing payload: $e'); // Log parsing errors
        }
      });

      client.onSubscribed = (String topic) {
        debugPrint('Successfully subscribed to topic: $topic');
      };

      client.onSubscribeFail = (String topic) {
        debugPrint('Failed to subscribe to topic: $topic');
      };

      client.onDisconnected = () {
        debugPrint('Disconnected from MQTT broker.');
      };

      client.onUnsubscribed = (String? topic) {
        debugPrint('Unsubscribed from topic: $topic');
      };
    } catch (e) {
      debugPrint('MQTT connection error: $e');
      throw Exception('MQTT connection error: $e');
    }
  }
}
