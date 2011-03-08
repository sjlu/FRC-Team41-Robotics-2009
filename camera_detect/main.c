#include <stdio.h>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include "cv.h"
#include "highgui.h"

#define CAMERA_DIST_BETWEEN 24
#define CAMERA_VIEW_FIELD 0.942477796
#define IMAGE_WIDTH 320
#define IMAGE_HEIGHT 288
#define PI 3.14159265358979323846

int check_red_r = 90;
int check_red_g = 10;
int check_red_b = 10;

int check_tolerance = 30;

int blob_tolerance = 10;

int blob_minlength = 10;

#define MSG_WELCOME "Depth Finder\n"
#define MSG_ENTER_X1 "Enter first x value: "
#define MSG_ENTER_X2 "Enter second x value: "

double
image_angle (double x)
{
  return ((x*CAMERA_VIEW_FIELD)/IMAGE_WIDTH) + ((PI/2)-(CAMERA_VIEW_FIELD/2));
}

double
find_depth (double x, double x2)
{
  double a1 = image_angle (x);
  double a2 = image_angle (x2);
  printf ("Angle 1: %f; Angle 2: %f\n", a1, a2);
  if ((a1 >= (PI/2)) && (a2 <= (PI/2)))
    return (CAMERA_DIST_BETWEEN * sin(a1)*sin(a2)) / sin(PI-(a1+a2));
  else if (a2 > (PI/2))
    return (CAMERA_DIST_BETWEEN * sin(a1)*sin(a2)) / sin(PI-(a1+(PI-a2)));
  else if (a1 < (PI/2))
    return (CAMERA_DIST_BETWEEN * sin(a1)*sin(a2)) / sin(PI-(a2+(PI-a1)));
  else
    return -1;
}

double
find_depth_angle (double a1, double a2)
{
  if ((a1 >= (PI/2)) && (a2 <= (PI/2)))
    return (CAMERA_DIST_BETWEEN * sin(a1)*sin(a2)) / sin(PI-(a1+a2));
  else if (a2 > (PI/2))
    return (CAMERA_DIST_BETWEEN * sin(a1)*sin(a2)) / sin(PI-(a1+(PI-a2)));
  else if (a1 < (PI/2))
    return (CAMERA_DIST_BETWEEN * sin(a1)*sin(a2)) / sin(PI-(a2+(PI-a1)));
  else
    return -1;
}

SDL_Surface *
ipl_to_surface (IplImage * opencvimg)
{
  SDL_Surface * surface = SDL_CreateRGBSurfaceFrom((void*)opencvimg->imageData,
    opencvimg->width,
    opencvimg->height,
    opencvimg->depth * opencvimg->nChannels,
    opencvimg->widthStep,
    0xff0000, 0x00ff00, 0x0000ff, 0
  );
  return surface;
}

void
easy_blit (int x, int y, SDL_Surface * src, SDL_Surface * dest)
{
  SDL_Rect off;
  off.x = x;
  off.y = y;
  SDL_BlitSurface (src, NULL, dest, &off);
}

Uint32
getpixel (SDL_Surface * surface, int x, int y)
{
  int bpp = surface->format->BytesPerPixel;
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
  switch(bpp) {
  case 1:
    return *p;
  case 2:
    return *(Uint16 *)p;
  case 3:
    if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
      return p[0] << 16 | p[1] << 8 | p[2];
    else
      return p[0] | p[1] << 8 | p[2] << 16;
  case 4:
    return *(Uint32 *)p;
  default:
    return 0;
  }
}

int
check_threshold_sdl (int x, int y, SDL_Surface * cam)
{
  SDL_Color col;
  Uint8 r, g, b;
  SDL_GetRGB (SPG_GetPixel (cam, x, y), cam->format, &r, &g, &b);
  if (r > check_red_r - check_tolerance && r < check_red_r + check_tolerance && g > check_red_g - check_tolerance && g < check_red_g + check_tolerance && b > check_red_b - check_tolerance && b < check_red_b + check_tolerance)
    return 1;
  return 0;
}

void
print_rgbs (int x, int y, SDL_Surface * img)
{
  Uint8 r, g, b;
  SDL_GetRGB (SPG_GetPixel (img, x, y), img->format, &r, &g, &b);
  check_red_r = r;
  check_red_g = g;
  check_red_b = b;
}

void
set_threshold (IplImage * img, SDL_Surface * camera)
{
  int i, j;
  for (i = 0; i <= img->width; i++)
    for (j = 0; j <= img->height; j++)
      if (check_threshold_sdl (i, j, camera))
        SPG_Pixel (camera, i, j, 0);
}

typedef struct {
  unsigned int x, y, x2, y2;
  void * n;
  void * e;
} blob;

typedef struct {
  unsigned int line, min, max;
  void * n;
  void * e;
  blob * b;
} line_blob;
/*
void
print_blobs (line_blob ** line_blobs)
{
  int i;
  line_blob * line;
  for (i = 0; i < IMAGE_HEIGHT; i++) {
    line = line_blobs[i];
    while (line != NULL) {
      printf ("lineblob: min: %d max: %d\n", line->min, line->max);
      line = line->n;
    }
  }
}*/

void
draw_lineblob (SDL_Surface * image, int x, int y, int length)
{
  int i;
  int col = x * 10000;
  for (i = 0; i <= length; i++) {
    SPG_Pixel (image, x + 320, y-i, col);
  }
}

void
free_blobs (blob * list)
{
  blob * free_me;
  while (list != NULL) {
    free_me = list;
    list = list->n;
    free (free_me);
  }
}

void
free_list (line_blob * list)
{
  line_blob * free_me;
  while (list != NULL) {
    free_me = list;
    list = list->n;
    free (free_me);
  }
}

void
add_lineblob (line_blob ** list, unsigned int min, unsigned int max, unsigned int line)
{
  line_blob * last;
  line_blob * new = malloc (sizeof (line_blob));
  new->min = min;
  new->max = max;
  new->line = line;
  new->b = NULL;
  new->n = NULL;
  if ((*list) == NULL) {
    (*list) = new;
    (*list)->e = new;
  } else {
    last = (*list)->e;
    last->n = new;
    (*list)->e = new;
  }
}

void
check_lineblobs (blob ** blobs, line_blob * line_1, line_blob * line_2)
{
  blob * next = NULL;
  blob * last = NULL;
  while (line_1 != NULL) {
    while (line_2 != NULL) {
      if (1) {
        if (line_1->b == NULL) {
          printf ("Line 1 is unattatched\n");
          if ((*blobs) == NULL) {
            (*blobs) = malloc (sizeof (blob));
            //(*blobs)->x = line_1->line;
            //(*blobs)->y = line_1->min;
           /* (*blobs)->w = 2;
            if ((line_1->max - line_1->min) > (line_2->max - line_2->min))
              (*blobs)->h = line_1->max - line_1->min;
            else
              (*blobs)->h = line_2->max - line_2->min;*/
            (*blobs)->x = line_1->line;
            (*blobs)->x2 = line_2->line;
            if (line_1->min < line_2->min)
              (*blobs)->y = line_1->min;
            else
              (*blobs)->y = line_2->min;
            if (line_1->max > line_2->max)
              (*blobs)->y2 = line_1->max;
            else
              (*blobs)->y2 = line_2->max;
            (*blobs)->e = (*blobs);
            (*blobs)->n = NULL;
            line_2->b = line_1->b = (*blobs);
          } else {
            next = malloc (sizeof (blob));
            last = (*blobs)->e;
            last->n = next;
            next->x = line_1->line;
            next->x2 = line_2->line;
            if (line_1->min > line_2->min)
              next->y = line_1->min;
            else
              next->y = line_2->min;
            if (line_1->max < line_2->max)
              next->y2 = line_1->max;
            else
              next->y2 = line_2->max;
            next->e = NULL;
            next->n = NULL;
            (*blobs)->e = next;
            line_2->b = line_1->b = next;
          }
        } else {
          line_2->b = line_1->b;
          printf ("Line 1 is attatched\n");
          line_1->b->x2 = line_2->line;
          if (line_2->min < line_1->b->y)
            line_1->b->y = line_2->min;
          if (line_2->max > line_1->b->y2)
            line_1->b->y2 = line_2->max;
          //if ((line_2->max - line_2->min) > line_1->b->h)
          //  line_1->b->h = line_2->max - line_2->min;
        }
      }
      line_2 = line_2->n;
    }
    line_1 = line_1->n;
  }
}

void
print_blobs (SDL_Surface * dest, blob * list)
{
  int center_x;
  int center_y;
  printf ("in print_blobs\n");
  while (list != NULL) {
    if (((list->x2 - list->x) > 5) && ((list->y2 - list->y) > 5)) {
      center_x = .5 * (list->x2 - list->x) + list->x;
      center_y = .5 * (list->y2 - list->y) + list->y;
      SPG_Pixel (dest, center_x, center_y, 0);
      SPG_Line (dest, center_x - 10, center_y, center_x + 10, center_y, 0);
      SPG_Line (dest, center_x, center_y - 10, center_x, center_y + 10, 0);
      SPG_Rect (dest, list->x, list->y, list->x2, list->y2, 0);
    }
    list = list->n;
  }
}

void
draw_lineblobs (SDL_Surface * image, line_blob * list)
{
  while (list != NULL) {
    SPG_Line (image, list->line, list->min, list->line, list->max, rand() * 10000);
    list = list->n;
  }
}


void
print_lineblobs (line_blob * list)
{
  while (list != NULL) {
    printf ("Lineblob: %d; %d to %d", list->line, list->min, list->max);
    list = list->n;
  }
}

blob *
find (SDL_Surface * image, SDL_Surface * screen)
{
  line_blob * prev_lines = NULL;
  line_blob * curr_lines = NULL;
  blob * detected = NULL;
  int i, j;
  int b_length = 0;
  int b_started = 0;
  int b_tol = 0;
  for (i = 0; i < image->w; i++) {
    for (j = 0; j < image->h; j++) {
      if (check_threshold_sdl (i, j, image)) {
        b_started = 1;
        b_length++;
      }
      else if (b_started) {
          if (b_length > blob_minlength) {
          //draw_lineblob (screen, i, j, b_length);
          add_lineblob (&curr_lines, j, j - b_length, i);
          b_started = 0;
          b_length = 0;
        } else {
          b_length = 0;
          b_started = 0;
        }
      }
    }
    if (i != 0) {
      check_lineblobs (&detected, prev_lines, curr_lines);
      free_list (prev_lines);
    }
    prev_lines = curr_lines;
    //draw_lineblobs (image, prev_lines);
    curr_lines = NULL;
  }
  free_list (prev_lines);
  return detected;
}

int
main (int argc, char ** argv)
{
  int do_blob = -1;
  printf ("sdfsdf");
  SDL_Event event;
  SDL_Surface * screen;
  SDL_Surface * camera;
  SDL_Surface * camera_2;
  SDL_Surface * back = IMG_Load ("back.PNG");
  CvCapture * capture;
  CvCapture * capture_2;
  IplImage * image;
  IplImage * image_2;
  blob * image_1_blobs = NULL;
  blob * image_2_blobs = NULL;
  printf ("hissd!");
  int x, y;
  if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_TIMER) == -1) {
    SDL_Quit ();
    printf ("SDL Initialization failed\n");
    exit (1);
  }
  if ((screen = SDL_SetVideoMode (900, 576, 32, SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL) {
    SDL_Quit ();
    printf ("Could not create output surface\n");
    exit (1);
  }
  if (TTF_Init () == -1) {
    SDL_Quit ();
    printf ("TTF_Init: %s\n", TTF_GetError());
    exit (1);
  }
  if (!(capture = cvCaptureFromCAM (0))) {
    SDL_Quit ();
    printf ("Failed to start capture\n");
    exit (1);
  }
  if (!(capture_2 = cvCaptureFromCAM (1))) {
    SDL_Quit ();
    printf ("Failed to start capture\n");
    exit (1);
  }
  printf ("hi!");
  SDL_WM_SetCaption ("Camera", NULL);
  while (1) {
    easy_blit (0, 0, back, screen);
    image = cvQueryFrame (capture);
    camera = ipl_to_surface (image);
    //set_threshold (image, camera);
    printf ("Before blob calc\n");
    image_2 = cvQueryFrame (capture_2);
    camera_2 = ipl_to_surface (image_2);
    if (do_blob == 1) {
      image_1_blobs = find (camera, screen);
      print_blobs (camera, image_1_blobs);
      free_blobs (image_1_blobs);
      image_2_blobs = find (camera_2, screen);
      print_blobs (camera_2, image_2_blobs);
      free_blobs (image_2_blobs);
    }
    easy_blit (0, 0, camera, screen);
    easy_blit (0, 288, camera_2, screen);
    SDL_Flip (screen);
    while (SDL_PollEvent (&event)) {
      switch (event.type) {
        case SDL_KEYDOWN:
          switch (event.key.keysym.sym) {
            case 'q':
              TTF_Quit ();
              SDL_Quit ();
              exit (0);
            break;
            case 'w':
              blob_tolerance++;
            break;
            case 's':
              blob_tolerance--;
            break;
            case 'b':
              do_blob = -do_blob;
            break;
            case 'u':
              blob_minlength++;
            break;
            case 'j':
              blob_minlength--;
            break;
            case 'o':
              check_tolerance++;
            break;
            case 'l':
              check_tolerance--;
            break;
          }
        break;
        case SDL_MOUSEBUTTONDOWN:
          SDL_GetMouseState (&x,&y);
          print_rgbs (x, y, screen);
        break;
      }
    }
    SDL_FreeSurface (camera);
    SDL_FreeSurface (camera_2);
  }
  cvReleaseCapture (&capture);
}
