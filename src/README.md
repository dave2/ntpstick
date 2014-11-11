NTPstick Firmware Source
========================

This is the source needed to build a firmware image for the NTPstick.

Source Structure
----------------

The source is divided into two major sections:
 
 - libkakapo/ from the Kakapo project, containing XMEGA MCU boilerplate
   drivers and general utilities
 - ntpstick/ which contains the NTPstick-specific implementation

Note that the license is different for each of these parts. libkakapo is
provided under LGPLv3, while NTPstick's main source is GPL v2.

The copy of libkakapo included here is known to work with the NTPstick
source. Other versions may not work.

libkakapo upstream can be obtained from here:
https://github.com/dave2/libkakapo

Build Requirements
------------------

You will need the following tools to complete the build:

 - gcc-avr, at least 4.8.1
 - avr-libc, at least 1.8.0
 - binutils-avr
 - make
 - ...

Note: this build process has only been tested on Ubuntu 14.04.

