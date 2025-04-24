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

void adjustBrightness() {
  int input = Serial.parseInt();
  if (input >= 0 && input <= 100) {
    int pwmValue = map(input, 0, 100, 0, 255);       // Convert to 0–255
    ledcWrite(ledPin, 255 - pwmValue);  // Invert for PNP
    Serial.print("Brightness set to: ");
    Serial.print(input);
    Serial.println("%");
  } else {
    Serial.println("Invalid input. Enter a number from 0 to 100.");
  }
    // Flush the rest of the line if any leftover junk
  while (Serial.available()) Serial.read();
}
