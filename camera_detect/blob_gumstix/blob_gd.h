typedef unsigned char Uint8;

// blob structure:
//   unsigned int x - min x
//   unsigned int y - min y
//   unsigned int x2 - max x
//   unsigned int y2 - max y
//   void * n - pointer to next blob, NULL if last blob in list
//   void * e - pointer to the last blob in the list
// usage: stores blob information in a linked list
typedef struct {
  unsigned int x, y, x2, y2;
  int center_x, center_y;
  void * n;
  void * e;
} blob;

// line_blob structure:
//   unsigned int line - the line # that this lineblob is on
//   unsigned int min - minimum value
//   unsigned int max - maximum value
//   void * n - pointer to next lineblob in the list, NULL if last lineblob
//   void * e - pointer to last lineblob in the list
//   blob * b - pointer to a blob if this lineblob is attached, NULL if not
// usage: stores lineblob information in a linked list
typedef struct {
  unsigned int line, min, max;
  void * n;
  void * e;
  blob * b;
} line_blob;

// tracking_t structure:
//   Uint8 r - red color to track
//   Uint8 g - green color to track
//   Uint8 b - blue color to track
//   int blob_tolerance - blob detection tolerance
//   int blob_minlength - minimum length of lineblobs
// usage: stores tracking threshold detection evaluation data
typedef struct {
  Uint8 r, g, b;
  int blob_tolerance;
  int blob_minlength;
} tracking_t;

blob * find (gdImagePtr image, tracking_t * col);
