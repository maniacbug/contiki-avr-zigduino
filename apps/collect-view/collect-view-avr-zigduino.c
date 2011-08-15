#include "collect-view-sky.h"
#include "dev/button-sensor.h"
#include "dev/temperature-sensor.h"
#include "dev/battery-sensor.h"

/*---------------------------------------------------------------------------*/
void
collect_view_arch_read_sensors(struct collect_view_data_msg *msg)
{
  msg->sensors[BATTERY_INDICATOR] = button_sensor.value(0);
  msg->sensors[BATTERY_VOLTAGE_SENSOR] = battery_sensor.value(0);  // 4096 = 5V
  msg->sensors[TEMP_SENSOR] = temperature_sensor.value(0) + 3960; // 3960 = 0.0C, +1 = +0.01C
}
/*---------------------------------------------------------------------------*/
