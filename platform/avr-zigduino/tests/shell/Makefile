CONTIKI_PROJECT = zigd-shell
all: $(CONTIKI_PROJECT)

APPS = serial-shell
CONTIKI = ../../../..
CONTIKI_TARGET_SOURCEFILES += shell-udphello.c shell-udp-sendcmd.c
UIP_CONF_IPV6=1
TARGET=avr-zigduino

include $(CONTIKI)/Makefile.include
