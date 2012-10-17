#include <SPI.h>
#include "LeloRemote.h"

LeloRemote remote;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  remote.reset();
}

void loop() {
  for (int i = 0; i < 5; i++) {
    remote.txMotorPower(0x80);
    delay(100);
  }
  for (int i = 0; i < 15; i++) {
    remote.txMotorPower(0x00);
    delay(100);
  }
}
