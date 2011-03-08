#include <stdio.h>
#include <time.h>
#include <winsock2.h>

#define PORT 1165

char magic_word_check[] = { 0xff, 0xfe, 0xfd, 0xfc };

int
get_file_better (char * filename, char ** res)
{
  FILE * fp = fopen (filename, "rb");
  int size;
  if (fp == NULL)
    return 1;
  fseek (fp, 0, SEEK_END);
  size = ftell (fp);
  fseek (fp, 0, SEEK_SET);
  *res = malloc (size + 1);
  memset (*res, '\0', size+ 1);
  if (size != fread (*res, sizeof (char), size, fp))
    return 2;
  fclose (fp);
  return size;
}
/*
typedef struct {
  unsigned short int num;
  char unused[39];
  float battery;
  float speed[4];
  float current[4];
} packet_t;
*/

typedef struct {
  unsigned short int num;
  char digin;
  char digout;
  unsigned short int battery;
  char status;
  char error;
  unsigned short int teamid;
  char versionmonth1, versionmonth2, versionday1, versionday2;
  char versionyear1, versionyear2;
  char rev1, rev2;
  int unusedbuffer1;
  unsigned short int unusedbuffer2;
  unsigned short int replypacketnum;
} packet_t;

void
print_int_bytes (char * array)
{
  int i;
  for (i = 0; i <= 3; i++)
    printf ("Int: %x\n", array[i]);
}

void
print_right_packet (packet_t * p)
{
  printf ("Packet #: %d\nBatt: %d\nTeamid: %d\n", p->num, p->battery, p->teamid);
}
/*
void
print_new_packet (packet_t * p)
{
  int i;
  printf ("Packet Number: %d\nBattery: %f\n", p->num, p->battery);
  for (i = 0; i <= 3; i++)
    printf ("Speed[%d]: %f\n", i, p->speed[i]);
  for (i = 0; i <= 3; i++)
    printf ("Current[%d]: %f\n", i, p->current[i]);
}*/

void
find_stuff (char * data)
{
  int i;
  float * fp;
  for (i = 0; i <= 400; i++) {
    fp = (float *) data + i;
    if ((*fp > 5) && (*fp < 15))
      printf ("Possible batt.  Offset %d, Value %f\n", i, *fp);
  }
  printf ("done\n");
}

void
get_net (void)
{
  WSADATA info;
  SOCKET recv_socket;
  struct sockaddr_in recv_addr;
  char recv_buff[1024];
  int recv_buff_len = 1024;
  struct sockaddr_in send_addr;
  int send_addr_size = sizeof (send_addr);
  if (WSAStartup (MAKELONG (1, 1), &info) == SOCKET_ERROR) {
    printf ("could not init winsock\n");
    exit (0);
  }
  recv_socket = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  recv_addr.sin_family = AF_INET;
  recv_addr.sin_port = htons (PORT);
  recv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
  bind (recv_socket, (SOCKADDR *) &recv_addr, sizeof (recv_addr));
  while (1) {
    recvfrom (recv_socket, recv_buff, recv_buff_len, 0, (SOCKADDR *) &send_addr, &send_addr_size);
    print_right_packet ((packet_t *) recv_buff);
    //print_new_packet ((packet_t *) recv_buff);
    //find_stuff (recv_buff);
    //print_hex (recv_buff);
  }
  printf ("Received buffer: %s\n", recv_buff);
  closesocket (recv_socket);
  WSACleanup ();
}

void
print_hex (char * d_gram)
{
  int i;
  printf ("Packet begin\n");
  for (i = 0; i <= 1024; i++)
    printf (" %x ", d_gram[i]);
  printf ("Packet end\n\n");
}

int
main (int argc, char ** argv)
{
  printf ("Packet Size: %d\n", sizeof (packet_t));
  char * file;
  switch (argv[1][0]) {
    case 'f':
      get_file_better (argv[2], &file);
      //print_new_packet ((packet_t *) file);
    break;
    case 'n':
      get_net ();
    break;
  }
  return 0;
}
