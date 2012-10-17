/*
 * Arduino library for emulating the Lelo vibrator remote.
 * Requires a CC2500 radio attached via SPI.
 *
 * Micah Elizabeth Scott <beth@scanlime.org>
 */

#ifndef LeloRemote_h
#define LeloRemote_h

#include "Arduino.h"
#include <avr/pgmspace.h>

class LeloRemote
{
public:
    struct Packet {
        byte unk0;
        byte unk1;
        byte unk2;
        byte motor[2];
        byte unk5;
        byte unk6;
        byte unk7;
        byte unk8;
    };

    LeloRemote(int chipSelectPin = 10);
    void reset();
    void txPacket(const Packet &p);
    void txMotorPower(byte power);

    static const byte MAX_POWER = 0x80;

private:
    int csn;

    void transferDelay();
    void spiTable(prog_uchar *table);
    byte regRead(byte reg);
};

#endif
