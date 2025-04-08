import 'dart:convert';
import 'package:http/http.dart' as http;
import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_server_client.dart';
import 'package:flutter/foundation.dart';

class TTNApi {
  final String baseUrl;
  final String apiKey;

  TTNApi({required this.baseUrl, required this.apiKey});

  // Fetch data from TTN
  Future<Map<String, dynamic>> fetchData(String endpoint) async {
    final url = Uri.parse('$baseUrl/$endpoint');
    debugPrint('Fetching data from URL: $url'); // Log the full URL
    final response = await http.get(
      url,
      headers: {
        'Authorization': 'Bearer $apiKey',
        'Content-Type': 'application/json',
      },
    );

    if (response.statusCode == 200) {
      return jsonDecode(response.body);
    } else {
      debugPrint('Failed to fetch data. Status code: ${response.statusCode}, Body: ${response.body}'); // Log error details
      throw Exception('Failed to fetch data: ${response.statusCode}');
    }
  }

  // Send data to TTN
  Future<void> sendData(String endpoint, Map<String, dynamic> data) async {
    final url = Uri.parse('$baseUrl/$endpoint');
    final response = await http.post(
      url,
      headers: {
        'Authorization': 'Bearer $apiKey',
        'Content-Type': 'application/json',
      },
      body: jsonEncode(data),
    );

    if (response.statusCode != 200) {
      throw Exception('Failed to send data: ${response.statusCode}');
    }
  }

  Future<void> connectToMqtt(String applicationId, String devEui, Function(String) onMessage) async {
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

      final specificTopic = 'v3/$applicationId@ttn/devices/$devEui/up';
      debugPrint('Subscribing to topic: $specificTopic');
      client.subscribe(specificTopic, MqttQos.atLeastOnce);

      client.updates!.listen((List<MqttReceivedMessage<MqttMessage>> messages) {
        debugPrint('Message received from broker.');
        final MqttPublishMessage message = messages[0].payload as MqttPublishMessage;
        final payload = MqttPublishPayload.bytesToStringAsString(message.payload.message);
        debugPrint('Raw message received on topic ${messages[0].topic}: $payload');
        try {
          debugPrint('Passing payload to onMessage callback...');
          onMessage(payload);
        } catch (e) {
          debugPrint('Error in onMessage callback: $e');
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

  // Send MQTT message
  Future<void> sendMqttMessage(MqttServerClient client, String topic, String payload) async {
    final builder = MqttClientPayloadBuilder();
    builder.addString(payload);

    try {
      debugPrint('Preparing to send message to topic $topic: $payload');
      client.publishMessage(topic, MqttQos.atLeastOnce, builder.payload!);
      debugPrint('Message successfully sent to topic $topic.');
    } catch (e) {
      debugPrint('Error sending MQTT message: $e');
      throw Exception('Error sending MQTT message: $e');
    }
  }
}
