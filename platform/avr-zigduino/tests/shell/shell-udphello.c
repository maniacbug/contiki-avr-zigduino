/*
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
 */

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "shell.h"

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
PROCESS(udp_server_process, "UDP server process");
/*---------------------------------------------------------------------------*/
static void
tcpip_handler(void)
{
  static int seq_id;
  char buf[MAX_PAYLOAD_LEN];

  if(uip_newdata())
  {
    ((char *)uip_appdata)[uip_datalen()] = 0;
    PRINTF("\nUDP Server received: '%s' from ", (char *)uip_appdata);
    PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
    PRINTF("\n");

    // If this is a client 'HELLO', then respond.
    if (!strncmp(uip_appdata,"HELLO",5))
    {
      uip_ipaddr_copy(&server_conn->ripaddr, &UIP_IP_BUF->srcipaddr);
      PRINTF("Responding with message: ");
      sprintf(buf, "Return UDP from the server (%d)", ++seq_id);
      PRINTF("%s\n", buf);

      udpconn = udp_new(&UIP_IP_BUF->srcipaddr, uip_htons(3000), NULL);
      udp_bind(udpconn, uip_htons(3001));
      uip_udp_packet_send(server_conn, buf, strlen(buf));
      /* Restore server connection to allow data from any node */
      memset(&server_conn->ripaddr, 0, sizeof(server_conn->ripaddr));
    }
  }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_process, ev, data)
{
  PROCESS_BEGIN();
  PROCESS_PAUSE();
  PRINTF("UDP server listening on 3000\n");

  server_conn = udp_new(NULL, UIP_HTONS(3001), NULL);
  udp_bind(server_conn, UIP_HTONS(3000));

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
PROCESS(shell_udphello_process, "udphello");
SHELL_COMMAND(udphello_command,
	      "udphello",
	      "udphello <host>: send UDP 'Hello' message",
	      &shell_udphello_process);
/*---------------------------------------------------------------------------*/

static void
send_line(char *line, int len)
{
  uip_udp_packet_send(udpconn, line, len);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_udphello_process, ev, data)
{
  PROCESS_BEGIN();

  if(strlen(data) > sizeof(server)) {
    shell_output_str(&udphello_command, "Too long input", "");
    PROCESS_EXIT();
  }
  strncpy(server, data, sizeof(server));
  /* NULL-terminate the server string. */
  server[sizeof(server)-1] = 0;

  uiplib_ipaddrconv(server, &serveraddr);
  udpconn = udp_new(&serveraddr, uip_htons(3000), NULL);
  udp_bind(udpconn, uip_htons(3001));
  
  send_line("HELLO",6);
  PRINTF("Sending HELLO to %s:3000\n",data);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
void
shell_udphello_init(void)
{
  shell_register_command(&udphello_command);
  process_start(&udp_server_process,NULL);
}
/*---------------------------------------------------------------------------*/
// vim:cin:ai:sts=2 sw=2
/*---------------------------------------------------------------------------*/
