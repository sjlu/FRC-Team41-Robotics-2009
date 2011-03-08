#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include "blob_sdl.h"

void
set_tracking (int x, int y, SDL_Surface * img, tracking_t * col)
{
  Uint8 r, g, b;
  SDL_GetRGB (SPG_GetPixel (img, x, y), img->format, &r, &g, &b);
  col->r = r;
  col->g = g;
  col->b = b;
}

void
validate_lineblobs (line_blob * list)
{
  int t;
  while (list != NULL) {
    if (list->min >= list->max) {
      t = list->min;
      list->min = list->max;
      list->max = t;
    }
    list = list->n;
  }
}

int
check_threshold_sdl (int x, int y, SDL_Surface * cam, tracking_t * tracking)
{
  SDL_Color col;
  Uint8 r, g, b;
  SDL_GetRGB (SPG_GetPixel (cam, x, y), cam->format, &r, &g, &b);
  if (r > tracking->r - tracking->blob_tolerance &&
      r < tracking->r + tracking->blob_tolerance && 
      g > tracking->g - tracking->blob_tolerance && 
      g < tracking->g + tracking->blob_tolerance && 
      b > tracking->b - tracking->blob_tolerance && 
      b < tracking->b + tracking->blob_tolerance) {
    //tracking->r = r;
    //tracking->g = g;
    //tracking->b = b;
    return 1;
  }
  return 0;
}

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
calc_centroid (blob * list)
{
  while (list != NULL) {
	list->center_x = .5 * (list->x2 - list->x) + list->x;
    if (list->y2 > list->y)
      list->center_y = .5 * (list->y2 - list->y) + list->y;
	else
	  list->center_y = .5 * (list->y - list->y2) + list->y2;
    list = list->n;
  }
}

void
print_blobs (SDL_Surface * dest, blob * list, Uint8 r, Uint8 g, Uint8 b)
{
  int center_x;
  int center_y;
  Uint32 color = SDL_MapRGB (dest->format, r, g, b);
  while (list != NULL) {
   // if (((list->x2 - list->x) > 5) && ((list->y2 - list->y) > 5)) {
	  center_x = list->center_x;
      center_y = list->center_y;
      SPG_Pixel (dest, center_x, center_y, color);
      SPG_Line (dest, center_x - 10, center_y, center_x + 10, center_y, color);
      SPG_Line (dest, center_x, center_y - 10, center_x, center_y + 10, color);
      SPG_RectFilledBlend (dest, list->x, list->y, list->x2, list->y2, color, 100);
      SPG_Rect (dest, list->x, list->y, list->x2, list->y2, color);
   //   }
    list = list->n;
  }
}

void
print_blobs_lighter (SDL_Surface * dest, blob * list, Uint8 r, Uint8 g, Uint8 b)
{
  int center_x;
  int center_y;
  Uint32 color = SDL_MapRGB (dest->format, r, g, b);
  while (list != NULL) {
   // if (((list->x2 - list->x) > 5) && ((list->y2 - list->y) > 5)) {
	  center_x = list->center_x;
      center_y = list->center_y;
      SPG_Pixel (dest, center_x, center_y, color);
      SPG_Line (dest, center_x - 10, center_y, center_x + 10, center_y, color);
      SPG_Line (dest, center_x, center_y - 10, center_x, center_y + 10, color);
      SPG_RectFilledBlend (dest, list->x, list->y, list->x2, list->y2, color, 50);
      SPG_Rect (dest, list->x, list->y, list->x2, list->y2, color);
   //   }
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
easy_blit (int x, int y, SDL_Surface * src, SDL_Surface * dest)
{
  SDL_Rect off;
  off.x = x;
  off.y = y;
  SDL_BlitSurface (src, NULL, dest, &off);
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
      //if (((line_1->max < line_2->min) || (line_1->min > line_2->max))) { // very bad hack, todo: fix
      if (1) {
        if (line_1->b == NULL) {
          if ((*blobs) == NULL) {
            (*blobs) = malloc (sizeof (blob));
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
          line_1->b->x2 = line_2->line;
          if (line_2->min < line_1->b->y)
            line_1->b->y = line_2->min;
          if (line_2->max > line_1->b->y2)
            line_1->b->y2 = line_2->max;
        }
      }
      line_2 = line_2->n;
    }
    line_1 = line_1->n;
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
find (SDL_Surface * image, tracking_t * col)
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
      if (check_threshold_sdl (i, j, image, col)) {
        b_started = 1;
        b_length++;
      }
      else if (b_started) {
        if (b_length > col->blob_minlength) {
          add_lineblob (&curr_lines, j - b_length, j, i);
          b_started = 0;
          b_length = 0;
        } else {
          b_length = 0;
          b_started = 0;
        }
      }
    }
    validate_lineblobs (curr_lines);
    if (i != 0) {
      check_lineblobs (&detected, prev_lines, curr_lines);
      free_list (prev_lines);
    }
    prev_lines = curr_lines;
    curr_lines = NULL;
  }
  free_list (prev_lines);
  calc_centroid (detected);
  return detected;
}
