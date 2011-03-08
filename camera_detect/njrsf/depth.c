#include <math.h>

#include "depth.h"

double
image_angle (double x)
{
  return ((x*CAMERA_VIEW_FIELD)/IMAGE_WIDTH) + ((PI/2)-(CAMERA_VIEW_FIELD/2));
}

double
find_depth (double x, double x2, int method)
{
  double a1 = x;
  double a2 = x2;
  printf ("Angle 1: %f; Angle 2: %f\n", a1, a2);
//  if ((a1 >= (PI/2)) && (a2 <= (PI/2)))
  if (method == 0)
    return (CAMERA_DIST_BETWEEN * sin(a1)*sin(a2)) / sin(PI-(a1+a2));
//  else if (a2 > (PI/2))
  else if (method == 1)
    return (CAMERA_DIST_BETWEEN * sin(a1)*sin(a2)) / sin(PI-(a1+(PI-a2)));
//  else if (a1 < (PI/2))
  else if (method == 2)
    return (CAMERA_DIST_BETWEEN * sin(a1)*sin(a2)) / sin(PI-(a2+(PI-a1)));
  else
    return -1;
}
