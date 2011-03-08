// Russell Frank's Blob Detection Code
// Written January - March 09

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// Change this to BLOB_GD if you want the GD code enabled
#define BLOB_SDL

#ifdef BLOB_SDL
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#define IMG_TYPE SDL_Surface *
#endif

#ifdef BLOB_GD
#include <gd.h>
#define IMG_TYPE gdImagePtr
#endif

#include "blob.h"

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


int
check_threshold (int x, int y, IMG_TYPE cam, tracking_t * tracking)
{
  #ifdef BLOB_GD
  int r, g, b;
  int c = gdImageGetPixel (cam, x, y);
  r = gdImageRed (cam, c);
  g = gdImageGreen (cam, c);
  b = gdImageBlue (cam, c);
  #endif
  #ifdef BLOB_SDL
  SDL_Color col;
  Uint8 r, g, b;
  SDL_GetRGB (SPG_GetPixel (cam, x, y), cam->format, &r, &g, &b);
  #endif
  if (r > tracking->r - tracking->blob_tolerance &&
      r < tracking->r + tracking->blob_tolerance && 
      g > tracking->g - tracking->blob_tolerance && 
      g < tracking->g + tracking->blob_tolerance && 
      b > tracking->b - tracking->blob_tolerance && 
      b < tracking->b + tracking->blob_tolerance) {
    return 1;
  }
  return 0;
}

#ifdef BLOB_GD
void
print_blobs (gdImagePtr dest, blob * list, Uint8 r, Uint8 g, Uint8 b)
{
  int fill_color = gdTrueColorAlpha (r, g, b, gdAlphaTransparent / 3);
  int line_color = gdTrueColorAlpha (r, g, b, gdAlphaOpaque);
  while (list != NULL) {
    gdImageLine (dest, list->center_x - 10, list->center_y, list->center_x + 10, list->center_y, line_color);
    gdImageLine (dest, list->center_x, list->center_y - 10, list->center_x, list->center_y + 10, line_color);
    gdImageFilledRectangle (dest, list->x, list->y, list->x2, list->y2, fill_color);
    gdImageRectangle (dest, list->x, list->y, list->x2, list->y2, line_color);
    list = list->n;
  }
}
#endif

#ifdef BLOB_SDL
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
print_blobs (SDL_Surface * dest, blob * list, Uint8 r, Uint8 g, Uint8 b)
{
  int center_x;
  int center_y;
  Uint32 color = SDL_MapRGB (dest->format, r, g, b);
  while (list != NULL) {
	center_x = list->center_x;
    center_y = list->center_y;
    SPG_Pixel (dest, center_x, center_y, color);
    SPG_Line (dest, center_x - 10, center_y, center_x + 10, center_y, color);
    SPG_Line (dest, center_x, center_y - 10, center_x, center_y + 10, color);
    SPG_RectFilledBlend (dest, list->x, list->y, list->x2, list->y2, color, 100);
    SPG_Rect (dest, list->x, list->y, list->x2, list->y2, color);
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
	center_x = list->center_x;
    center_y = list->center_y;
    SPG_Pixel (dest, center_x, center_y, color);
    SPG_Line (dest, center_x - 10, center_y, center_x + 10, center_y, color);
    SPG_Line (dest, center_x, center_y - 10, center_x, center_y + 10, color);
    SPG_RectFilledBlend (dest, list->x, list->y, list->x2, list->y2, color, 50);
    SPG_Rect (dest, list->x, list->y, list->x2, list->y2, color);
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
#endif

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
free_line_blobs (line_blob * list)
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
      if (((line_1->max > line_2->min) && (line_1->min < line_2->max))) {
        if (line_1->b == NULL) {
          if ((*blobs) == NULL) {
            (*blobs) = malloc (sizeof (blob));
            next = (*blobs);
          } else {
            next = malloc (sizeof (blob));
            last = (*blobs)->e;
            last->n = next;
          }
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

blob *
find (IMG_TYPE image, tracking_t * col)
{
  line_blob * prev_lines = NULL;
  line_blob * curr_lines = NULL;
  blob * detected = NULL;
  int i, j;
  int b_length = 0;
  int b_started = 0;
  for (i = 0; i < image->w; i++) {
    for (j = 0; j < image->h; j++) {
      if (check_threshold (i, j, image, col)) {
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
      free_line_blobs (prev_lines);
    }
    prev_lines = curr_lines;
    curr_lines = NULL;
  }
  free_line_blobs (prev_lines);
  calc_centroid (detected);
  return detected;
}
