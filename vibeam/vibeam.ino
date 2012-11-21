/*
 * Arduino sketch for the ViBeam remote.
 * 
 * Micah Elizabeth Scott <beth@scanlime.org>
 * http://creativecommons.org/licenses/by-sa/2.0/
 */

#include <SPI.h>
#include <LeloRemote.h>
#include <SoftwareSerial.h>


/**********************************************************************
 * Hardware Declarations
 */

LeloRemote remote;
SoftwareSerial led(2, 3);
const int pingSignalPin = 4;


/**********************************************************************
 * LED Display
 */

void ledSetup()
{
   led.begin(9600);
   
   // Soft reset, home cursor
   led.write(0x76);
}

void ledDisplay(int value)
{
  /*
   * Display a number right-justified on a SparkFun serial LED
   * attached to our hardware serial port.
   */
  led.write(value < 1000 ? 'x' : (value / 1000) % 10);
  led.write(value < 100  ? 'x' : (value / 100) % 10);
  led.write(value < 10   ? 'x' : (value / 10) % 10);
  led.write(value % 10);
}


/**********************************************************************
 * Ping Sonar Input
 */

unsigned long ping()
{
  // From http://arduino.cc/en/Tutorial/Ping

  pinMode(pingSignalPin, OUTPUT);             // Switch signalpin to output
  digitalWrite(pingSignalPin, LOW);           // Send low pulse
  delayMicroseconds(2);                       // Wait for 2 microseconds
  digitalWrite(pingSignalPin, HIGH);          // Send high pulse
  delayMicroseconds(5);                       // Wait for 5 microseconds
  digitalWrite(pingSignalPin, LOW);           // Holdoff
  pinMode(pingSignalPin, INPUT);              // Switch signalpin to input
  digitalWrite(pingSignalPin, HIGH);          // Turn on pullup resistor
  return pulseIn(pingSignalPin, HIGH, 5000);  // Listen for echo
}
 

/**********************************************************************
 * Setup
 */

void setup()
{
  ledSetup();
  SPI.begin();
  remote.reset();
}

void loop()
{
  int range = ping();
  if (!range) range = 10000;
  int power = map(range, 3000, 250, 0, remote.MAX_POWER);
  power = constrain(power, 0, remote.MAX_POWER);

  // Show the current power level on our LED display
  ledDisplay(power);

  remote.txMotorPower(power);

  // Send radio packets at no more than 100 Hz
  delay(10);
}

