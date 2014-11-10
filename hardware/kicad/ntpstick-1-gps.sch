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
LIBS:ntpstick-1-cache
EELAYER 24 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 4 4
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text HLabel 2800 3500 0    60   Input ~ 0
GPS_RX
Text HLabel 2800 3400 0    60   Output ~ 0
GPS_TX
Text HLabel 4800 3600 2    60   Output ~ 0
GPS_PPS
$Comp
L UBLOX-GPS U4
U 1 1 526239C4
P 3850 3200
F 0 "U4" V 3850 3200 60  0000 C CNN
F 1 "UBLOX-GPS" V 3750 3200 60  0000 C CNN
F 2 "" H 3850 3200 60  0000 C CNN
F 3 "" H 3850 3200 60  0000 C CNN
	1    3850 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	3000 3400 2800 3400
Wire Wire Line
	3000 3500 2800 3500
Wire Wire Line
	4600 3600 4800 3600
$Comp
L GND #PWR053
U 1 1 526239FA
P 4800 3200
F 0 "#PWR053" H 4800 3200 30  0001 C CNN
F 1 "GND" H 4800 3130 30  0001 C CNN
F 2 "" H 4800 3200 60  0000 C CNN
F 3 "" H 4800 3200 60  0000 C CNN
	1    4800 3200
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4600 3200 4800 3200
$Comp
L GND #PWR054
U 1 1 52623E6D
P 2900 3950
F 0 "#PWR054" H 2900 3950 30  0001 C CNN
F 1 "GND" H 2900 3880 30  0001 C CNN
F 2 "" H 2900 3950 60  0000 C CNN
F 3 "" H 2900 3950 60  0000 C CNN
	1    2900 3950
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR055
U 1 1 52623E73
P 2850 2800
F 0 "#PWR055" H 2850 2800 30  0001 C CNN
F 1 "GND" H 2850 2730 30  0001 C CNN
F 2 "" H 2850 2800 60  0000 C CNN
F 3 "" H 2850 2800 60  0000 C CNN
	1    2850 2800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR056
U 1 1 52623E79
P 4800 2900
F 0 "#PWR056" H 4800 2900 30  0001 C CNN
F 1 "GND" H 4800 2830 30  0001 C CNN
F 2 "" H 4800 2900 60  0000 C CNN
F 3 "" H 4800 2900 60  0000 C CNN
	1    4800 2900
	1    0    0    -1  
$EndComp
Wire Wire Line
	3000 3800 2900 3800
Wire Wire Line
	2900 3800 2900 3950
Wire Wire Line
	3000 2600 2850 2600
Wire Wire Line
	2850 2600 2850 2800
Wire Wire Line
	4600 2800 5000 2800
Wire Wire Line
	4800 2600 4800 2900
Wire Wire Line
	4600 2600 5000 2600
Connection ~ 4800 2800
$Comp
L SMA_EDGE SMA_E1
U 1 1 52623ED3
P 5350 2700
F 0 "SMA_E1" H 5500 3000 60  0000 C CNN
F 1 "SMA_EDGE" H 5500 2900 60  0000 C CNN
F 2 "" H 5350 2700 60  0000 C CNN
F 3 "" H 5350 2700 60  0000 C CNN
	1    5350 2700
	1    0    0    -1  
$EndComp
Connection ~ 4800 2600
Wire Wire Line
	5000 2700 4600 2700
$EndSCHEMATC
