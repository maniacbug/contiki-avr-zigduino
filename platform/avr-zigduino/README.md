# Contiki for Zigduino

Created by maniacbug <maniacbug@ymail.com>

For details on the Zigduino platform on Contiki, please see my wiki:
https://github.com/maniacbug/contiki-avr-zigduino/wiki/Home

## Zigduino Board Specifics

The Zigduino board is an atmega128rfa1 board with a few additions:

* Arduino form factor and shield compatibility
* 3 LED''s, mapped to LED_GREEN, LED_YELLOW, and LED_BLUE
* Only 1 UART exposed, so you have to choose between SLIP output and debugging output

The avr-zigduino platform also specifies some additional optional peripherals:

* Button on pin 7, connected to ground when pressed
* Battery voltage divider on pin A1, divides battery voltage form 5V to 1.6V
