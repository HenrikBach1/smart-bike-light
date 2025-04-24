const int ledPin = 5; //Choose ledPin, has to be PWM compatable. Pin 5 on the ESP32 is
const int pwmFreq = 5000; // 
const int pwmResolution = 8; // 8-bit → 0–255

void setup() {
  Serial.begin(115200);
  ledcAttach(ledPin, pwmFreq, pwmResolution); // Adjusted ESP32 package
  Serial.println("Enter brightness (0-100):"); 
}

void loop() {
  if (Serial.available() > 0) { //Check if serial is open
  
    adjustBrightness(); //Function that reads for 
  }
}
