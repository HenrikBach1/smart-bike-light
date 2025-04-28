import 'dart:convert';
import 'dart:math';
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

      // Dynamically extract deviceId from the received message
      final deviceId = _lastReceivedDeviceId;
      if (deviceId == null) {
        throw Exception('Device ID not available. Ensure a message has been received before sending.');
      }

      debugPrint('Preparing to send payload data: $prefixedMessage'); // Log the payload data
      await _sendDownlinkMessage(deviceId, payloadData); // Send the message
      debugPrint('Downlink message sent successfully.');
    } catch (e) {
      debugPrint('Error sending message: $e');
      throw Exception('Error sending message: $e');
    }
  }

  /// Sends a message in the format: <to-module>-<data>
  /// 
  /// This is a specialized version of sendMessage that formats the payload
  /// according to the format: <to-module>-<data>
  /// 
  /// [toModule] - Identifier of the target module
  /// [data] - The data to be sent to the module
  /// [devEui] - The device EUI to send the message to
  Future<void> sendFormattedMessage(String devEui, String toModule, String data) async {
    try {
      // Format the message as <to-module>-<data>
      final formattedMessage = '$toModule-$data';
      
      // Dynamically extract deviceId from the received message
      final deviceId = _lastReceivedDeviceId;
      if (deviceId == null) {
        throw Exception('Device ID not available. Ensure a message has been received before sending.');
      }

      // Encode the payload
      final payloadData = base64.encode(utf8.encode(formattedMessage));
      
      debugPrint('Preparing to send formatted payload: $formattedMessage');
      await _sendDownlinkMessage(deviceId, payloadData);
      debugPrint('Formatted downlink message sent successfully.');
    } catch (e) {
      debugPrint('Error sending formatted message: $e');
      throw Exception('Error sending formatted message: $e');
    }
  }

  String? _lastReceivedDeviceId; // Store the last received device ID

  Future<Map<String, double>> computeDeviceLocation(String devEui, List<Map<String, dynamic>> gatewaysData) async {
    if (gatewaysData.isEmpty) {
      throw Exception('No gateway data available for location computation.');
    }

    return Trilateration.calculatePosition(gatewaysData);
  }

  // Public method to receive messages
  Future<void> receiveMessage(String devEui, Function(String, String, List<Map<String, dynamic>>?, Map<String, double>?) onMessageReceived) async {
    await _connectToMqtt(devEui, (filteredPayload, deviceId) async {
      try {
        final data = jsonDecode(filteredPayload);
        final uplinkMessage = data['uplink_message'];
        final frmPayload = uplinkMessage?['frm_payload'];
        final decodedPayload = frmPayload != null
            ? utf8.decode(base64.decode(frmPayload))
            : 'No payload available';

        final rxMetadataList = uplinkMessage?['rx_metadata'] ?? [];
        List<Map<String, dynamic>> gatewaysData = [];

        for (var metadata in rxMetadataList) {
          final gatewayId = metadata['gateway_ids']?['gateway_id'];
          final location = metadata['location'];
          final rssi = metadata['rssi'] ?? 0;

          if (gatewayId != null && location != null) {
            gatewaysData.add({
              'gateway_id': gatewayId,
              'latitude': location['latitude'],
              'longitude': location['longitude'],
              'rssi': rssi,
            });
          }
        }

        Map<String, double>? devicePosition;
        if (gatewaysData.isNotEmpty) {
          devicePosition = await computeDeviceLocation(devEui, gatewaysData);
        }

        // Pass the entire list of gateways to the UI instead of just the first one
        onMessageReceived(decodedPayload, deviceId, gatewaysData.isNotEmpty ? gatewaysData : null, devicePosition);
      } catch (e) {
        debugPrint('Error parsing filtered payload: $e');
      }
    });
  }

  // Public method to query gateway coordinates
  Future<Map<String, dynamic>> queryGatewayCoordinates(String gatewayId) async {
    final endpoint = '$baseUrl/api/v3/gateways/$gatewayId';
    final url = Uri.parse(endpoint);

    try {
      debugPrint('Querying gateway coordinates for Gateway ID: $gatewayId');
      final response = await http.get(
        url,
        headers: {
          'Authorization': 'Bearer $apiKey',
          'Content-Type': 'application/json',
        },
      );

      if (response.statusCode == 200) {
        final data = jsonDecode(response.body);
        final coordinates = data['location']?['latitude'] != null && data['location']?['longitude'] != null
            ? {
                'latitude': data['location']['latitude'],
                'longitude': data['location']['longitude'],
                'gateway_id': gatewayId, // Include gateway_id in the returned data
              }
            : throw Exception('Coordinates not found for Gateway ID: $gatewayId');

        debugPrint('Gateway coordinates retrieved: $coordinates');
        return coordinates;
      } else {
        debugPrint('Failed to query gateway coordinates: ${response.body}');
        throw Exception('Failed to query gateway coordinates: ${response.statusCode}');
      }
    } catch (e) {
      debugPrint('Error querying gateway coordinates: $e');
      throw Exception('Error querying gateway coordinates: $e');
    }
  }

  // Private method to send a downlink message
  Future<void> _sendDownlinkMessage(String deviceId, String payloadData) async {
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
            _lastReceivedDeviceId = deviceId; // Store the device ID for downlink messages
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

class Trilateration {
  static const double _pathLossExponent = 2.5; // Typical value for indoor environments
  static const int _txPower = -59; // Calibrated transmit power at 1 meter in dBm

  // Convert RSSI to estimated distance in meters
  static double rssiToDistance(int rssi) {
    // Log-distance path loss model: distance = 10^((txPower - RSSI)/(10 * n))
    // where n is the path loss exponent (typically 2-4)
    return pow(10, (_txPower - rssi) / (10 * _pathLossExponent)).toDouble();
  }

  // Calculate device position using RSSI-based trilateration
  static Map<String, double> calculatePosition(List<Map<String, dynamic>> gatewaysData) {
    if (gatewaysData.isEmpty) {
      throw Exception('No gateway data available for trilateration.');
    }

    if (gatewaysData.length == 1) {
      // If only one gateway is available, estimate position based on RSSI
      final gateway = gatewaysData.first;
      final double distance = rssiToDistance(gateway['rssi']);
      debugPrint('Estimated distance from gateway: $distance meters');
      
      // Return the gateway's location as an approximation
      return {
        'latitude': gateway['latitude'],
        'longitude': gateway['longitude'],
        'accuracy': distance, // Include accuracy estimate
      };
    }

    // For multiple gateways, weighted centroid method
    // Use RSSI values as weights for position estimation
    double totalWeight = 0;
    double weightedLatitude = 0;
    double weightedLongitude = 0;

    for (var gateway in gatewaysData) {
      // Convert RSSI to a weight (stronger signal = higher weight)
      final weight = pow(10, (gateway['rssi'] / 20)).toDouble(); // Higher RSSI = more weight
      weightedLatitude += gateway['latitude'] * weight;
      weightedLongitude += gateway['longitude'] * weight;
      totalWeight += weight;
    }

    if (totalWeight > 0) {
      return {
        'latitude': weightedLatitude / totalWeight,
        'longitude': weightedLongitude / totalWeight,
        'accuracy': gatewaysData.length > 2 ? 50.0 : 150.0, // Rough accuracy estimate
      };
    }

    // Fallback to simple average if weights are problematic, like if only one gateway present
    debugPrint('No valid weights found, falling back to average position.');
    return {
      'latitude': gatewaysData.map((g) => g['latitude']).reduce((a, b) => a + b) / gatewaysData.length,
      'longitude': gatewaysData.map((g) => g['longitude']).reduce((a, b) => a + b) / gatewaysData.length,
      'accuracy': 200.0, // Low accuracy for fallback method
    };
  }
}
