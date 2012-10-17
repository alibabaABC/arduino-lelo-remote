arduino-lelo-remote
===================

Arduino library for remote-control Lelo vibrators

This is a work in progress!

The "notes" directory contains some SPI bus traces from the original remote.

You'll need a CC2500 radio module. There are many sources for these. Take care that the radio is a 3.3v part. If you're lazy like me, connect its power to the Arduino's 3.3v regulator, and use 1K series current limiting resistors on all outputs from the Arduino to the radio.

