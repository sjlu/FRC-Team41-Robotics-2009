#include <stdio.h>
#include <time.h>
#include <winsock2.h>

#include "log.h"
#include "net.h"

#define DATA_PORT 1165

struct {
  WSADATA info;
  SOCKET recv_socket;
  struct sockaddr_in recv_addr;
  char recv_buff[1024];
  int recv_buff_len;
  struct sockaddr_in send_addr;
  int send_addr_size;
} net_global;

void
net_init (packet_t ** packetptr)
{
  if (WSAStartup (MAKELONG (1, 1), &(net_global.info)) == SOCKET_ERROR) {
    log_write ("EE: Could not init winsock.  Exiting.");
    exit (1);
  }
  net_global.recv_socket = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  net_global.recv_addr.sin_family = AF_INET;
  net_global.recv_addr.sin_port = htons (DATA_PORT);
  net_global.recv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
  bind (net_global.recv_socket, (SOCKADDR *) &(net_global.recv_addr), sizeof (net_global.recv_addr));
  *packetptr = (packet_t *) net_global.recv_buff;
  net_global.recv_buff_len = 1024;
  net_global.send_addr_size = sizeof (net_global.send_addr);
}

void
net_quit (void)
{
  closesocket (net_global.recv_socket);
}

void
net_receive (void)
{
  recvfrom (net_global.recv_socket, net_global.recv_buff, net_global.recv_buff_len, 0, (SOCKADDR *) &(net_global.send_addr), &(net_global.send_addr_size));
}
