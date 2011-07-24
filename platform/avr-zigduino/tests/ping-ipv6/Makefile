CONTIKI_PROJECT = ping-ipv6
all: $(CONTIKI_PROJECT)
CONTIKI_TARGET_SOURCEFILES += ping6.c 
CFLAGS += -DNODE=$(NODE)
UIP_CONF_IPV6=1
TARGET=avr-zigduino
CONTIKI = ../../../..
include $(CONTIKI)/Makefile.include
