EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:special
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:w5500
LIBS:microchip-mac
LIBS:w_analog
LIBS:w_connectors
LIBS:w_logic
LIBS:w_memory
LIBS:w_opto
LIBS:w_relay
LIBS:w_rtx
LIBS:w_transistor
LIBS:hirose-fx2-100-2
LIBS:hirose
LIBS:halo-electronics
LIBS:tl2575
LIBS:lm2672
LIBS:ds3234
LIBS:ublox-gps
LIBS:sma_edge
LIBS:pcf2129at
LIBS:recom-power
LIBS:vcxo
LIBS:max5805baub+
LIBS:vcxo-7x5
LIBS:lmr16006
LIBS:tlv70xx
LIBS:ublox-max
LIBS:mcp4725
LIBS:pdi-ser
LIBS:ntpstick-1-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 3
Title "NTP Stick 1.2"
Date "Sun 29 Mar 2015"
Rev "1"
Comp "hairy.geek.nz"
Comment1 "CC-NZ-3.0 BY-SA"
Comment2 "Not Production Proven"
Comment3 ""
Comment4 ""
$EndDescr
$Sheet
S 1250 3750 1150 900 
U 525A0905
F0 "NTP Stick Ethernet" 50
F1 "ntpstick-1-ethernet.sch" 50
F2 "NICINTn" O R 2400 3900 60 
F3 "NICMOSI" I R 2400 4000 60 
F4 "NICMISO" O R 2400 4100 60 
F5 "NICSCK" I R 2400 4200 60 
F6 "NICSCSn" I R 2400 4300 60 
F7 "NICRSTn" I R 2400 4400 60 
F8 "CON_PWR" O R 2400 4500 60 
$EndSheet
$Sheet
S 3150 3700 1400 1050
U 525A3117
F0 "NTP Stick System" 50
F1 "ntpstick-1-system.sch" 50
F2 "NICINTn" I L 3150 3900 60 
F3 "NICSCSn" O L 3150 4300 60 
F4 "NICMOSI" O L 3150 4000 60 
F5 "NICMISO" I L 3150 4100 60 
F6 "NICSCK" O L 3150 4200 60 
F7 "DC_IN" I L 3150 4500 60 
F8 "NICRSTn" O L 3150 4400 60 
$EndSheet
Wire Wire Line
	2400 3900 3150 3900
Wire Wire Line
	2400 4000 3150 4000
Wire Wire Line
	2400 4100 3150 4100
Wire Wire Line
	2400 4200 3150 4200
Wire Wire Line
	2400 4300 3150 4300
Wire Wire Line
	2400 4500 3150 4500
$Comp
L HEADER_1 J1
U 1 1 525E3109
P 2600 2650
F 0 "J1" H 2600 2800 60  0000 C CNN
F 1 "MOUNT0" H 2600 2500 60  0000 C CNN
F 2 "Screws:4-40-HOLE" H 2600 2650 60  0001 C CNN
F 3 "" H 2600 2650 60  0000 C CNN
	1    2600 2650
	1    0    0    -1  
$EndComp
$Comp
L HEADER_1 J2
U 1 1 525E3116
P 3100 2650
F 0 "J2" H 3100 2800 60  0000 C CNN
F 1 "MOUNT1" H 3100 2500 60  0000 C CNN
F 2 "Screws:4-40-HOLE" H 3100 2650 60  0001 C CNN
F 3 "" H 3100 2650 60  0000 C CNN
	1    3100 2650
	-1   0    0    1   
$EndComp
Text Notes 3700 2950 2    60   ~ 0
Used to cable tie down ethernet cable
Wire Notes Line
	1800 1500 1800 3050
Wire Notes Line
	1800 3050 3900 3050
Wire Notes Line
	3900 3050 3900 1500
Wire Notes Line
	3900 2300 1800 2300
Wire Wire Line
	2400 4400 3150 4400
$Comp
L HEADER_1 J3
U 1 1 551778B9
P 2600 1900
F 0 "J3" H 2600 2050 60  0000 C CNN
F 1 "MOUNT2" H 2600 1750 60  0000 C CNN
F 2 "Screws:4-40-HOLE" H 2600 1900 60  0001 C CNN
F 3 "" H 2600 1900 60  0000 C CNN
	1    2600 1900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR01
U 1 1 55177DDF
P 2300 2050
F 0 "#PWR01" H 2300 1800 50  0001 C CNN
F 1 "GND" H 2300 1900 50  0000 C CNN
F 2 "" H 2300 2050 60  0000 C CNN
F 3 "" H 2300 2050 60  0000 C CNN
	1    2300 2050
	1    0    0    -1  
$EndComp
Wire Wire Line
	2500 2650 2250 2650
$Comp
L GND #PWR02
U 1 1 55282FC9
P 2250 2700
F 0 "#PWR02" H 2250 2450 50  0001 C CNN
F 1 "GND" H 2250 2550 50  0000 C CNN
F 2 "" H 2250 2700 60  0000 C CNN
F 3 "" H 2250 2700 60  0000 C CNN
	1    2250 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	2250 2650 2250 2700
$Comp
L GND #PWR03
U 1 1 55283041
P 3300 2700
F 0 "#PWR03" H 3300 2450 50  0001 C CNN
F 1 "GND" H 3300 2550 50  0000 C CNN
F 2 "" H 3300 2700 60  0000 C CNN
F 3 "" H 3300 2700 60  0000 C CNN
	1    3300 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	3200 2650 3300 2650
Wire Wire Line
	3300 2650 3300 2700
$Comp
L HEADER_1 J4
U 1 1 55283329
P 3100 1900
F 0 "J4" H 3100 2050 60  0000 C CNN
F 1 "MOUNT2" H 3100 1750 60  0000 C CNN
F 2 "Screws:4-40-HOLE" H 3100 1900 60  0001 C CNN
F 3 "" H 3100 1900 60  0000 C CNN
	1    3100 1900
	-1   0    0    1   
$EndComp
Wire Wire Line
	2500 1900 2300 1900
Wire Wire Line
	2300 1900 2300 2050
$Comp
L GND #PWR04
U 1 1 55283402
P 3350 2050
F 0 "#PWR04" H 3350 1800 50  0001 C CNN
F 1 "GND" H 3350 1900 50  0000 C CNN
F 2 "" H 3350 2050 60  0000 C CNN
F 3 "" H 3350 2050 60  0000 C CNN
	1    3350 2050
	1    0    0    -1  
$EndComp
Wire Wire Line
	3200 1900 3350 1900
Wire Wire Line
	3350 1900 3350 2050
Text Notes 3750 1650 2    60   ~ 0
Top end mounts/antenna tie-down
Wire Notes Line
	3900 1500 1800 1500
$EndSCHEMATC
