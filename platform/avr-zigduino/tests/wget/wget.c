/*
 * Copyright (c) 2002, Adam Dunkels.
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution. 
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.  
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
 *
 * This file is part of the Contiki desktop environment
 *
 * $Id: wget.c,v 1.4 2010/10/31 22:43:06 oliverschmidt Exp $
 *
 */

#include <stdio.h>
#include <string.h>

#include "contiki-net.h"
#include "webclient.h"
#include "cfs/cfs.h"
#include "lib/petsciiconv.h"

PROCESS(wget_process, "Wget");

AUTOSTART_PROCESSES(&wget_process);

#if METHOD_POST
static char url[128] = "http://[fec0::63]/testbed/test.php";
static char formvars[] = "ONE=1&TWO=2";
#else
static char url[128] = "http://[fec0::63]/testbed/test.php?op=server";
#endif

/*-----------------------------------------------------------------------------------*/
/* Called when the URL present in the global "url" variable should be
 * opened. It will call the hostname resolver as well as the HTTP
 * client requester.
 */
static void
start_get(void)
{
  uip_ipaddr_t addr;
  unsigned char i;
  static char host[32];
  char *file;
  register char *urlptr;

  /* Trim off any spaces in the end of the url. */
  urlptr = url + strlen(url) - 1;
  while(*urlptr == ' ' && urlptr > url) {
    *urlptr = 0;
    --urlptr;
  }

  /* Don't even try to go further if the URL is empty. */
  if(urlptr == url) {
    return;
  }

  /* See if the URL starts with http://, otherwise prepend it. */
  if(strncmp(url, http_http, 7) != 0) {
    while(urlptr >= url) {
      *(urlptr + 7) = *urlptr;
      --urlptr;
    }
    strncpy(url, http_http, 7);
  } 

  /* Find host part of the URL. */
  urlptr = &url[7];
  char *hostptr = host;
  for(i = 0; i < sizeof(host); ++i) {
    if(*urlptr == 0 ||
       *urlptr == '/' ||
       *urlptr == ' ' ||
       *urlptr == ' ') {
      *hostptr++ = 0;
      break;
    }
    *hostptr++ = *urlptr;
    ++urlptr;
  }

  /* XXX: Here we should find the port part of the URL, but this isn't
     currently done because of laziness from the programmer's side
     :-) */

  /* Find file part of the URL. */
  if (*urlptr == ']')
    ++urlptr;
  while(*urlptr != '/' && *urlptr != 0) {
    ++urlptr;
  }
  if(*urlptr == '/') {
    file = urlptr;
  } else {
    file = "/";
  }

#if UIP_UDP
  /* First check if the host is an IP address. */
  if(uiplib_ipaddrconv(host, &addr) == 0) {    
    
    /* Try to lookup the hostname. If it fails, we initiate a hostname
       lookup and print out an informative message on the
       statusbar. */
    if(resolv_lookup(host) == NULL) {
      resolv_query(host);
      printf("Resolving host %s...",host);
      return;
    }
  }
#else /* UIP_UDP */
  uiplib_ipaddrconv(host, &addr);
#endif /* UIP_UDP */

#if METHOD_POST
  if(webclient_post(host, 80, file, formvars) == 0) {
    puts("Out of memory error");
  } else {
    printf("Connecting to %s:80 for %s...",host,file);
  }
#else
  /* The hostname we present in the hostname table, so we send out the
     initial GET request. */
  if(webclient_get(host, 80, file) == 0) {
    puts("Out of memory error");
  } else {
    printf("Connecting to %s:80 for %s...",host,file);
  }
#endif
}
/*-----------------------------------------------------------------------------------*/
static void
app_quit(void)
{
  process_exit(&wget_process);
  LOADER_UNLOAD();
}
/*-----------------------------------------------------------------------------------*/
PROCESS_THREAD(wget_process, ev, data)
{
  PROCESS_BEGIN();

  PROCESS_PAUSE();

  printf("\nwget %s...\n",url);

  start_get();

  while(1) {

    PROCESS_WAIT_EVENT();
  
    if(ev == tcpip_event) {
      webclient_appcall(data);
    }
  }

  PROCESS_END();
}
/*-----------------------------------------------------------------------------------*/
/* Callback function. Called from the webclient when the HTTP
 * connection was abruptly aborted.
 */
void
webclient_aborted(void)
{
  puts("Connection reset by peer");
  app_quit();
}
/*-----------------------------------------------------------------------------------*/
/* Callback function. Called from the webclient when the HTTP
 * connection timed out.
 */
void
webclient_timedout(void)
{
  puts("Connection timed out");
  app_quit();
}
/*-----------------------------------------------------------------------------------*/
/* Callback function. Called from the webclient when the HTTP
 * connection was closed after a request from the "webclient_close()"
 * function. .
 */
void
webclient_closed(void)
{  
  puts("Done.");
  app_quit();
}
/*-----------------------------------------------------------------------------------*/
/* Callback function. Called from the webclient when the HTTP
 * connection is connected.
 */
void
webclient_connected(void)
{    
  puts("Request sent...");
}
/*-----------------------------------------------------------------------------------*/
/* Callback function. Called from the webclient module when HTTP data
 * has arrived.
 */
void
webclient_datahandler(char *data, u16_t len)
{
  static unsigned long dload_bytes;

  if(len > 0) {
    dload_bytes += len;
    printf("Downloading (%lu bytes)\n", dload_bytes);
    fwrite(data,1,len,stdout);
  }

  if(data == NULL) {
    printf("Finished downloading %lu bytes.\n", dload_bytes);
    app_quit();
  }
}
/*-----------------------------------------------------------------------------------*/
