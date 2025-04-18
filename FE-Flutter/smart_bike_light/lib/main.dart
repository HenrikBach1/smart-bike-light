import 'package:flutter/material.dart';
import 'package:smart_bike_light/api/ttn_api.dart'; // Import TTN API module

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
  late final TTNApi _ttnApi; // Use TTNApi for API interactions

  @override
  void initState() {
    super.initState();
    _ttnApi = TTNApi.createDefault(); // Use the factory method to initialize TTNApi
  }

  Future<void> receiveMessages() async {
    final devEui = _devEuiController.text.trim();
    if (devEui.isEmpty) {
      setState(() {
        _deviceData = 'Error: DevEUI cannot be empty.';
      });
      return;
    }

    setState(() {
      _isLoading = true;
      _deviceData = 'Receiving messages...';
    });

    try {
      await _ttnApi.receiveMessage(devEui, (decodedPayload, deviceId, gatewayCoordinates) {
        final gatewayId = gatewayCoordinates != null ? gatewayCoordinates['gateway_id'] : 'Unknown';
        final gatewayInfo = gatewayCoordinates != null
            ? 'Lat: ${gatewayCoordinates['latitude']}, Lon: ${gatewayCoordinates['longitude']}'
            : 'Not available';

        setState(() {
          _deviceData = '''
Received payload:
- Device ID: $deviceId
- Decoded Payload: $decodedPayload
- Gateway ID: $gatewayId
- Gateway Coordinates: $gatewayInfo
''';
        });
      });
    } catch (e) {
      setState(() {
        _deviceData = 'Error receiving messages: $e';
      });
    } finally {
      setState(() {
        _isLoading = false;
      });
    }
  }

  Future<void> sendMessage() async {
    final hexPrefix = _hexPrefixController.text.trim();
    final devEui = _devEuiController.text.trim();
    final message = _messageController.text.trim();

    if (devEui.isEmpty || message.isEmpty) {
      setState(() {
        _deviceData = 'Error: DevEUI and message cannot be empty.';
      });
      return;
    }

    setState(() {
      _isLoading = true;
      _deviceData = 'Sending message...';
    });

    try {
      await _ttnApi.sendMessage(devEui, hexPrefix, message);
      setState(() {
        _deviceData = 'Message sent successfully.';
      });
    } catch (e) {
      setState(() {
        _deviceData = 'Error sending message: $e';
      });
    } finally {
      setState(() {
        _isLoading = false;
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
              onPressed: receiveMessages,
              child: const Text('Receive Messages'),
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
              onPressed: sendMessage,
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
