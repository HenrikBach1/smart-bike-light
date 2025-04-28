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

  /// Parses the payload string into its components
  /// Format: <to-module>-<from-module-status>-<from-module-data>
  Map<String, String> _parsePayload(String payload) {
    final result = {
      'toModule': 'N/A',
      'fromModuleStatus': 'N/A',
      'fromModuleData': 'N/A',
    };
    
    if (payload.isEmpty) {
      return result;
    }
    
    final parts = payload.split('-');
    
    if (parts.length >= 1) {
      result['toModule'] = parts[0];
    }
    
    if (parts.length >= 2) {
      result['fromModuleStatus'] = parts[1];
    }
    
    if (parts.length >= 3) {
      // Join any remaining parts in case the data itself contains hyphens
      result['fromModuleData'] = parts.sublist(2).join('-');
    }
    
    return result;
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
      await _ttnApi.receiveMessage(devEui, (decodedPayload, deviceId, gatewaysInfo, devicePosition) {
        // Format gateway information for multiple gateways
        String gatewaysText = 'None detected';
        
        if (gatewaysInfo != null && gatewaysInfo.isNotEmpty) {
          gatewaysText = '';
          for (int i = 0; i < gatewaysInfo.length; i++) {
            final gateway = gatewaysInfo[i];
            final gatewayId = gateway['gateway_id'];
            final latitude = gateway['latitude']?.toStringAsFixed(6) ?? 'N/A';
            final longitude = gateway['longitude']?.toStringAsFixed(6) ?? 'N/A';
            final rssi = gateway['rssi'];
            
            // Calculate approximate distance using the RSSI value
            final distance = Trilateration.rssiToDistance(rssi).toStringAsFixed(2);
            
            gatewaysText += '''
  Gateway ${i+1}:
   - ID: $gatewayId
   - Location: Lat: $latitude, Lon: $longitude
   - RSSI: $rssi dBm
   - Estimated Distance: $distance m
''';
          }
        }
            
        // Format device position information with null checks
        final devicePositionText = devicePosition != null
            ? 'Lat: ${devicePosition['latitude']?.toStringAsFixed(6) ?? 'N/A'}, Lon: ${devicePosition['longitude']?.toStringAsFixed(6) ?? 'N/A'}, Accuracy: ${devicePosition['accuracy']?.toStringAsFixed(1) ?? 'N/A'} m'
            : 'Not available';

        // Parse the payload
        final parsedPayload = _parsePayload(decodedPayload);

        setState(() {
          _deviceData = '''
DEVICE INFORMATION:
- Device ID: $deviceId
- Decoded Payload: $decodedPayload
- To Module: ${parsedPayload['toModule']}
- From Module Status: ${parsedPayload['fromModuleStatus']}
- From Module Data: ${parsedPayload['fromModuleData']}

NETWORK INFORMATION:
- Gateways (${gatewaysInfo?.length ?? 0}):
$gatewaysText
- Calculated Device Position: $devicePositionText
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

  Future<void> sendFormattedMessage() async {
    final toModule = _hexPrefixController.text.trim(); // Using hexPrefixController for to-module
    final data = _messageController.text.trim(); // Using messageController for data
    final devEui = _devEuiController.text.trim();

    if (devEui.isEmpty || toModule.isEmpty || data.isEmpty) {
      setState(() {
        _deviceData = 'Error: DevEUI, to-module, and data cannot be empty.';
      });
      return;
    }

    setState(() {
      _isLoading = true;
      _deviceData = 'Sending formatted message...';
    });

    try {
      await _ttnApi.sendFormattedMessage(devEui, toModule, data);
      setState(() {
        _deviceData = 'Formatted message sent successfully.';
      });
    } catch (e) {
      setState(() {
        _deviceData = 'Error sending formatted message: $e';
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
                labelText: 'To Module',
                border: OutlineInputBorder(),
                hintText: 'Enter target module identifier',
              ),
            ),
            const SizedBox(height: 16),
            TextField(
              controller: _messageController,
              decoration: const InputDecoration(
                labelText: 'Data',
                border: OutlineInputBorder(),
                hintText: 'Enter data to send to the module',
              ),
            ),
            const SizedBox(height: 16),
            ElevatedButton(
              onPressed: sendFormattedMessage,
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
