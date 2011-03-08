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

void set_tracking (int x, int y, SDL_Surface * img, tracking_t * col);

// check_threshold_sdl:
//   int x - x value of pixel to check
//   int y - y value of pixel to check
//   SDL_Surface * cam - surface to check
//   col_2 - int value to distinguish which color set to check with
// return value: 0 or 1 if it passes or fails the pixel threshold
//   inspection value tolerance check thing
// usage: checks the threshold of a pixel to do actual blob detection stuff
int check_threshold_sdl (int x, int y, SDL_Surface * cam, tracking_t * tracking);

// draw_lineblob:
//   SDL_Surface * image - surface to draw lineblob onto
//   int x - x value of start of lineblob
//   int y - y value of start of lineblob
//   int length - length of lineblob
// return value: void
// usage: draws a single lineblob with color according to the x value
void draw_lineblob (SDL_Surface * image, int x, int y, int length);

// print_blobs:
//   SDL_Surface * dest - surface to draw the blob rects to
//   blob * list - list of blobs to draw
//   Uint32 color - color of rectangles and centerpoints
// return value: void
// usage: draws a list of blobs onto the screen
void print_blobs (SDL_Surface * dest, blob * list, Uint8 r, Uint8 g, Uint8 b);

// draw_lineblobs:
//   SDL_Surface * image - surface to draw lineblobs onto
//   line_blob * list - list of lineblobs to draw to the dest surface
// return value: void
// usage: draws a list of lineblobs to the destination surface
void draw_lineblobs (SDL_Surface * image, line_blob * list);

// easy_blit:
//   int x - x value on dest surface
//   int y - y value on dest surface
//   SDL_Surface * src - surface to blit
//   SDL_Surface * dest - surface to blit onto
// return value: void
// usage: blits stuff without having to use sdl_rects cause thats a pain
void easy_blit (int x, int y, SDL_Surface * src, SDL_Surface * dest);

// free_blobs:
//   blob * list - list of blobs to free
// return value: void
// usage: i don't know maybe frees a list of blobs or something 
void free_blobs (blob * list);

// find:
//   SDL_Surface * image - image to find blobs in
//   SDL_Surface * screen - screen to draw debug lineblobs onto
//   int col - passed to the threshold checker function to tell which color
//     set to pick
// return value: list of blobs
// usage: finds waldo
blob * find (SDL_Surface * image, tracking_t * col);

// free_list:
//   line_blob * list - list of lineblobs to free
// return value: void
// usage: frees a list of lineblobs
void free_list (line_blob * list);

// add_lineblob:
//   line_blob ** list - reference to a line_blob pointer to start the list
//     at
//   unsigned int min - min value of lineblob
//   unsigned int max - max value of lineblob
//   unsigned int line - line that this lineblob is on
// return value: void
// usage: adds a lineblob onto a list
void add_lineblob (line_blob ** list, unsigned int min, unsigned int max, unsigned int line);

// print_lineblobs:
//   line_blob * list - list of lineblobs to print
// return value: void
// usage: prints a list of lineblobs to stdout
void print_lineblobs (line_blob * list);

// check_lineblobs:
//   blob ** blobs - reference to a blob pointer to continue the list at
//   line_blob * line_1 - list of blobs to check against line_2
//   line_blob * line_2 - list of blobs to check against line_1
// return value: void
// usage: checks 2 lists of lineblobs against eachother
void check_lineblobs (blob ** blobs, line_blob * line_1, line_blob * line_2);


void print_blobs_lighter (SDL_Surface * dest, blob * list, Uint8 r, Uint8 g, Uint8 b);


