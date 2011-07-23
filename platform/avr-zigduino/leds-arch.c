/*
 * Copyright (c) 2005, Swedish Institute of Computer Science.
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
 * This file is part of the Configurable Sensor Network Application
 * Architecture for sensor nodes running the Contiki operating system.
 *
 * This is a dummy non-functional dummy implementation.
 *
 * $Id: leds-arch.c,v 1.1 2006/12/22 17:05:31 barner Exp $
 *
 * -----------------------------------------------------------------
 *
 * Author  : Adam Dunkels, Joakim Eriksson, Niclas Finne, Simon Barner
 * Created : 2005-11-03
 * Updated : $Date: 2006/12/22 17:05:31 $
 *           $Revision: 1.1 $
 */

#include <avr/io.h>
#include "contiki-conf.h"
#include "dev/leds.h"

/*
 * PORT where LEDs are connected
 */
#define LED_PORT0                        (PORTB)
#define LED_PORT_DIR0                    (DDRB)
#define LED_PORT                        (PORTD)
#define LED_PORT_DIR                    (DDRD)

/*
 * PINs where LEDs are connected
 */
#define LED_PIN_0                       (PB1)
#define LED_PIN_1                       (PD5)
#define LED_PIN_2                       (PD6)

/*---------------------------------------------------------------------------*/
void
leds_arch_init(void)
{
  LED_PORT0 |= (1 << LED_PIN_0);
  LED_PORT_DIR0 |= (1 << LED_PIN_0);
  LED_PORT |= (1 << LED_PIN_1);
  LED_PORT_DIR |= (1 << LED_PIN_1);
  LED_PORT |= (1 << LED_PIN_2);
  LED_PORT_DIR |= (1 << LED_PIN_2);
}
/*---------------------------------------------------------------------------*/
unsigned char
leds_arch_get(void)
{
  unsigned char result = 0;

  if ( LED_PORT0 & _BV(LED_PIN_0 ) )
    result |= LEDS_GREEN;
  if ( LED_PORT & _BV(LED_PIN_1 ) )
    result |= LEDS_YELLOW;
  if ( LED_PORT & _BV(LED_PIN_2 ) )
    result |= LEDS_RED;

  return result;
}

/*---------------------------------------------------------------------------*/
void
leds_arch_set(unsigned char leds)
{
  uint8_t led_port = LED_PORT;

  LED_PORT0 &= ~_BV(LED_PIN_0);
  if ( leds & LEDS_GREEN )
    LED_PORT0 |= _BV(LED_PIN_0);

  led_port &= ~_BV(LED_PIN_1);
  if ( leds & LEDS_YELLOW )
    led_port |= _BV(LED_PIN_1);

  led_port &= ~_BV(LED_PIN_2);
  if ( leds & LEDS_RED )
    led_port |= _BV(LED_PIN_2);

  LED_PORT = led_port;
}
/*---------------------------------------------------------------------------*/
