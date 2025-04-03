import 'package:flutter/material.dart';
import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_server_client.dart'; // Use only MqttServerClient
import 'dart:convert';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Smart Bike Light',
      theme: ThemeData(primarySwatch: Colors.blue),
      home: const DeviceDataScreen(),
    );
  }
}

class DeviceDataScreen extends StatefulWidget {
  const DeviceDataScreen({super.key});

  @override
  DeviceDataScreenState createState() => DeviceDataScreenState();
}

class DeviceDataScreenState extends State<DeviceDataScreen> {
  final TextEditingController _devEuiController = TextEditingController();
  final TextEditingController _messageController = TextEditingController(); // Controller for the message input field
  String _deviceData = '';
  bool _isLoading = false;
  MqttServerClient? _client; // Use MqttServerClient for native platforms
  static const int maxRetries = 5; // Maximum retry attempts
  int retryCount = 0; // Current retry count
  String _lastSentMessage = ''; // Store the last sent message
  String _lastEncodedMessage = ''; // Store the last encoded message

  Future<void> connectToMqtt(String devEui) async {
    setState(() {
      _isLoading = true;
      _deviceData = 'Connecting to MQTT broker...';
    });

    // Configure MqttServerClient with SSL/TLS
    final client = MqttServerClient.withPort(
      'eu1.cloud.thethings.network',
      '',
      8883, // Secure TCP port
    );
    client.secure = true; // Enable SSL/TLS
    client.onBadCertificate = (dynamic certificate) => true; // Allow self-signed certificates
    client.logging(on: true); // Enable detailed logging
    client.keepAlivePeriod = 20;
    client.setProtocolV311(); // Explicitly set protocol to MQTT v3.1.1
    client.connectTimeoutPeriod = 20000; // Set timeout to 20 seconds

    client.onConnected = () {
      debugPrint('Connected to MQTT broker');
      setState(() {
        _deviceData = 'Connected to MQTT broker. Subscribing to topic...';
      });
    };

    client.onDisconnected = () {
      debugPrint('Disconnected from MQTT broker');
      setState(() {
        _deviceData = 'Disconnected from MQTT broker.';
      });
    };

    client.onSubscribed = (String topic) {
      debugPrint('Successfully subscribed to topic: $topic');
      setState(() {
        _deviceData = 'Successfully subscribed to topic: $topic';
      });
    };

    client.onSubscribeFail = (String topic) async {
      debugPrint('Failed to subscribe to topic: $topic');
      setState(() {
        _deviceData = 'Failed to subscribe to topic: $topic. Retrying... ($retryCount/$maxRetries)';
      });

      if (retryCount < maxRetries) {
        retryCount++;
        debugPrint('Retrying subscription for topic: $topic (Attempt $retryCount)');
        final specificTopic = 'v3/smart-bike-light-henrikbach1@ttn/devices/iot-course-device-1/up';
        client.subscribe(specificTopic, MqttQos.atLeastOnce);
      } else {
        debugPrint('Max retry attempts reached for topic: $topic');
        setState(() {
          _deviceData = 'Max retry attempts reached for topic: $topic. Please check permissions or topic format.';
        });
      }
    };

    final connMessage = MqttConnectMessage()
        .withProtocolName('MQTT') // Use 'MQTT' for v3.1.1
        .withProtocolVersion(4) // Protocol version 4 is MQTT v3.1.1
        .withClientIdentifier('flutterclient${DateTime.now().millisecondsSinceEpoch}')
        .authenticateAs(
          'smart-bike-light-henrikbach1@ttn', // Application ID with @ttn suffix
          'NNSXS.2RRIRHET3ST3AKV7YMQKLQNPWAYDGME5B6ILBHA.EZUBU7VTBNBHP4J45MZYX6674P6SUTTO2Z4YJ2D6CEMFFEYS3B4Q', // API Key
        )
        .startClean()
        .withWillQos(MqttQos.atMostOnce);

    client.connectionMessage = connMessage;

    try {
      await client.connect();
      retryCount = 0; // Reset retry count on successful connection

      // Attempt to subscribe to the specific device topic
      final specificTopic = 'v3/smart-bike-light-henrikbach1@ttn/devices/iot-course-device-1/up';
      client.subscribe(specificTopic, MqttQos.atLeastOnce);

      client.updates!.listen((List<MqttReceivedMessage<MqttMessage>> messages) {
        final MqttPublishMessage message = messages[0].payload as MqttPublishMessage;
        final payload = MqttPublishPayload.bytesToStringAsString(message.payload.message);

        debugPrint('Raw MQTT message received: $payload'); // Log the raw payload

        try {
          // Parse the JSON payload
          final data = json.decode(payload);
          debugPrint('Parsed JSON data: $data'); // Log the parsed JSON

          final uplinkMessage = data['uplink_message'];
          final receivedAt = uplinkMessage?['received_at'] ?? 'N/A';
          final frmPayload = uplinkMessage?['frm_payload'] ?? 'N/A';
          final decodedPayload = utf8.decode(base64.decode(frmPayload));

          // Update the UI with the latest uplink data
          setState(() {
            _deviceData = '''
Latest Uplink Data:
- Received At: $receivedAt
- Decoded Payload: $decodedPayload
''';
          });
        } catch (e) {
          debugPrint('Error parsing uplink data: $e');
          setState(() {
            _deviceData = 'Error parsing uplink data: $e';
          });
        }
      });
    } catch (e) {
      debugPrint('Connection error: $e');
      setState(() {
        _deviceData = 'Connection error: $e. Please verify API key and broker settings.';
      });
    } finally {
      setState(() {
        _isLoading = false;
      });
    }

    _client = client; // Assign the client to the common base class
  }

  Future<void> sendMessage(String message) async {
    if (_client != null && _client!.connectionStatus!.state == MqttConnectionState.connected) {
      final topic = 'v3/smart-bike-light-henrikbach1@ttn/devices/iot-course-device-1/down/push'; // Corrected downlink topic
      final encodedMessage = utf8.encode(message).map((byte) => byte.toRadixString(16).padLeft(2, '0').toUpperCase()).join(''); // Encode the message in uppercase hex
      final payload = json.encode({
        "downlinks": [
          {
            "f_port": 1, // Replace with the correct port if needed
            "frm_payload": base64.encode(utf8.encode(message)), // Use Base64 for the actual payload
            "confirmed": false // Set to true if a confirmed downlink is required
          }
        ]
      });

      final builder = MqttClientPayloadBuilder();
      builder.addString(payload);

      try {
        _client!.publishMessage(topic, MqttQos.atLeastOnce, builder.payload!);
        debugPrint('Message sent to topic $topic: $payload');
        setState(() {
          _lastSentMessage = message; // Update the last sent message
          _lastEncodedMessage = encodedMessage; // Update the last encoded message
          _deviceData = '''
Message sent:
- Original: $message
- Encoded (Hex): $encodedMessage
''';
        });
      } catch (e) {
        debugPrint('Error sending message: $e');
        setState(() {
          _deviceData = 'Error sending message: $e';
        });
      }
    } else {
      debugPrint('Client not connected. Cannot send message.');
      setState(() {
        _deviceData = 'Client not connected. Cannot send message.';
      });
    }
  }

  @override
  void dispose() {
    _client?.disconnect();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Smart Bike Light')),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: SingleChildScrollView( // Wrap the Column in a scrollable widget
          child: Column(
            children: [
              TextField(
                controller: _devEuiController,
                decoration: const InputDecoration(
                  labelText: 'Enter Device EUI (DevEUI)',
                  border: OutlineInputBorder(),
                ),
              ),
              const SizedBox(height: 16),
              ElevatedButton(
                onPressed: () {
                  final devEui = _devEuiController.text.trim();
                  if (devEui.isNotEmpty) {
                    connectToMqtt(devEui);
                  }
                },
                child: const Text('Connect and Fetch Data'),
              ),
              const SizedBox(height: 16),
              TextField(
                controller: _messageController,
                decoration: const InputDecoration(
                  labelText: 'Enter Message to Send',
                  border: OutlineInputBorder(),
                ),
              ),
              const SizedBox(height: 16),
              ElevatedButton(
                onPressed: () {
                  final message = _messageController.text.trim();
                  if (message.isNotEmpty) {
                    sendMessage(message);
                  }
                },
                child: const Text('Send Message'),
              ),
              const SizedBox(height: 16),
              if (_lastSentMessage.isNotEmpty) ...[
                Text(
                  'Last Sent Message:',
                  style: const TextStyle(fontWeight: FontWeight.bold),
                ),
                Text('Original: $_lastSentMessage'),
                Text('Encoded (Hex): $_lastEncodedMessage'),
                const SizedBox(height: 16),
              ],
              _isLoading
                  ? const CircularProgressIndicator()
                  : Text(
                      _deviceData,
                      style: const TextStyle(fontSize: 14),
                    ),
            ],
          ),
        ),
      ),
    );
  }
}
