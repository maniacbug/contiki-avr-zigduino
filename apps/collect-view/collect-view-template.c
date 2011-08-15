#include "collect-view-sky.h"

/*---------------------------------------------------------------------------*/
void
collect_view_arch_read_sensors(struct collect_view_data_msg *msg)
{
  msg->sensors[SENSOR1] = 0;
  msg->sensors[SENSOR2] = 0;
  msg->sensors[BATTERY_VOLTAGE_SENSOR] = 0;
  msg->sensors[BATTERY_INDICATOR] = 0;
  msg->sensors[LIGHT1_SENSOR] = 0;
  msg->sensors[LIGHT2_SENSOR] = 0;
  msg->sensors[TEMP_SENSOR] = 0;
  msg->sensors[HUMIDITY_SENSOR] = 0;
  msg->sensors[RSSI_SENSOR] = 0;
  msg->sensors[ETX1_SENSOR] = 0;
  msg->sensors[ETX2_SENSOR] = 0;
  msg->sensors[ETX3_SENSOR] = 0;
  msg->sensors[ETX4_SENSOR] = 0;
}
/*---------------------------------------------------------------------------*/
