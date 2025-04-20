//file=Network.cpp

#include "LedControl.h"
#include "Network.h" // Include Network.h (which includes Pins.h)
#include <Arduino.h> // Include Arduino library for Base64 decoding
#include <base64.h> // Include Base64 library for decoding

// Configuration constants
#define JOIN_MAX_RETRIES 5     // Maximum number of join attempts
#define JOIN_RETRIES_DELAY 60000 // Delay between retries in milliseconds (60 seconds)
#define APP_EUI "0000000000000000"  // Application EUI for TTN
#define APP_KEY "5EA67FF029810B31D0805D4749AA682E"  // Application Key for TTN

HardwareSerial mySerial(UART); // Initialize UART1
rn2xx3 myLora(mySerial);       // Initialize LoRa instance

//Global wide variables for :
String message = "";       // Received message

//Module wide variables for :
namespace {
    String _devEUI = "";         // DevEUI/HWEUI
    bool _is_joined_TTN = false; // Track TTN join status
}

void initialize_LoRaWAN() {
    //HB: initialize_globals();

    mySerial.setRxBufferSize(1024);          // Set receive buffer size to 1024 bytes
    mySerial.begin(57600, SERIAL_8N1, RX, TX); // Initialize UART communication
    mySerial.setTimeout(1000);              // Set timeout to 1000ms
    mySerial.println("Serial initialized.");

    initialize_module_rn2483_LoRa();
    mySerial.println("LoRa module initialized.");
}

bool join_TTN() {
    if (_is_joined_TTN) {
        return true;
    }

    // myLora.sendRawCommand("mac set deveui " + devEUI); // Set DevEUI explicitly
    // OTAA: initOTAA(String AppEUI, String AppKey);
    _is_joined_TTN = myLora.initOTAA(APP_EUI, APP_KEY);
    
    int retries = 0;
    
    while (!_is_joined_TTN && retries < JOIN_MAX_RETRIES) {
        Serial.println("Join failed. Retrying...");
        delay(JOIN_RETRIES_DELAY);  // Wait between retry attempts
        _is_joined_TTN = myLora.initOTAA(APP_EUI, APP_KEY);
        retries++;
    }
    
    if (_is_joined_TTN) {
        Serial.println("Successfully joined!");
    } else {
        Serial.println("Failed to join after maximum retry attempts.");
    }
    
    return _is_joined_TTN;
}

bool is_joined_TTN() {
    return _is_joined_TTN;
}

bool leave_TTN() {
    if (!_is_joined_TTN) {
        // Already disconnected, nothing to do
        Serial.println("Already disconnected from TTN");
        return true;
    }
    
    // Send the command to disconnect from the network
    String response = myLora.sendRawCommand("mac reset");
    
    if (response == "ok") {
        _is_joined_TTN = false;
        Serial.println("Successfully disconnected from TTN");
        return true;
    } else {
        Serial.println("Failed to disconnect from TTN. Response: " + response);
        return false;
    }
}

void initialize_module_rn2483_LoRa() {
    // Reset RN2xx3
    pinMode(RST, OUTPUT); // Use RST from Pins.h
    digitalWrite(RST, LOW);
    delay(100);
    digitalWrite(RST, HIGH);
    delay(100); // Wait for RN2xx3's startup message
    mySerial.flush();

    // Check communication with the radio
    myLora.sendRawCommand("mac reset");
    _devEUI = myLora.hweui();
    while (_devEUI.length() != 16) {
        Serial.println("Communication with RN2xx3 unsuccessful.");
        delay(10000);
        _devEUI = myLora.hweui();
    }

    Serial.println("DevEUI: " + _devEUI);
    Serial.println("Firmware: " + myLora.sysver());
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

TX_RETURN_TYPE transeive(Module module, Status status, const char* data) {
    // Convert both the module and status to a two-byte hex prefix
    char prefixedData[256]; // Adjust size as needed
    snprintf(prefixedData, sizeof(prefixedData), "%02X%02X%s", (uint8_t)module, (uint8_t)status, data);

    // Print the prefixed data for debugging
    Serial.print("DevEUI: ");
    Serial.println(_devEUI);
    Serial.print("Sending message: ");
    Serial.println(prefixedData);

    // Send the prefixed data
    TX_RETURN_TYPE response = myLora.tx(prefixedData);
    message = myLora.getRx();

    // Decode the hexadecimal string to ASCII
    char asciiMessage[256]; // Adjust size as needed
    hexDecode(message, asciiMessage);
    message = String(asciiMessage); // Convert to String

    Serial.print("ASCII message received: ");
    Serial.println(message);

    delay(2000); //TODO: HB: 1% duty cycle implemented in this way, for now. Or does the WAN implementation this for us?
    return response;
}
