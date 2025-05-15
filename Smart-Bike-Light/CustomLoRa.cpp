//file=CustomLoRa.cpp

#include "LedControl.h"
#include "CustomLoRa.h" // Include CustomLoRa.h (which includes Globals.h)
#include "WiFiScanner.h" // Include WiFiScanner.h for getTop3Networks function
#include <Arduino.h>    // Include Arduino library for Base64 decoding
#include <base64.h>     // Include Base64 library for decoding

/* This module's compilation is controlled by the ENABLE_LORA_MODULE flag in Globals.h
 * When ENABLE_LORA_MODULE is set to 0, this implementation code is excluded from compilation
 * When ENABLE_LORA_MODULE is set to 1, this implementation code is included in compilation
 * Empty function stubs are provided in CustomLoRa.h when this module is disabled
 */

#if ENABLE_LORA_MODULE

// Configuration constants
#define JOIN_MAX_RETRIES 2     // Maximum number of join attempts
#define JOIN_RETRIES_DELAY 1000 // Delay between retries in milliseconds (60 seconds)
#define APP_EUI "0000000000000000"  // Application EUI for TTN
#define APP_KEY "5EA67FF029810B31D0805D4749AA682E"  // Application Key for TTN

RTC_DATA_ATTR HardwareSerial myLoRaSerial(UART); // Initialize UART1
RTC_DATA_ATTR rn2xx3 myLora(myLoRaSerial);       // Initialize LoRa instance

// Define global application variables

// Define the global decomposed message variable
DecomposedMessage g_decomposedMessage = {MODULE_NONE, ""};

//Module wide variables for :
namespace {
    String _devEUI = "";         // DevEUI/HWEUI
    bool _is_joined_TTN = false; // Track TTN join status
    String _message = "";        // Module-internal message variable
    
    // Internal function prototypes and implementations
    DecomposedMessage decompose_message(const String& formattedMessage);
}

void initialize_LoRaWAN() {
    //HB: initialize_globals();

    myLoRaSerial.setRxBufferSize(1024);          // Set receive buffer size to 1024 bytes
    myLoRaSerial.begin(57600, SERIAL_8N1, RX, TX); // Initialize UART communication
    myLoRaSerial.setTimeout(1000);              // Set timeout to 1000ms
    
    Serial.println("Serial initialized.");

    if (initialize_module_rn2483_LoRa()) {
      Serial.println("LoRa module initialized.");
    } else {
      Serial.println("LoRa module NOT initialized.");
    }

    Serial.println("LoRaWAN initialized");  
    lora_ping();
}

bool join_TTN() {
    lora_ping();
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

void lora_ping() {
  if (!myLora.hweui().isEmpty()) {
    Serial.println("Ping to RN2384 successfull");
  } else {
    Serial.println("Ping to RN2384 failed");
  }
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

bool initialize_module_rn2483_LoRa() {
    // Reset RN2xx3
    pinMode(RST, OUTPUT); // Use RST from Pins.h
    digitalWrite(RST, LOW);
    delay(100);
    digitalWrite(RST, HIGH);
    delay(100); // Wait for RN2xx3's startup message
    myLoRaSerial.flush();

    // Check communication with the radio
    myLora.sendRawCommand("mac reset");
    int lora_connect_tries = 1;
    _devEUI = myLora.hweui();
    while ((_devEUI.length() != 16) && (lora_connect_tries <= 2)) {
        Serial.println("Communication with RN2xx3 unsuccessful.");
        delay(1000);
        _devEUI = myLora.hweui();
        ++lora_connect_tries;
    }

    Serial.println("DevEUI: " + _devEUI);
    Serial.println("Firmware: " + myLora.sysver());

    return (!_devEUI.isEmpty());
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

namespace {
    // Function to decompose a formatted message into components
    DecomposedMessage decompose_message(const String& formattedMessage) {
        DecomposedMessage result;
        
        // Initialize default values
        result.toModule = MODULE_NONE;
        result.data = "";
        
        // Check if message is valid
        if (formattedMessage.length() < 3) { // At minimum need XX-
            Serial.println("Invalid message format: too short");
            // Even for invalid messages, we still return the original message as data
            result.data = formattedMessage;
            return result;
        }
        
        // Parse module value (first two hex characters)
        String moduleStr = formattedMessage.substring(0, 2);
        uint8_t moduleVal = strtol(moduleStr.c_str(), nullptr, 16);
        
        // Get the data portion (everything after the dash)
        if (formattedMessage.length() > 3) {
            result.data = formattedMessage.substring(3);
        } else {
            // If there's no data portion, set data to empty string but still return
            result.data = "";
        }
        
        result.toModule = static_cast<Module>(moduleVal); // Use static_cast to convert to Module enum
        return result;
    }
}

TX_RETURN_TYPE tranceive(Module module, Status status, const char* data) {
    
    lora_ping();
    // Check if we are connected to TTN, if not try to join
    if (!is_joined_TTN()) {
        Serial.println("Not connected to TTN, attempting to join...");
        led_on();
        if (!join_TTN()) {
            Serial.println("Failed to join TTN, cannot send data");
            led_off();
            return TX_FAIL;  // Return failure if we can't join TTN
        }
        Serial.println("Successfully joined TTN");
        led_off();
    }

    // Convert both the module and status to a two-byte hex prefix
    char prefixedData[256]; // Adjust size as needed
    snprintf(prefixedData, sizeof(prefixedData), "%02X-%02X-%s", (uint8_t)module, (uint8_t)status, data);

    // TXing section
    Serial.println("TXing");
    Serial.print("DevEUI: ");
    Serial.println(_devEUI);
    Serial.print("Sending message: ");
    Serial.println(prefixedData);
    
    // Send the message
    led_on();
    TX_RETURN_TYPE response = myLora.tx(prefixedData);
    led_off();

    // Show send status after receiving message
    if (response == TX_SUCCESS) {
        Serial.println("Message sent!");
    } else if (response == TX_FAIL) {
        Serial.println("Failed to send.");
    } else if (response == 2) {
        Serial.println("Message sent and downlink data received!");
        response = TX_SUCCESS; // Treat this as a success for our purposes
    } else {
        Serial.println("Unknown response: \"" + String(response) + "\"");
        response = TX_FAIL; // Treat this as a failure
    }

    _message = myLora.getRx();
    
    if (_message != "") {
        Serial.println("RXing: " + _message);
        
        // Decode the hexadecimal string to ASCII
        char asciiMessage[256]; // Adjust size as needed
        hexDecode(_message, asciiMessage);
        _message = String(asciiMessage); // Convert to String
        
        Serial.print("ASCII message received: ");
        Serial.println(_message);
        
            // Use decompose_message to extract module and data from received message and store in global variable
        g_decomposedMessage = decompose_message(_message);
        
        // Print the decomposed message components using cast instead of switch
        Serial.print("To Module: ");
        Serial.println((int)g_decomposedMessage.toModule);
        
        Serial.print("Data: ");
        Serial.println(g_decomposedMessage.data);
        
        _message = "";
        Serial.println("Reset message: " + _message);
    }

    return response;
}

// Function to process LoRaWAN messages
void processLoRaWANMessage(const DecomposedMessage& message) {
  if (message.toModule == MODULE_LORAWAN) {  //01
    // Define variables outside of switch to avoid cross-initialization errors
    uint8_t macs[3][6];
    uint8_t rssis[3];
    int networksFound = 0;
    
    // Handle LoRaWAN specific commands
    switch (message.data[0]) {
      case MODULE_WIFI:
        networksFound = getTop3Networks(macs, rssis);
        Serial.println("WiFi Networks Scan Results:");
        if (networksFound == 0) {
          Serial.println("No networks found");
        } else {
          for (int i = 0; i < networksFound; i++) {
            Serial.print("Network ");
            Serial.print(i + 1);
            Serial.print(": MAC=");
            for (int j = 0; j < 6; j++) {
              if (macs[i][j] < 16) Serial.print("0"); // Add leading zero for proper MAC format
              Serial.print(macs[i][j], HEX);
              if (j < 5) Serial.print(":"); // Add colon between MAC bytes except after the last one
            }
            Serial.print(", RSSI=");
            Serial.println(rssis[i]);
          }

          // Create a data message in the format for above wifi networks: 
          // "<networksFound>-<MAC1>:<RSSI1>,<MAC2>:<RSSI2>,<MAC3>:<RSSI3>"
          // 3-28EE52A277AE:59,A0E4CBFEDAC6:72,C4E9844ED82F:73

          char wifiData[150]; // Buffer for wifi network data
          char macStr[18]; // Buffer for MAC address string
          int offset = 0;
          
          // Add the number of networks found
          offset += snprintf(wifiData + offset, sizeof(wifiData) - offset, "%d-", networksFound);
          
          // Add each network's MAC and RSSI
          for (int i = 0; i < networksFound; i++) {
            // Format MAC address
            sprintf(macStr, "%02X%02X%02X%02X%02X%02X", 
                    macs[i][0], macs[i][1], macs[i][2], 
                    macs[i][3], macs[i][4], macs[i][5]);
            
            // Add MAC:RSSI to the message
            offset += snprintf(wifiData + offset, sizeof(wifiData) - offset, 
                             "%s:%d", macStr, rssis[i]);
            
            // Add comma separator if not the last network
            if (i < networksFound - 1) {
              offset += snprintf(wifiData + offset, sizeof(wifiData) - offset, ",");
            }
          }
          
          // Send the WiFi data via LoRaWAN
          Serial.print("Sending WiFi data: ");
          Serial.println(wifiData);
          TX_RETURN_TYPE wifiResponse = tranceive(MODULE_WIFI, STATUS_OK, wifiData);
          
          if (wifiResponse == TX_SUCCESS) {
            Serial.println("WiFi data sent successfully");
          } else {
            Serial.println("Failed to send WiFi data");
          }
        }
        break;
      case '1':
        led_on();
        Serial.println("LED turned ON by LoRaWAN command");
        break;
      case '0':
        led_off();
        Serial.println("LED turned OFF by LoRaWAN command");
        break;
      case 'L':
        // Check if it's "LED_ON" or "LED_OFF"
        if (message.data.indexOf("LED_ON") == 0) {
          led_on();
          Serial.println("LED turned ON by LoRaWAN command");
        } else if (message.data.indexOf("LED_OFF") == 0) {
          led_off();
          Serial.println("LED turned OFF by LoRaWAN command");
        } else {
          Serial.println("Unknown LED command");
        }
        break;
      default:
        Serial.println("Unknown LED command");
        break;
    }
  }
}

// Function to lower the RX pin for at least 226 microseconds and send 0x55 character
void wakeUp() {
    Serial.println("Resetting RN2483 with RX pin and sending 0x55");
    
    // Configure RX pin as output temporarily
    pinMode(RX, OUTPUT);
    
    // Lower the RX pin (set to LOW)
    digitalWrite(RX, LOW);
    
    // Delay for at least 226 microseconds (using 230 to be safe)
    delayMicroseconds(230);
    
    // Release the RX pin (back to HIGH)
    digitalWrite(RX, HIGH);
    
    // Short delay to ensure the pin transition is complete
    delayMicroseconds(10);
    
    // Reconfigure RX pin back for serial communication
    pinMode(RX, INPUT);
    
    // Send the 0x55 character to the LoRa module
    myLoRaSerial.write(0x55);
    
    // Wait for the transmission to complete
    myLoRaSerial.flush();
    
    Serial.println("Reset and character transmission complete");
    lora_ping();
}


// Function to put RN2483 LoRa module into deep sleep mode
void deepSleep() {
    lora_ping();
    Serial.println("Putting RN2483 into deep sleep mode");
    
    // Send the sleep command with maximum sleep time (4294967296 ms ≈ 49.7 days)
    myLora.sleep(4294967296);  // non-blocking
    
    
    // Check response
    //if (response == "ok") {
    //   Serial.println("RN2483 entered deep sleep successfully");
    //} else {
    //    Serial.println("Failed to put RN2483 into deep sleep. Response: '" + response + "'");
    //}
}

#endif // ENABLE_LORA_MODULE
