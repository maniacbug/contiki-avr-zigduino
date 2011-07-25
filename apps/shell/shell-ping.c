/*
 * Copyright (c) 2004, Adam Dunkels.
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
 * Author: Adam Dunkels <adam@sics.se>
 *
 * $Id: shell-ping.c,v 1.6 2011/01/12 22:58:34 nifi Exp $
 */

#include <string.h>
#include <stddef.h>

#include "contiki.h"
#include "shell.h"
#include "contiki-net.h"

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF(" %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x ", ((u8_t *)addr)[0], ((u8_t *)addr)[1], ((u8_t *)addr)[2], ((u8_t *)addr)[3], ((u8_t *)addr)[4], ((u8_t *)addr)[5], ((u8_t *)addr)[6], ((u8_t *)addr)[7], ((u8_t *)addr)[8], ((u8_t *)addr)[9], ((u8_t *)addr)[10], ((u8_t *)addr)[11], ((u8_t *)addr)[12], ((u8_t *)addr)[13], ((u8_t *)addr)[14], ((u8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF(" %02x:%02x:%02x:%02x:%02x:%02x ",lladdr->addr[0], lladdr->addr[1], lladdr->addr[2], lladdr->addr[3],lladdr->addr[4], lladdr->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#endif

/*---------------------------------------------------------------------------*/
PROCESS(shell_ping_process, "ping");
SHELL_COMMAND(ping_command,
	      "ping",
	      "ping <host>: ping an IP host",
	      &shell_ping_process);
/*---------------------------------------------------------------------------*/

#define UIP_IP_BUF     ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_ICMP_BUF   ((struct uip_icmp_hdr *)&uip_buf[UIP_LLH_LEN + UIP_IPH_LEN])
#define PING_DATALEN 16

#define ICMP_ECHO_REPLY 0
#define ICMP_ECHO       8

#define ICMP6_ECHO_REPLY             129
#define ICMP6_ECHO                   128

static uip_ipaddr_t remoteaddr;
static unsigned char running;
/*---------------------------------------------------------------------------*/
static void
send_ping(uip_ipaddr_t *dest_addr)
#if UIP_CONF_IPV6
{
  static uint16_t count;
  UIP_IP_BUF->vtc = 0x60;
  UIP_IP_BUF->tcflow = 1;
  UIP_IP_BUF->flow = 0;
  UIP_IP_BUF->proto = UIP_PROTO_ICMP6;
  UIP_IP_BUF->ttl = uip_ds6_if.cur_hop_limit;
  uip_ipaddr_copy(&UIP_IP_BUF->destipaddr, dest_addr);
  uip_ds6_select_src(&UIP_IP_BUF->srcipaddr, &UIP_IP_BUF->destipaddr);
  
  UIP_ICMP_BUF->type = ICMP6_ECHO_REQUEST;
  UIP_ICMP_BUF->icode = 0;
  /* set identifier and sequence number to 0 */
  memset((uint8_t *)UIP_ICMP_BUF + UIP_ICMPH_LEN, 0, 4);
  /* put one byte of data */
  memset((uint8_t *)UIP_ICMP_BUF + UIP_ICMPH_LEN + UIP_ICMP6_ECHO_REQUEST_LEN,
	 count, PING_DATALEN);
  count++;
  
  uip_len = UIP_ICMPH_LEN + UIP_ICMP6_ECHO_REQUEST_LEN +
    UIP_IPH_LEN + PING_DATALEN;
  UIP_IP_BUF->len[0] = (u8_t)((uip_len - 40) >> 8);
  UIP_IP_BUF->len[1] = (u8_t)((uip_len - 40) & 0x00ff);
  
  UIP_ICMP_BUF->icmpchksum = 0;
  UIP_ICMP_BUF->icmpchksum = ~uip_icmp6chksum();
  
  PRINTF("Sending Echo Request to");
  PRINT6ADDR(&UIP_IP_BUF->destipaddr);
  PRINTF("from");
  PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
  PRINTF("\n");
  
  tcpip_ipv6_output();
}
#else /* UIP_CONF_IPV6 */
{
  static uint16_t ipid = 0;
  static uint16_t seqno = 0;
  
  UIP_IP_BUF->vhl = 0x45;
  UIP_IP_BUF->tos = 0;
  UIP_IP_BUF->ipoffset[0] = UIP_IP_BUF->ipoffset[1] = 0;
  ++ipid;
  UIP_IP_BUF->ipid[0] = ipid >> 8;
  UIP_IP_BUF->ipid[1] = ipid & 0xff;
  UIP_IP_BUF->proto = UIP_PROTO_ICMP;
  UIP_IP_BUF->ttl = UIP_TTL;

  uip_ipaddr_copy(&UIP_IP_BUF->destipaddr, dest_addr);
  uip_ipaddr_copy(&UIP_IP_BUF->srcipaddr, &uip_hostaddr);
  
  UIP_ICMP_BUF->type = ICMP_ECHO;
  UIP_ICMP_BUF->icode = 0;
  UIP_ICMP_BUF->id = 0xadad;
  UIP_ICMP_BUF->seqno = uip_htons(seqno++);
  
  uip_len = UIP_ICMPH_LEN + UIP_IPH_LEN + PING_DATALEN;
  UIP_IP_BUF->len[0] = (u8_t)((uip_len) >> 8);
  UIP_IP_BUF->len[1] = (u8_t)((uip_len) & 0x00ff);
  
  UIP_ICMP_BUF->icmpchksum = 0;
  UIP_ICMP_BUF->icmpchksum = ~uip_chksum((u16_t *)&(UIP_ICMP_BUF->type),
					 UIP_ICMPH_LEN + PING_DATALEN);

  /* Calculate IP checksum. */
  UIP_IP_BUF->ipchksum = 0;
  UIP_IP_BUF->ipchksum = ~(uip_ipchksum());

  tcpip_output();
}
#endif /* UIP_CONF_IPV6 */
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_ping_process, ev, data)
{
  static struct etimer e;
  struct shell_input *input;

  PROCESS_BEGIN();

  if(data == NULL) {
    shell_output_str(&ping_command,
		     "ping <server>: server as address", "");
    PROCESS_EXIT();
  }
  uiplib_ipaddrconv(data, &remoteaddr);
  icmp6_new(NULL);

  send_ping(&remoteaddr);
  
  running = 1;

  while(running) {
    etimer_set(&e, CLOCK_SECOND * 10);
    
    PROCESS_WAIT_EVENT();
  
#if UIP_CONF_IPV6
    if(ev == tcpip_icmp6_event) {
      switch(*((uint8_t *)data)){
      case ICMP6_ECHO_REPLY:
	PRINTF("Echo reply received.\n");
	break;
      default:
	PRINTF("Other ICMP6 message received.\n");
      }
    }
#endif

    if(etimer_expired(&e)) {
      PROCESS_EXIT();      
    }
    
    if(ev == shell_event_input) {
      input = data;
      if(input->len1 + input->len2 == 0) {
	PROCESS_EXIT();
      }
#if 0
    } else if(ev == resolv_event_found) {
      /* Either found a hostname, or not. */
      if((char *)data != NULL &&
	 resolv_lookup((char *)data) != NULL) {
	uip_ipaddr_copy(serveraddr, ipaddr);
	telnet_connect(&s, server, serveraddr, nick);
      } else {
	shell_output_str(&ping_command, "Host not found.", "");
      }
#endif /* 0 */
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
void
shell_ping_init(void)
{
  shell_register_command(&ping_command);
}
/*---------------------------------------------------------------------------*/
