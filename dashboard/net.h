typedef struct {
  unsigned short int num;
  char unused[39];
  float battery;
  float speed[4];
  float current[4];
} packet_t;

void net_init (packet_t ** packetptr);

void net_quit (void);

void net_receive (void);
