#include <gd.h>

#include "blob_gd.h"

int
check_threshold_gd (int x, int y, gdImagePtr cam, tracking_t * tracking)
{
  int r, g, b;
  int c = gdImageGetPixel (cam, x, y);
  r = gdImageRed (cam, c);
  g = gdImageGreen (cam, c);
  b = gdImageBlue (cam, c);
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
  unsigned int t_min, t_max;
  while (line_1 != NULL) {
    while (line_2 != NULL) {
      //if (((line_1->max < line_2->min) || (line_1->min > line_2->max))) {
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
          if (line_2->min < line_2->max) {
            t_min = line_2->min;
            t_max = line_2->max;
          } else {
            t_min = line_2->max;
            t_max = line_2->min;
          }
          if (t_min < line_1->b->y)
            line_1->b->y = t_min;
          if (t_max > line_1->b->y2)
            line_1->b->y2 = t_max;
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

void
validate_lineblobs (line_blob * list)
{
  int t;
  while (list != NULL) {
    if (list->min >= list->max) {
      log_write ("EE: lineblob has reversed min/max! Line: %d  Min: %d  Max: %d", list->line, list->min, list->max);
      t = list->min;
      list->min = list->max;
      list->max = t;
    }
    list = list->n;
  }
}

blob *
find (gdImagePtr image, tracking_t * col)
{
  line_blob * prev_lines = NULL;
  line_blob * curr_lines = NULL;
  blob * detected = NULL;
  int i, j;
  int b_length = 0;
  int b_started = 0;
  int b_tol = 0;
  for (i = 0; i < gdImageSX (image); i++) {
    for (j = 0; j < gdImageSY (image); j++) {
	//TODO: fix wrap around bug like if j resets back to 0 and yo do not clear the blob
      if (check_threshold_gd (i, j, image, col)) {
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
    //validate_lineblobs (curr_lines);
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
