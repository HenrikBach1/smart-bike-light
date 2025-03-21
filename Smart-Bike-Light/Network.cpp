//file=Network.cpp

#include "LedControl.h"
#include "Network.h" // Include Network.h (which includes Pins.h)

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
    const char* appEUI = "0000000000000000"; //HB:
    const char* appKey = "5EA67FF029810B31D0805D4749AA682E"; //HB:
    bool join_result = myLora.initOTAA(appEUI, appKey); //HB:
    while (!join_result) {
        Serial.println("Join failed. Retrying...");
        delay(60000);
        join_result = myLora.init();
    }
    Serial.println("Successfully joined!");
}

TX_RETURN_TYPE transeive(Module module, Status status, const char* data) {
    //myLora.sendRawCommand("mac set conf 1"); // Enable confirmed uplink
    TX_RETURN_TYPE response = myLora.tx(data);
    message = myLora.getRx();
    delay(2000); //TODO: HB: 1% duty cycle implemented in this way, for now. Or does the WAN implementation this for us?
    return response;
}
