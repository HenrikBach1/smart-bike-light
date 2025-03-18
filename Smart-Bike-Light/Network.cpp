//file=Network.cpp

#include "LedControl.h"
#include "Network.h" // Include Network.h (which includes Pins.h)

HardwareSerial mySerial(UART); // Initialize UART1
rn2xx3 myLora(mySerial);       // Initialize LoRa instance

namespace {
    String devEUI = "";        // Global variable for DevEUI
}

void initialize_LoRaWAN() {
    //HB: initialize_globals();

    mySerial.setRxBufferSize(1024);          // Set receive buffer size to 1024 bytes
    mySerial.begin(57600, SERIAL_8N1, RX, TX); // Initialize UART communication
    mySerial.setTimeout(1000);              // Set timeout to 1000ms
    initialize_module_rn2483_LoRa();
    myLora.sendRawCommand("mac set conf 1"); // Enable confirmed uplink
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

    bool join_result = myLora.initOTAA("BE010000000007BA", "064D3897B8F4382A8C937BCA890435D3");
    while (!join_result) {
        Serial.println("Join failed. Retrying...");
        delay(60000);
        join_result = myLora.init();
    }
    Serial.println("Successfully joined!");
}

void send(Module module, Status status, const char* data) {
    led_on();
    TX_RETURN_TYPE response = myLora.tx(data);
    if (response == TX_SUCCESS) {
        Serial.println("Message sent!");
    } else if (response == TX_FAIL) {
        Serial.println("Failed to send.");
    } else {
        Serial.println("Unknown response.");
    }
    led_off();
    //delay(1000); //TODO: HB: 1% duty cycle implemented in this way, for now or does the WAN implementation this for us?
}
