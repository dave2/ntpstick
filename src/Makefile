# This is the master ntpstick makefile
#
# It builds libkakapo correctly, and then builds ntpstick, finally generating
# a firmware image if required or flashing directly over an external
# programmer

MCU=atxmega128a4u
TOOLBASE=/usr/local/avr
export MCU TOOLBASE

firmware: 
	( cd libkakapo; make )
	( cd ntpstick; make )

clean:
	( cd libkakapo; make clean )
	( cd ntpstick; make clean )


