ntpstick
========

NTPstick is a GPS-synced NTP server designed for low cost and ease
of deployment in places outside traditional datacentres.

The "stick" concept is that the board is expected to be placed into
a PVC tube at least 30mm I.D. with only an Ethernet cable reaching
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

 - u.fl GPS antenna connection
 - uBlox MAX-M8Q GPS module (includes internal TCXO)
 - Atmel ATXMEGA128A4U MCU
 - 27MHz VCXO system clock
 - 12-bit DAC for VCXO control
 - WizNet W5500 Ethernet/IP chip
 - DC input on spare Ethernet pairs
 - DC-DC first stage, linear second stage PSU
 - Board is 113mm long, 30mm wide
 
License
-------

 - Please check each folder for the specific license applied, as they
   vary between sections of the project.

