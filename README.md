ntpstick
========

NTPstick is a GPS-synced NTP server designed for low cost and ease
of deployment in places outside traditional datacentres.

The "stick" concept is that the board is expected to be placed into
a PVC tube at least 25mm I.D. with only an Ethernet cable reaching
the outside world carrying both Ethernet and poor-mans POE.

Structure
---------

This repo is divided into sections for different parts of the project:

 - hardware/ .. Contains the Kicad sources, schematics in PDF, and generated
   gerber files.
 - firmware/ .. Completed builds of the firmware, able to be flashed to 
   boards over TFTP
 - src/ .. Source code for the firmware.

Specifications
--------------

 - Integrated GPS chip antenna
 - uBlox NEO-6M GPS module (includes internal TCXO)a
 - Atmel ATXMEGA128A4U MCU
 - 27MHz VCXO system clock
 - Maxim 12-bit DAC for VCXO control
 - WizNet W5500 Ethernet/IP chip
 - 7-37V DC input on spare Ethernet pairs
 - Board is 140mm long, 25mm wide
 
License
-------

 - Please check each folder for the specific license applied, as they
   vary between sections of the project.

