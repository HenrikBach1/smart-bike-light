#include "LedControl.h"
#include "Network.h" // Include Network.h (which includes Pins.h)
#include <Arduino.h> // Include Arduino library for Base64 decoding
#include <base64.h> // Include Base64 library for decoding
#include <math.h> // For geolocation calculations
#include <rn2xx3.h> // Include the rn2xx3 library for RN2483
#include <TheThingsNetwork.h> // Include TheThingsNetwork library for metadata-rich messages

// Forward declarations of functions local to this module
void initialize_lora_module_and_retrieve_info();
void join_TTN(const char* appEui, const char* appKey);
void messageCallback_ttn(const uint8_t* payload, size_t size, port_t port);
void processDownlinkMessage(const String& rawMsg);
void hexDecode(const String& hexInput, char* output); // Add forward declaration for hexDecode

// OTAA credentials
const char *_appEui = "0000000000000000"; // Replace with your Application EUI
const char *_appKey = "5EA67FF029810B31D0805D4749AA682E"; // Replace with your Application Key

HardwareSerial mySerial(UART); // Initialize UART1 for rn2xx3
rn2xx3 myLora(mySerial); // Initialize LoRa instance

#define loraSerial Serial1
#define freqPlan TTN_FP_EU868

// Initialize The Things Network object for metadata handling
TheThingsNetwork ttn(loraSerial, Serial, freqPlan);

//Module wide variables
String message = "";       // Received message
String rawMessage = "";    // Public global variable to store the received downlink message
String _devEUI = "";       // Define the devEUI variable
String metadataJson = "";  // Store raw metadata from TTN for geolocation
volatile bool messageAcknowledged = false; // Flag for message acknowledgment

void initialize_LoRaWAN() {
    // Initialize rn2xx3 library first for device info
    mySerial.setRxBufferSize(1024);          
    mySerial.begin(57600, SERIAL_8N1, RX, TX); 
    mySerial.setTimeout(1000);              
    Serial.println("Serial initialized for rn2xx3.");

    // Initialize TTN library serial
    loraSerial.begin(57600);
    Serial.println("Serial initialized for TTN.");

    // Get device info using rn2xx3 first
    initialize_lora_module_and_retrieve_info();
    
    // Then join TTN using the parameters
    join_TTN(_appEui, _appKey);
}

// Use rn2xx3 to get device information only
void initialize_lora_module_and_retrieve_info() {
    // Reset RN2xx3
    pinMode(RST, OUTPUT);
    digitalWrite(RST, LOW);
    delay(100);
    digitalWrite(RST, HIGH);
    delay(100); 
    mySerial.flush();

    // Get hardware EUI
    String hweui = myLora.hweui();
    while (hweui.length() != 16) {
      Serial.println("Communication with RN2xx3 unsuccessful.");
      delay(1000);
      hweui = myLora.hweui();
    }
    Serial.println("Communication with RN2xx3 successful.");

    // Store device information
    _devEUI = hweui;
    Serial.println("DevEUI: " + _devEUI);
    String firmware = myLora.sysver();
    Serial.println("Firmware: " + firmware);
}

// Join TTN using the rn2xx3 library
void join_TTN(const char* appEui, const char* appKey) {
  // Check credentials before joining
  if (appEui == nullptr || appKey == nullptr) {
    Serial.println("Error: AppEUI or AppKey is null");
    return;
  }
      
  Serial.println("Joining TTN using rn2xx3 library...");
    
  // Add delay to ensure hardware is ready
  delay(1000);
    
  Serial.println("Attempting to join TTN with provided credentials...");
    
  // Check if credentials are valid strings
  if (appEui != nullptr && appKey != nullptr) {
    Serial.println("Using AppEUI: " + String(appEui));
    // Don't print the full AppKey for security
    Serial.println("Using AppKey: " + String(appKey).substring(0, 6) + "...");
    
    // Try joining with retry logic
    bool join_result = false;
    for (int attempts = 0; attempts < 3; attempts++) {
        Serial.println("Join attempt " + String(attempts + 1) + "/3");
        
        // Use only the rn2xx3 library for joining - this is known to work
        join_result = myLora.initOTAA(appEui, appKey);
        
        if (join_result) {
            Serial.println("Successfully joined TTN!");
            break;
        } else {
            Serial.println("Join failed. Retrying after delay...");
            delay(5000); // Shorter delay for testing
        }
    }
    
    if (!join_result) {
        Serial.println("Failed to join TTN after multiple attempts.");
    }
  } else {
    Serial.println("Error: AppEUI or AppKey is null");
  }
}

// Process downlink messages with metadata if TTN library is used
void messageCallback_ttn(const uint8_t* payload, size_t size, port_t port) {
    Serial.println("Received downlink message with metadata from TTN library.");
    
    // Store the raw message
    rawMessage = "";
    for (size_t i = 0; i < size; ++i) {
        rawMessage += String((char)payload[i]); // rawMessage may be a json string containing metadata or just hex decimal value string
    }
    processDownlinkMessage(rawMessage); // Process the downlink message
    messageAcknowledged = true;
    
    Serial.println("Raw message: " + rawMessage);
}

// Check if received message is JSON
void processDownlinkMessage(const String& rawMsg) {
    Serial.println("Processing downlink message: " + rawMsg);
    
    // Check if the message starts with '{' which would indicate JSON
    if (rawMsg.length() > 0 && rawMsg.charAt(0) == '{') {
        Serial.println("Detected JSON format in downlink message");
        metadataJson = rawMsg; // Store the JSON for later processing
        
        // TODO: Parse JSON to extract gateway information
        // This would be where we extract gateway data for location estimation
        // For now, just indicate we received a JSON message
        Serial.println("JSON metadata available for geolocation");
    } else {
        // Not JSON, handle as regular message
        char asciiMessage[256]; // Adjust size as needed
        hexDecode(rawMsg, asciiMessage);
        message = String(asciiMessage); // Convert to String
        
        Serial.print("ASCII message received: ");
        Serial.println(message);
    }
}

// Function to estimate location from metadata
void estimateLocationFromMetadata(const String& metadata) {
  // Parse metadata to extract gateway information (example format: "gateway1_lat,gateway1_lon,rssi1;gateway2_lat,gateway2_lon,rssi2")
  struct Gateway {
    float latitude;
    float longitude;
    int rssi;
  };

  std::vector<Gateway> gateways;
  size_t start = 0, end;
  while ((end = metadata.indexOf(';', start)) != -1) {
    String gatewayData = metadata.substring(start, end);
    size_t latEnd = gatewayData.indexOf(',');
    size_t lonEnd = gatewayData.indexOf(',', latEnd + 1);

    Gateway gateway;
    gateway.latitude = gatewayData.substring(0, latEnd).toFloat();
    gateway.longitude = gatewayData.substring(latEnd + 1, lonEnd).toFloat();
    gateway.rssi = gatewayData.substring(lonEnd + 1).toInt();

    gateways.push_back(gateway);
    start = end + 1;
  }

  // Perform trilateration using the parsed gateway data
  Serial.println("Performing trilateration using the parsed informations from gateways: " + String(gateways.size()));
  float estimatedLat = 0.0;
  float estimatedLon = 0.0;
  int totalWeight = 0;

  for (const auto& gateway : gateways) {
    int weight = -gateway.rssi; // Use RSSI as weight (higher RSSI = closer)
    estimatedLat += gateway.latitude * weight;
    estimatedLon += gateway.longitude * weight;
    totalWeight += weight;

    Serial.println("Actual coordinates and weight from metadata:");
    Serial.println("Weight: " + String(weight, 6));
    Serial.println("Latitude: " + String(estimatedLat, 6));
    Serial.println("Longitude: " + String(estimatedLon, 6));  
  }

  if (totalWeight == 0) {
    Serial.println("No gateway coordinates found in metadata. Unable to estimate location.");
    return; // Exit the function if no valid data is available
  }

  estimatedLat /= totalWeight;
  estimatedLon /= totalWeight;

  Serial.println("Estimated coordinates from metadata:");
  Serial.println("Latitude: " + String(estimatedLat, 6));
  Serial.println("Longitude: " + String(estimatedLon, 6));
}

// Custom Base64 decoding function
int base64Decode(const char* input, unsigned char* output, int inputLength) {
    const char* base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int outputIndex = 0, buffer = 0, bitsCollected = 0;

    for (int i = 0; i < inputLength; ++i) {
        char c = input[i];
        if (c == '=') break; // Padding character, stop processing
        const char* pos = strchr(base64Chars, c);
        if (!pos) continue; // Skip invalid characters
        buffer = (buffer << 6) | (pos - base64Chars);
        bitsCollected += 6;
        if (bitsCollected >= 8) {
            bitsCollected -= 8;
            output[outputIndex++] = (buffer >> bitsCollected) & 0xFF;
        }
    }
    return outputIndex; // Return the length of the decoded data
}

// Function to decode a hexadecimal string to ASCII
void hexDecode(const String& hexInput, char* output) {
    int length = hexInput.length();
    for (int i = 0; i < length; i += 2) {
        String byteString = hexInput.substring(i, i + 2);
        char byte = (char)strtol(byteString.c_str(), nullptr, 16);
        output[i / 2] = byte;
    }
    output[length / 2] = '\0'; // Null-terminate the decoded string
}

// Updated tranceive function to use rn2xx3 library for sending data and receiving messages
TX_RETURN_TYPE tranceive(Module module, Status status, const char* data) {
    // Convert the module to a one-byte hex prefix
    char prefixedData[256]; // Adjust size as needed
    rawMessage = ""; // Clear the raw message
    message = ""; // Clear the message
    TX_RETURN_TYPE response = TX_FAIL; // Initialize response to failure

    // Format the data to send
    snprintf(prefixedData, sizeof(prefixedData), "%02X%s", (uint8_t)module, data);
    Serial.println("Sending: " + String(prefixedData));

    // Validate data length
    if (strlen(prefixedData) == 0) {
        Serial.println("Error: Data to send is empty.");
        return TX_FAIL;
    }

    // Send using rn2xx3 library - avoid using TTN library which is causing crashes
    Serial.println("Sending message via LoRa...");
    response = myLora.tx(prefixedData);

    // Print the prefixed data for debugging
    Serial.print("ASCII message sent: ");
    Serial.println(prefixedData);

    // Get received message
    rawMessage = myLora.getRx();

    Serial.print("ASCII message received: ");
    Serial.println(message);

    // Process the downlink message, checking for JSON format
    if (rawMessage.length() > 0) {
        processDownlinkMessage(rawMessage);
        response = TX_SUCCESS; // Message received
    }
    
    // In LoRaWAN, we need to respect duty cycle - wait a bit
    delay(1000);
    return response;
}

// Getter function for devEUI
const String& getDevEUI() {
    return _devEUI; // Return a constant reference to devEUI
}
