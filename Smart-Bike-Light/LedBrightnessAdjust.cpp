


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

