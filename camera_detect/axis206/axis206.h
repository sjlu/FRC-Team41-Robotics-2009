// where to find the image
#define CAM_RESOURCE "/jpg/image.jpg"

// get_jpeg:
//   char * hostname - hostname to access to get the image
//   int * size - int ptr to put the length of the image into
// return value: char * to jpeg in memory
// usage: gets a jpeg from hostname at cam_resource.  uses a 1mb static buffer
char * get_jpeg (char * hostname, int * size);

// str_token:
//   char * str - haystack string
//   char * instr - needle string
//   int strlen - lenght of haystack string
//   int instrlen - length of needle string
//   int * newlen - int * to put length of return value into
// return value: haystack starting at needle
// usage: needed a str token function that didn't stop at null characters
char * str_token (char * str, char * instr, int strlen, int instrlen, int * newlen);

// http_socket_connect:
//   char * hostname - hostname to connect to
// return value: connected tcp socket
// usage: connects the socket for use with winsock
SOCKET http_socket_connect (char * hostname);
