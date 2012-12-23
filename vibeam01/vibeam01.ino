/*
 * Arduino sketch for the original ViBeam remote, as described here:
 * http://scanlime.org/2012/11/hacking-my-vagina/
 *
 * Micah Elizabeth Scott <beth@scanlime.org>
 * http://creativecommons.org/licenses/by-sa/2.0/
 */

#include <SPI.h>
#include <LeloRemote.h>
#include <SoftwareSerial.h>
#include "RunningMedian.h"

/****************************************************************************
 ****************************************************************************/


/*
 * Simple driver for a SparkFun COM-097565 serial LED module.
 */

class LEDModule
{
  SoftwareSerial serial;

public:
  LEDModule(int dummyPin, int txPin) :
    serial(dummyPin, txPin) {}

  int power;
  char mode;
  bool echoIndicator;

  void update()
  {
    /*
     * We use the right three digits for a power level, in decimal.
     * The left digit is a mode indicator, currently unused.
     * The first period is used to indicate the presence of a good Sonar echo. 
     */

    serial.write(0x77);    // Decimal point command    
    serial.write(echoIndicator);
  
    serial.write(mode);
    serial.write(power < 100  ? 'x' : (power / 100) % 10);
    serial.write(power < 10   ? 'x' : (power / 10) % 10);
    serial.write(power % 10);
  }

  void setup()
  {
    serial.begin(9600);

    power = 0;
    mode = 'x';
    echoIndicator = false;

    serial.write(0x76);    // Soft reset, home cursor
    update();
  }    
};


/*
 * Ping sonar sensor, with a median filter for rejecting noise and outliers.
 */

class PingModule
{
  int pin;

  RunningMedian <unsigned long, 16> filter;
  bool tapState, prevTapState;

public:
  static const int LIMIT = 5000;
  static const int TAP = 200;
  static const int NEAR = 500;
  static const int FAR = 3000;

  PingModule(int pin) : pin(pin) {}

  bool measure(unsigned long &result)
  {
    // Take one measurement, and add it to the buffer.
    // Based on http://arduino.cc/en/Tutorial/Ping

    pinMode(pin, OUTPUT);       // Switch signalpin to output
    digitalWrite(pin, LOW);     // Send low pulse
    delayMicroseconds(2);       // Wait for 2 microseconds
    digitalWrite(pin, HIGH);    // Send high pulse
    delayMicroseconds(5);       // Wait for 5 microseconds
    digitalWrite(pin, LOW);     // Holdoff
    pinMode(pin, INPUT);        // Switch signalpin to input
    digitalWrite(pin, HIGH);    // Turn on pullup resistor

    // Listen for an echo, with timeout
    unsigned long echo;
    echo = pulseIn(pin, HIGH, LIMIT);
    if (!echo)
      echo = LIMIT;

    // Update the filter
    filter.add(echo);
    bool okay = filter.getMedian(result) == filter.OK && result < LIMIT;

    // Detect 'tap' gesture, with hysteresis.
    prevTapState = tapState;
    tapState = result < (tapState ? NEAR : TAP);

    return okay;
  }

  bool tapped()
  {
    // Was a tap detected on the last measure()?
    return tapState && !prevTapState;
  }
};


/****************************************************************************
 ****************************************************************************/

LeloRemote remote;
LEDModule led(2, 3);
PingModule ping(4);

void setup()
{
  Serial.begin(9600);
  led.setup();
  SPI.begin();
  remote.reset();
}

void loop()
{
  // At every loop iteration, take a sonar measurement and update the filter
  unsigned long range;
  led.echoIndicator = ping.measure(range);

  // Change modes when the sensor is "tapped" by bringing the hand very close momentarily.
  if (ping.tapped()) {
    led.mode = (led.mode == 'x') ? 'L' : 'x';
  }

  // We only want to send radio packets at about 80 Hz, so rate-limit the rest of our calculations.
  static unsigned long deadline;
  unsigned long time = millis();
  if (time <= deadline)
    return;
  deadline = time + 1000 / 80;

  // In (L)ock mode, the power level only changes when a good sonar signal exists.
  if (led.mode != 'L' || led.echoIndicator) {
    // Calculate new power level
    led.power = constrain(map(range, ping.FAR, ping.NEAR, 0, remote.MAX_POWER), 0, remote.MAX_POWER);
  }

  // Send radio packet first, so it gets the lowest latency
  remote.txMotorPower(led.power);
  
  // Update lower-priority outputs
  led.update();
}

