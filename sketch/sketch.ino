#include <SPI.h>
#include "LeloRemote.h"

LeloRemote remote;

void setup()
{
  Serial.begin(9600);
  SPI.begin();
  remote.reset();
}

void loop()
{
  // Read potentiometer input
  int analog = analogRead(0);

  /*
   * This defines the maximum and minimum analog values that
   * will correspond to the maximum and minimum vibration levels.
   * We have a small margin at either side so that it's possible
   * to reliably reach both extremes.
   */
  const int analogMin = 10;
  const int analogMax = 1023 - 10;

  // Scale from 0 to MAX_POWER
  int power = (constrain(analog, analogMin, analogMax) - analogMin)
    * long(remote.MAX_POWER) / long(analogMax - analogMin);
  
  remote.txMotorPower(power);
  delay(10);
}
