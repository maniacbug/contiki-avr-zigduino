CONTIKI_PROJECT = wget
all: $(CONTIKI_PROJECT)

APPS = webbrowser

WITH_UIP6=1
UIP_CONF_IPV6=1

ifeq ($(METHOD_POST),1)
CFLAGS+=-DMETHOD_POST=1
endif

CONTIKI = ../../../..
include $(CONTIKI)/Makefile.include
