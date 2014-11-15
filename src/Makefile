# This is the master ntpstick makefile
#
# It builds libkakapo correctly, and then builds ntpstick, finally generating
# a firmware image if required or flashing directly over an external
# programmer

MCU=atxmega64d4
TOOLBASE=/usr/local/avr
F_CPU=32000000
export MCU TOOLBASE PORT F_CPU

firmware:
	( cd libkakapo; make)
	( cd ntpstick; make)

clean:
	( cd libkakapo; make clean )
	( cd ntpstick; make clean )

flash:
	( cd ntpstick; make ext-program )

