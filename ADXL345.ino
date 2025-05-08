#include <SparkFun_ADXL345.h>         // SparkFun ADXL345 Library

#define HARD_BREAK 30
#define SOFT_BREAK 10
#define STILL_RANGE 2

#define X_OFFSET 2
#define Y_OFFSET 1
#define Z_OFFSET 31

int lastMessage = 0;
 
ADXL345 adxl = ADXL345();             // USE FOR I2C COMMUNICATION
 
void IRAM_ATTR pin1Interrupt();


void setup(){
 
  Serial.begin(9600);                 // Start the serial terminal

  adxl.powerOn();                     // Power on the ADXL345
  adxl.setRangeSetting(16);           // Give the range settings
 
  adxl.setSpiBit(0);                  
 
  adxl.setActivityXYZ(1, 0, 0);       // Set to activate movement detection in the axes "adxl.setActivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
  adxl.setActivityThreshold(75);      // 62.5mg per increment   // Set activity   // Inactivity thresholds (0-255)
 
  adxl.setInactivityXYZ(1, 0, 0);     // Set to detect inactivity in all the axes "adxl.setInactivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
  adxl.setInactivityThreshold(75);    // 62.5mg per increment   // Set inactivity // Inactivity thresholds (0-255)
  adxl.setTimeInactivity(10);         // How many seconds of no activity is inactive?
 
  adxl.setTapDetectionOnXYZ(0, 0, 1); // Detect taps in the directions turned ON "adxl.setTapDetectionOnX(X, Y, Z);" (1 == ON, 0 == OFF)
 
  // Set values for what is considered a TAP and what is a DOUBLE TAP (0-255)
  adxl.setTapThreshold(50);           // 62.5 mg per increment
  adxl.setTapDuration(15);            // 625 Î¼s per increment
  adxl.setDoubleTapLatency(80);       // 1.25 ms per increment
  adxl.setDoubleTapWindow(200);       // 1.25 ms per increment
 
  // Set values for what is considered FREE FALL (0-255)
  adxl.setFreeFallThreshold(7);       //62.5mg per increment
  adxl.setFreeFallDuration(30);       //5ms per increment
 
  // Setting all interupts to take place on INT1 pin except inactivity on INT2
  adxl.setImportantInterruptMapping(1, 1, 1, 1, 2);     // Sets "adxl.setEveryInterruptMapping(single tap, double tap, free fall, activity, inactivity);"

   // Turn on Interrupts for each mode (1 == ON, 0 == OFF)
  adxl.InactivityINT(1);
  adxl.ActivityINT(1);
  adxl.FreeFallINT(1);
  adxl.doubleTapINT(1);
  adxl.singleTapINT(1);

  pinMode(GPIO_NUM_15, INPUT);

  attachInterrupt(digitalPinToInterrupt(GPIO_NUM_15), pin1Interrupt, RISING);
}
 
void ADXL_ISR();

int checkBrakeType(int x, int y, int z)
{
  int returnValue;

  x = abs(x)-X_OFFSET;
  y = abs(y)-Y_OFFSET;
  z = abs(z)-Z_OFFSET;

  if(x >= SOFT_BREAK && x < HARD_BREAK && lastMessage != 1)
  {
    Serial.print("Soft Break! Value: ");
    Serial.println(x);
    lastMessage = 1;
    returnValue = 1;
  }
  else if (x > SOFT_BREAK && x > HARD_BREAK && lastMessage != 2) 
  {
    Serial.print("Hard Break! Value: ");
    Serial.println(x);
    lastMessage = 2;
    returnValue = 2;
  } else if((x >= 0 || x <= STILL_RANGE) && lastMessage != 3)
  {
    Serial.println("Standing still!");
    lastMessage = 3;
    returnValue = 0;
  }
  else
  {
    returnValue = -1;
  }

  return returnValue;
}


void loop(){
 
  // Accelerometer Readings
  int x,y,z;
  int rtrnVal;
  adxl.readAccel(&x, &y, &z);         // Read the accelerometer values and store them in variables declared above x,y,z
  rtrnVal = checkBrakeType(x, y, z);
  delay(100);
  

  ADXL_ISR(); //Check for interrupts on the ADXL345
}
 
/********************* ISR *********************/
void ADXL_ISR() {
 
  // getInterruptSource clears all triggered actions after returning value
  // Do not call again until you need to recheck for triggered actions
  byte interrupts = adxl.getInterruptSource();
 
  // Free Fall Detection
  if(adxl.triggered(interrupts, ADXL345_FREE_FALL)){
    Serial.println("*** FREE FALL ***");
    //add code here to do when free fall is sensed
  }
 
  // Inactivity
  if(adxl.triggered(interrupts, ADXL345_INACTIVITY)){
    Serial.println("*** INACTIVITY ***");
     //add code here to do when inactivity is sensed
  }
 
  // Activity
  if(adxl.triggered(interrupts, ADXL345_ACTIVITY)){
    Serial.println("*** ACTIVITY ***");
     //add code here to do when activity is sensed
  }
 
  // Double Tap Detection
  if(adxl.triggered(interrupts, ADXL345_DOUBLE_TAP)){
    Serial.println("*** DOUBLE TAP ***");
     //add code here to do when a 2X tap is sensed
  }
 
  // Tap Detection
  if(adxl.triggered(interrupts, ADXL345_SINGLE_TAP)){
    Serial.println("*** TAP ***");
     //add code here to do when a tap is sensed
  }
}

void IRAM_ATTR pin1Interrupt() {
  //Serial.println("ISR on pin 1!");
}
