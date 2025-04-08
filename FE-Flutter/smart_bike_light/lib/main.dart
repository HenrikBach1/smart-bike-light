import 'package:flutter/material.dart';
import 'package:smart_bike_light/api/ttn_api.dart'; // Import TTN API module
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
      home: const TabbedHomePage(),
    );
  }
}

class TabbedHomePage extends StatelessWidget {
  const TabbedHomePage({super.key});

  @override
  Widget build(BuildContext context) {
    return DefaultTabController(
      length: 1,
      child: Scaffold(
        appBar: AppBar(
          title: const Text('Smart Bike Light'),
          bottom: const TabBar(
            tabs: [
              Tab(text: 'Test'),
            ],
          ),
        ),
        body: const TabBarView(
          children: [
            TestPage(),
          ],
        ),
      ),
    );
  }
}

class TestPage extends StatefulWidget {
  const TestPage({super.key});

  @override
  TestPageState createState() => TestPageState();
}

class TestPageState extends State<TestPage> {
  final TextEditingController _devEuiController = TextEditingController();
  final TextEditingController _messageController = TextEditingController();
  final TextEditingController _hexPrefixController = TextEditingController();
  String _deviceData = '';
  bool _isLoading = false;
  TTNApi? _ttnApi; // Use TTNApi for API interactions

  @override
  void initState() {
    super.initState();
    _ttnApi = TTNApi(
      baseUrl: 'https://eu1.cloud.thethings.network',
      apiKey: 'NNSXS.2RRIRHET3ST3AKV7YMQKLQNPWAYDGME5B6ILBHA.EZUBU7VTBNBHP4J45MZYX6674P6SUTTO2Z4YJ2D6CEMFFEYS3B4Q',
    );
  }

  Future<void> connectToMqtt(String devEui) async {
    setState(() {
      _isLoading = true;
      _deviceData = 'Connecting to MQTT broker...';
    });

    try {
      final applicationId = 'smart-bike-light-henrikbach1'; // Replace with your application ID
      await _ttnApi!.connectToMqtt(applicationId, devEui, (payload) {
        debugPrint('Raw payload received: $payload'); // Log raw payload
        try {
          // Parse the payload as JSON
          final data = jsonDecode(payload);
          debugPrint('Parsed data: $data'); // Log parsed data
          setState(() {
            _deviceData = 'Received payload: $data';
          });
        } catch (e) {
          debugPrint('Error parsing payload: $e'); // Log parsing errors
          setState(() {
            _deviceData = 'Error parsing payload: $e';
          });
        }
      });
    } catch (e) {
      setState(() {
        _deviceData = 'Error connecting to MQTT: $e';
      });
    } finally {
      setState(() {
        _isLoading = false;
      });
    }
  }

  Future<void> sendMessage(String message) async {
    try {
      final hexPrefix = _hexPrefixController.text.trim();
      final prefixedMessage = hexPrefix.isNotEmpty ? '$hexPrefix$message' : message;
      final payload = {
        "downlinks": [
          {
            "f_port": 1,
            "frm_payload": base64.encode(utf8.encode(prefixedMessage)),
            "confirmed": false,
          }
        ]
      };

      await _ttnApi!.sendData('v3/devices/down/push', payload);
      setState(() {
        _deviceData = 'Message sent: $prefixedMessage';
      });
    } catch (e) {
      setState(() {
        _deviceData = 'Error sending message: $e';
      });
    }
  }

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.all(16.0),
      child: SingleChildScrollView(
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
              controller: _hexPrefixController,
              decoration: const InputDecoration(
                labelText: 'Module Number in Hex (1 Byte)',
                border: OutlineInputBorder(),
              ),
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
            _isLoading
                ? const CircularProgressIndicator()
                : Text(
                    _deviceData,
                    style: const TextStyle(fontSize: 14),
                  ),
          ],
        ),
      ),
    );
  }
}
