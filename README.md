arduino-lelo-remote
===================

This project is an Arduino-based remote control for the radio-controlled Lelo vibrators. This speaks the RF protocol used by the Lyla vibrator, which may also be used by other products from Lelo. 

Library
-------

The "LeloRemote" directory is an Arduino library. It comes with a couple very simple examples. To use this in your own projects, just copy the whole thing into your Arduino "libraries" folder.

The library requires a CC2500 radio chip. Breakout boards for this chip are plentiful on eBay from a variety of manufacturers.

Take care that this is a 3.3v chip! If possible, use an Arduino variant with a 3.3v supply. If you must use a 5v Arduino, you _must_ power the CC2500 off a 3.3v regulator. If you're using an Arduino Uno, for example, there's already a 3.3v output pin that will work fine. To protect the 3.3v inputs on the CC2500, use 1K resistors in series with the MOSI, SCK, and SS pins.

"Vibeam" Sonar Remote
---------------------

The Vibeam project (Vibrator + Beam) is a wireless hands-free vibrator controller based on the Parallax Ping sonar sensor. Source code and design files are in the "vibeam" folder.

This project uses an Arduino Pro Mini and the LeloRemote Arduino library. Additionally, it needs:

 - A portable source of regulated 5V power. I used a cheap USB phone charger battery
 - The Parallax Ping sonar sensor
 - A 4-digit serial LED module from SparkFun
 - The CC2500 radio

I designed a 3D printable housing, designed to be printable on a Makerbot or similar printer. The original design source files for Blender are here, as are ready-to-print STL meshes.

About
-----

Written by Micah Elizabeth Scott. Everything here is released under the CC-BY-SA license.

http://creativecommons.org/licenses/by-sa/2.0/
