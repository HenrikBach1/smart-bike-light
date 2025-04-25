#include <Arduino.h>
#include "LedBrightnessAdjust.h"

/* This module's compilation is controlled by the ENABLE_LED_BRIGHTNESS flag in Globals.h
 * When ENABLE_LED_BRIGHTNESS is set to 0, this implementation code is excluded from compilation
 * When ENABLE_LED_BRIGHTNESS is set to 1, this implementation code is included in compilation
 * Empty function stubs are provided in LedBrightnessAdjust.h when this module is disabled
 */

#if ENABLE_LED_BRIGHTNESS

const int pwmFreq = 5000; // 
const int pwmResolution = 8; // 8-bit → 0–255

// Global string to store input for brightness adjustment
String input = "";

void adjustBrightness() {
  input.toUpperCase(); //Make case insensitive

  int abIndex = input.indexOf("AB"); //Find position in string where AB occurs, return -1 if not found
  if (abIndex != -1 && abIndex + 2 < input.length()) { //Check if abIndex is 0 or above (position in string) and check if its longer than the two characters AB
    String numberPart = input.substring(abIndex + 2); //Make a string of the numbers behind abIndex position
    int value = numberPart.toInt(); //Turn the numbers in the string into integers

    if (value >= 0 && value <= 100) {
      int pwmValue = map(value, 0, 100, 0, 255);
      ledcWrite(ledPin, 255 - pwmValue);
      Serial.print("Brightness set to: ");
      Serial.print(value);
      Serial.println("%");
    } else {
      Serial.println("Number after B must be 0 to 100.");
    }
  } else {
    Serial.println("Input must include 'AB' followed by a number.");
  }
}

#endif // ENABLE_LED_BRIGHTNESS
