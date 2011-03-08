// Russ's Axis206 code
// Written January 2009 for FRC 41

// axis206.c
// Code for getting a jpeg image from the axis206 network camera.
// Uses winsock to get the image from the http stream into a static
// 1mb buffer, then parses out the http headers and returns the
// image data.

#include <winsock.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "axis206.h"

// jpeg header so I can parse out all the http headers
char jpeg_header[] = { 0xff, 0xd8, 0xff, 0xe0, 0x00, 0x10, 
0x4a, 0x46, 0x49, 0x46 };

SOCKET
http_socket_connect (char * hostname) {
  WSADATA info;
  struct sockaddr_in sa;
  struct hostent * hp;
  SOCKET s;
  if (WSAStartup(MAKELONG(1, 1), &info) == SOCKET_ERROR) {
    printf ("could not init socket library\n");
    exit (0);
  } 
  hp = gethostbyname (hostname);
  if (hp==NULL) {
    printf ("cannot find host\n");
    exit (0);
  }
  memset (&sa, 0, sizeof (sa));
  memcpy ((char *)&sa.sin_addr, hp->h_addr, hp->h_length);
  sa.sin_family = hp->h_addrtype;
  sa.sin_port = htons ((u_short)80);
  s = socket (hp->h_addrtype, SOCK_STREAM, 0);
  if (s == INVALID_SOCKET) {
    printf ("invalid socket\n");
    exit (0);
  }
  if (connect (s, (struct sockaddr *)&sa, sizeof (sa)) == SOCKET_ERROR) {
    closesocket (s);
    printf ("invalid socket 2\n");
    exit (0);
  }
  return s;
}

char *
str_token (char * str, char * instr, int strlen, int instrlen, int * newlen)
{
  int i, j;
  for (i = 0; i < strlen; i++) {
    for (j = 0; j < instrlen; j++) {
      if (str[i+j] != instr[j])
        break;
      if (j == instrlen-1) {
        *newlen = strlen - i;
        return str + i;
      }
    }
  }
  return NULL;
}

char *
get_jpeg (char * hostname, int * size)
{
  static int sockopen = 0;
  static char recv_buff[1000005];
  int rw = 1;
  int totalsize = 0;
  char send_buff[1024];
  snprintf (send_buff, sizeof(send_buff), "GET %s HTTP/1.1\r\nHOST: %s\r\nConnection: Keep-Alive\r\nKeep-Alive: 5000\r\n\r\n\0", CAM_RESOURCE, hostname);
  int send_buff_size = strlen(send_buff);
  char * img_begin;
  static SOCKET http_socket;
  memset (recv_buff, '\0', 1000000);
  if (!sockopen)
    http_socket = http_socket_connect (hostname);
  send (http_socket, send_buff, send_buff_size, 0);
  while (rw > 0) {
    rw = recv (http_socket, recv_buff + totalsize, 1000000 - totalsize, 0);
    totalsize += rw;
  }
  img_begin = str_token (recv_buff, jpeg_header, totalsize, 10, size);
  //closesocket (http_socket);
  return img_begin;
}
