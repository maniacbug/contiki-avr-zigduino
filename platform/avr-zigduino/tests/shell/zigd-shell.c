/*
 * Copyright (c) 2009, Swedish Institute of Computer Science.
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
 * This file is part of the Contiki operating system.
 *
 * $Id: example-shell.c,v 1.3 2010/02/03 20:37:52 adamdunkels Exp $
 */

/**
 * \file
 *         Contiki shell example
 * \author
 *         Fredrik Osterlind <fros@sics.se>
 */

#include "contiki.h"
#include "shell.h"
#include "serial-shell.h"

#include "net/rime.h"
#include "dev/leds.h"
#include "net/rime/timesynch.h"

#include <stdio.h>
#include <string.h>

void shell_udphello_init(void);
void shell_udp_sendcmd_init(void);

/*---------------------------------------------------------------------------*/
PROCESS(example_shell_process, "Contiki shell");
AUTOSTART_PROCESSES(&example_shell_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_shell_process, ev, data)
{
  PROCESS_BEGIN();

  /* Only shell components proven to work on Zigduino */
  serial_shell_init();
  shell_blink_init();

  shell_ps_init();
  shell_time_init();
  shell_text_init();
  shell_ping_init();
  shell_netstat_init();
  shell_udphello_init();
  shell_udp_sendcmd_init();

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
