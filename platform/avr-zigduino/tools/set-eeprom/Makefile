# Usage: make NODE=3 AVRDUDE_PORT=/dev/ttyUSB0
all: set-eeprom.eu
CFLAGS += -DNODE=$(NODE)
%.eep: %.o
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O ihex $^ $@

TARGET = avr-zigduino
CONTIKI = ../../../..

include $(CONTIKI)/Makefile.include
