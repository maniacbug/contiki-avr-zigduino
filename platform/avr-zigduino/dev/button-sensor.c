/* Dummy sensor routine */

#include "lib/sensors.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"

#if ARDUINO_CORE
#include "wiring.h"
#endif

#if 0
const struct sensors_sensor button_sensor;
static int status(int type);
struct sensors_sensor *sensors[1];
unsigned char sensors_flags[1];
#endif

#define BUTTON_PIN 7
#define IRQ_NUM 3

static int enabled = 0;
static struct timer debouncetimer;

static int
status(int type)
{
	switch (type) {
	case SENSORS_ACTIVE:
	case SENSORS_READY:
		return enabled;
	}
	return 0;
}

#if ARDUINO_CORE
static void
isr(void)
{
    if(timer_expired(&debouncetimer)) {
      timer_set(&debouncetimer, CLOCK_SECOND / 20);
      sensors_changed(&button_sensor);
      leds_toggle(LEDS_YELLOW);
	}
}

static int
value(int type)
{
	// Button returns LOW when pressed
	return digitalRead(BUTTON_PIN) ? 0 : 1;
}

static int
configure(int type, int c)
{
	switch (type) {
	case SENSORS_ACTIVE:
		if (c) {
			if(!status(SENSORS_ACTIVE)) {
				pinMode(BUTTON_PIN,INPUT);
				digitalWrite(BUTTON_PIN,HIGH);
      				timer_set(&debouncetimer, 0);
				attachInterrupt(IRQ_NUM,isr,CHANGE);
				enabled = 1;
  				leds_on(LEDS_GREEN);
			}
		} else {
			detachInterrupt(IRQ_NUM);
			enabled = 0;
  			leds_off(LEDS_GREEN);
		}
		return 1;
	}
	return 0;
}
#else

static int
value(int type)
{
	return 0;
}

static int
configure(int type, int c)
{
	switch (type) {
	case SENSORS_ACTIVE:
		if (c) {
			if(!status(SENSORS_ACTIVE)) {

			}
		} else {
		}
		return 1;
	}
	return 0;
}
#endif
SENSORS_SENSOR(button_sensor, BUTTON_SENSOR,
	       value, configure, status);

