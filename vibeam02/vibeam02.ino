/*
 * Arduino sketch for version of the ViBeam remote.
 *
 * This uses the SparkFun serial 7-segment board (COM-11442), and it runs
 * entirely on the LED module's built-in ATmega328.
 *
 * Dependencies:
 *   - S7S Arduino add-on: https://github.com/sparkfun/Serial7SegmentDisplay
 *   - SevSeg library: https://github.com/sparkfun/SevSeg
 *
 * Micah Elizabeth Scott <beth@scanlime.org>
 * http://creativecommons.org/licenses/by-sa/2.0/
 */

#include <SevSeg.h>
#include <SPI.h>
#include <LeloRemote.h>
#include "RunningMedian.h"


/****************************************************************************
 ****************************************************************************/

/*
 * LED module abstraction, compatible with the SparkFun COM-11442.
 */

class LEDModule
{
  SevSeg display;
  char string[5];

public:
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
    
    string[0] = mode;
    string[1] = power < 100  ? 'x' : (power / 100) % 10;
    string[2] = power < 10   ? 'x' : (power / 10) % 10;
    string[3] = power % 10;
    string[4] = 0;

    display.DisplayString(string, echoIndicator);
  }

  void setup()
  {
    display.Begin(COMMON_ANODE, 4, 16, 17, 3, 4, 2, 9, 8, 14, 6, A1, 23, 7, 5, 22, 6, 7);
    display.SetBrightness(100);

    power = 0;
    mode = 'x';
    echoIndicator = false;

    update();
  }    
};


/*
 * HC-SR04 sonar sensor, with a median filter for rejecting noise and outliers.
 */

class SonarModule
{
  static const int txPin = A4;
  static const int rxPin = A5;

  RunningMedian <unsigned long, 4> filter;
  bool tapState, prevTapState;

public:
  static const int LIMIT = 5000;
  static const int TAP = 200;
  static const int NEAR = 500;
  static const int FAR = 3000;

  bool measure(unsigned long &result)
  {
    // Take one measurement, and add it to the buffer.

    pinMode(txPin, OUTPUT);
    pinMode(rxPin, INPUT);

    digitalWrite(txPin, LOW);
    delayMicroseconds(2);
    digitalWrite(txPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(txPin, LOW);

    // Listen for an echo, with timeout
    unsigned long echo = pulseIn(rxPin, HIGH, LIMIT);
Serial.println(echo);

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
LEDModule led;
SonarModule sonar;

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
  led.echoIndicator = sonar.measure(range);

  // Change modes when the sensor is "tapped" by bringing the hand very close momentarily.
  if (sonar.tapped()) {
    led.mode = (led.mode == 'x') ? 'L' : 'x';
  }

  // In (L)ock mode, the power level only changes when a good sonar signal exists.
  if (led.mode != 'L' || led.echoIndicator) {
    // Calculate new power level
    led.power = constrain(map(range, sonar.FAR, sonar.NEAR, 0, remote.MAX_POWER), 0, remote.MAX_POWER);
  }

  remote.txMotorPower(led.power);

  /* We need to rate limit our radio transmissions and our sonar pings,
   * since both the vibrator and sonar module will behave badly if we
   * spam them too quickly. Update LED while we wait.
   */
  static unsigned long deadline;
  for (;;) {
    unsigned long time = millis();
    if (time > deadline) {
      deadline += 30;
      break;
    }

    led.update();
  }
}

