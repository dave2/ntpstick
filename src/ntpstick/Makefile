# Where is the toolchain unpacked?
TOOLBASE  ?= /usr/local/avr
# What MCU do we have on this board?
MCU       ?= atxmega64d4
# Port that Kakapo popped up on
PORT      ?= /dev/ttyUSB0
# Application name
APP       = ntpstick
# If using libkakapo, uncomment
LIBKAKAPO = -lkakapo
#Tools we'll need
CC        = $(TOOLBASE)/bin/avr-gcc
AVRDUDE   = /usr/bin/avrdude
OBJCOPY   = $(TOOLBASE)/bin/avr-objcopy
SIZE	  = $(TOOLBASE)/bin/avr-size
CFLAGS    = -Os --std=c99 -funroll-loops -funsigned-char -funsigned-bitfields -fpack-struct
CFLAGS   += -fshort-enums -Wstrict-prototypes -Wall -mcall-prologues -iquote. -I../.
CFLAGS   += -mmcu=$(MCU)
INCLUDE   = -L../libkakapo
OBJ       = $(patsubst %.c,%.o,$(wildcard *.c))
SIZEFLAGS =
ifdef F_CPU
CFLAGS	+= -DF_CPU=$(F_CPU)
endif

build: $(APP).hex

ntpstick: $(APP).eep
	$(AVRDUDE) -p $(MCU) -c avr109 -P $(PORT) -b 115200 -U eeprom:w:$(APP).eep

program: $(APP).hex
	$(AVRDUDE) -p $(MCU) -c avr109 -P $(PORT) -b 115200 -U flash:w:$(APP).hex -e

ext-program: $(APP).hex
	$(AVRDUDE) -p $(MCU) -c avrisp2 -P usb -U application:w:$(APP).hex

$(APP).hex: $(APP).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(APP).eep : $(APP).elf
	$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O ihex $< $@

$(APP).elf: $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $@ $(LIBKAKAPO)
	$(SIZE) $(SIZEFLAGS) $@

%.o: %.c %.h Makefile
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJ) $(APP).hex $(APP).elf $(APP).eep

