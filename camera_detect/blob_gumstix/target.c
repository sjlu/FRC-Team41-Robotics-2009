#include <stdlib.h>
#include <gd.h>
#include "blob_gd.h"
#include "target.h"

blob * 
target (blob * red, blob * green, int alliance)
{
  blob * ret = malloc (sizeof (blob));
  if (alliance == ALLIANCE_BLUE)
    while (red != NULL) {
      while (green != NULL) {
        if ((green->center_x > red->x && green->center_y < red->x2) && green->center_y < red->center_y) {
          ret->x = red->x;
          ret->x2 = red->x2;
          ret->y = green->y;
          ret->y2 = red->y2;
          ret->center_x = red->center_x;
          ret->center_y = (red->center_y + green->center_y) / 2;
          ret->n = NULL;
          log_write ("Found a blue target. Center: (%d, %d).\n", ret->center_x, ret->center_y);
          return ret;
        }
        green = green->n;
      }
      red = red->n;
    }
  else
    while (red != NULL) {
      while (green != NULL) {
        if ((green->center_x > red->x && green->center_y < red->x2) && green->center_y > red->center_y) {
          ret->x = red->x;
          ret->x2 = red->x2;
          ret->y = green->y2;
          ret->y2 = red->y;
          ret->center_x = red->center_x;
          ret->center_y = (red->center_y + green->center_y) / 2;
          ret->n = NULL;
          log_write ("Found a red target. Center: (%d, %d).\n", ret->center_x, ret->center_y);
          return ret;
        }
        green = green->n;
      }
      red = red->n;
    }
  free (ret);
  return NULL;
}
