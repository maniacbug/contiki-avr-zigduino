/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id: battery-sensor.c,v 1.11 2010/08/25 19:30:52 nifi Exp $
 *
 * -----------------------------------------------------------------
 *
 * Author  : Adam Dunkels, Joakim Eriksson, Niclas Finne
 * Created : 2005-11-01
 * Updated : $Date: 2010/08/25 19:30:52 $
 *           $Revision: 1.11 $
 */

#include "dev/battery-sensor.h"
#include "adc.h"

/* Connect Battery(+) to pin A1, via a 1000/470 voltage divider.
 * This will case a battery voltage of 5.0V to read as the max analog
 * voltage of 1.6V.
 */
#define INPUT_CHANNEL 1

const struct sensors_sensor battery_sensor;
/*---------------------------------------------------------------------------*/

/**
 * \return Voltage on battery measurement pin, 4096 is 5V.
 */
static int
value(int type)
{
  return readADC(INPUT_CHANNEL) * 4;
}
/*---------------------------------------------------------------------------*/
static int
configure(int type, int c)
{
  // No configuration needed.  readADC() handles all the config needed.
  return type == SENSORS_ACTIVE;
}
/*---------------------------------------------------------------------------*/
static int
status(int type)
{
  // analog sensors are always ready 
  return 1;
}
/*---------------------------------------------------------------------------*/
SENSORS_SENSOR(battery_sensor, BATTERY_SENSOR, value, configure, status);
