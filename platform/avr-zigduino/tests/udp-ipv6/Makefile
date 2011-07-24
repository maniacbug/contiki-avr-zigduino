all: udp-server udp-client
TARGET = avr-zigduino
UIP_CONF_IPV6 = 1
CFLAGS += -DUDP_CONNECTION_ADDR=fdfd::$(NODE)
CONTIKI = ../../../..
include $(CONTIKI)/Makefile.include
