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
 * $Id: shell-udp-sendcmd.c,v 1.2 2009/11/08 19:43:00 adamdunkels Exp $
 */

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "contiki-conf.h"
#include "shell.h"

#include "dev/leds.h"

#if CONTIKI_TARGET_NETSIM
#include "ether.h"
#endif /* CONTIKI_TARGET_NETSIM */

#include <stdio.h>
#ifndef HAVE_SNPRINTF
int snprintf(char *str, size_t size, const char *format, ...);
#endif /* HAVE_SNPRINTF */
#include <string.h>

#define DEBUG DEBUG_PRINT
#include "net/uip-debug.h"

#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])

#define MAX_PAYLOAD_LEN 120

static struct uip_udp_conn *server_conn;

#define MAX_SERVERLEN 16

static struct uip_udp_conn *udpconn;

static uip_ipaddr_t serveraddr;
static char server[MAX_SERVERLEN + 1];
/*---------------------------------------------------------------------------*/
PROCESS(shell_sendcmd_process, "sendcmd");
PROCESS(shell_sendcmd_host_process, "sendcmd host");
PROCESS(shell_sendcmd_receiver_process, "sendcmd receiver");
SHELL_COMMAND(sendcmd_command,
          "sendcmd",
          "sendcmd <IP addr> <command>: send a command to the specified node",
          &shell_sendcmd_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_sendcmd_host_process, ev, data)
{
  static struct process *child_command;
  int err;
  PROCESS_BEGIN();

  /* XXX: direct output to null. */
  /*  printf("sendcmd server got command string '%s'\n", (char *)data);*/
  err = shell_start_command(data, strlen((char * )data), NULL, &child_command);
  if(err == SHELL_FOREGROUND && process_is_running(child_command)) {
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_EXIT ||
                 (ev == PROCESS_EVENT_EXITED &&
                  data == child_command));
    if(ev == PROCESS_EVENT_EXIT) {
      process_exit(child_command);
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
static void
tcpip_handler(void)
{
  if(uip_newdata())
  {
    ((char *)uip_appdata)[uip_datalen()] = 0;
    PRINTF("\nUDP Command Server received: '%s' from ", (char *)uip_appdata);
    PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
    PRINTF("\n");
    
    /* Start the server process with the incoming command. */
    process_start(&shell_sendcmd_host_process, (char *)uip_appdata);
  }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_sendcmd_receiver_process, ev, data)
{
  PROCESS_BEGIN();
  PROCESS_PAUSE();
  PRINTF("UDP command server listening on 3002\n");

  server_conn = udp_new(NULL, UIP_HTONS(3003), NULL);
  udp_bind(server_conn, UIP_HTONS(3002));

  while(1)
  {
    PROCESS_YIELD();
    if(ev == tcpip_event)
    {
      tcpip_handler();
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_sendcmd_process, ev, data)
{
  struct cmd_msg;
  int len;
  char *nextptr;
  char buf[32];

  PROCESS_BEGIN();

  nextptr = strchr(data,' ');
  if(!nextptr) {
    shell_output_str(&sendcmd_command,
             "sendcmd <IP addr>: receiver must be specified", "");
    PROCESS_EXIT();
  }
  else
    *nextptr++ = 0;

  shell_output_str(&sendcmd_command, "Sending command to ", data);

  /* Get the length of the command line, excluding a terminating NUL character. */
  len = strlen((char *)nextptr);

  /* Check the length of the command line to see that it is small
     enough to fit in a packet. We count with 32 bytes of header,
     which may be a little too much, but at least we are on the safe
     side. */
  if(len > MAX_PAYLOAD_LEN) {
    snprintf(buf, sizeof(buf), "%d", len);
    shell_output_str(&sendcmd_command, "command line too large: ", buf);
    PROCESS_EXIT();
  }

  uiplib_ipaddrconv(data, &serveraddr);
  udpconn = udp_new(&serveraddr, uip_htons(3002), NULL);
  udp_bind(udpconn, uip_htons(3003));
  uip_udp_packet_send(udpconn, nextptr, ++len);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
void
shell_udp_sendcmd_init(void)
{
  shell_register_command(&sendcmd_command);
  process_start(&shell_sendcmd_receiver_process,NULL);
}
// vim:cin:ai:sts=2 sw=2
/*---------------------------------------------------------------------------*/
