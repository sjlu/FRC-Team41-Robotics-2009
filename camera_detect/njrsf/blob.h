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
  unsigned int line;
  unsigned int min;
  unsigned int max;
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

// free_blobs:
//   blob * list - list of blobs to free
// return value: void
// usage: frees a list of blobs
void free_blobs (blob * list);

// find:
//   SDL_Surface * image - surface to search for blob in
//   tracking_t * col - colors to search for
// return value: linked list of blobs
// usage: finds blobs in an SDL_Surface
blob * find (IMG_TYPE image, tracking_t * col);

#ifdef BLOB_SDL // Begin SDL Specific Code

// print_blobs:
//   SDL_Surface * dest - surface to draw blobs onto
//   Uint8 r, g, b - red green blue values of blob rectangle
// return value: void
// usage: prints a list of blobs to an SDL_Surface
void print_blobs (SDL_Surface * dest, blob * list, Uint8 r, Uint8 g, Uint8 b);

void print_blobs_lighter (SDL_Surface * dest, blob * list, Uint8 r, Uint8 g, Uint8 b);

void set_tracking (int x, int y, SDL_Surface * img, tracking_t * col);

void easy_blit (int x, int y, SDL_Surface * src, SDL_Surface * dest);
#endif

#ifdef BLOB_GD
void print_blobs (gdImagePtr dest, blob * list, Uint8 r, Uint8 g, Uint8 b);
#endif

