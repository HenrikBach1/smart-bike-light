//file=Network.cpp

#include "LedControl.h"
#include "Network.h" // Include Network.h (which includes Pins.h)
#include <Arduino.h> // Include Arduino library for Base64 decoding
#include <base64.h> // Include Base64 library for decoding

HardwareSerial mySerial(UART); // Initialize UART1
rn2xx3 myLora(mySerial);       // Initialize LoRa instance

//Global wide variables for :
String message = "";       // Received message

//Module wide variables for :
namespace {
    String devEUI = "";        // DevEUI/HWEUI
}

void initialize_LoRaWAN() {
    //HB: initialize_globals();

    mySerial.setRxBufferSize(1024);          // Set receive buffer size to 1024 bytes
    mySerial.begin(57600, SERIAL_8N1, RX, TX); // Initialize UART communication
    mySerial.setTimeout(1000);              // Set timeout to 1000ms
    initialize_module_rn2483_LoRa();
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
    devEUI = myLora.hweui();
    while (devEUI.length() != 16) {
        Serial.println("Communication with RN2xx3 unsuccessful.");
        delay(10000);
        devEUI = myLora.hweui();
    }

    Serial.println("DevEUI: " + devEUI);
    Serial.println("Firmware: " + myLora.sysver());

    //myLora.sendRawCommand("mac set deveui " + devEUI); // Set DevEUI explicitly
    //OTAA: initOTAA(String AppEUI, String AppKey);
    const char* appEUI = "0000000000000000";
    const char* appKey = "5EA67FF029810B31D0805D4749AA682E";
    bool join_result = myLora.initOTAA(appEUI, appKey);
    while (!join_result) {
        Serial.println("Join failed. Retrying...");
        delay(60000);
        join_result = myLora.init();
    }
    Serial.println("Successfully joined!");
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
    // Convert the module to a one-byte hex prefix
    char prefixedData[256]; // Adjust size as needed
    snprintf(prefixedData, sizeof(prefixedData), "%02X%s", (uint8_t)module, data);

    // Print the prefixed data for debugging
    Serial.print("ASCII message sent: ");
    Serial.println(prefixedData);

    // Send the prefixed data
    TX_RETURN_TYPE response = myLora.tx(prefixedData);
    message = myLora.getRx();

    //HB: // Print the message as hex values to the Serial Monitor
    // Serial.print("Received message (hex): ");
    // for (size_t i = 0; i < message.length(); i++) {
    //     Serial.print((uint8_t)message[i], HEX);
    //     Serial.print(" ");
    // }
    // Serial.println();

    // Decode the hexadecimal string to ASCII
    char asciiMessage[256]; // Adjust size as needed
    hexDecode(message, asciiMessage);
    message = String(asciiMessage); // Convert to String

    Serial.print("ASCII message received: ");
    Serial.println(message);

    delay(2000); //TODO: HB: 1% duty cycle implemented in this way, for now. Or does the WAN implementation this for us?
    return response;
}
