#include <SPI.h>
#include "LeloRemote.h"

LeloRemote remote;

void ledReset()
{
   // Reset and home cursor
   Serial.write(0x76);

   // High brightness
   Serial.write(0x7A);
   Serial.write(byte(0x00));
}

void ledDisplay(int value)
{
  /*
   * Display a number right-justified on a SparkFun serial LED
   * attached to our hardware serial port.
   */
  Serial.write(value < 1000 ? 'x' : (value / 1000) % 10);
  Serial.write(value < 100  ? 'x' : (value / 100) % 10);
  Serial.write(value < 10   ? 'x' : (value / 10) % 10);
  Serial.write(value % 10);
}

void setup()
{
  Serial.begin(9600);
  SPI.begin();
  remote.reset();
  ledReset();
}

void loop()
{
  const int hysteresis = 4;
  const int deadzone = 5;
  
  // Read potentiometer input, with a little hysteresis
  static int analogValue;
  int sample = analogRead(0);
  if (abs(sample - analogValue) > hysteresis)
    analogValue = sample;

  // Scale analog input from 0 to MAX_POWER
  int power = map(analogValue, 0 + deadzone, 1023 - deadzone, 0, remote.MAX_POWER);
  power = constrain(power, 0, remote.MAX_POWER);

  // Show the current power level on our LED display
  ledDisplay(power);

  // Send radio packets at about 100 Hz
  remote.txMotorPower(power);
  delay(10);
}

