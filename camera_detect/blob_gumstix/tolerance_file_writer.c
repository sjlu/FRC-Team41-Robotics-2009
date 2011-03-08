#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char Uint8;

typedef struct {
  Uint8 r, g, b;
  int blob_tolerance;
  int blob_minlength;
} tracking_t;

typedef struct {
  tracking_t cam1_red;
  tracking_t cam1_green;
  tracking_t cam2_red;
  tracking_t cam2_green;
} tolerance_file_t;

void
main (void)
{
  tolerance_file_t * tol = malloc (sizeof (tolerance_file_t));
  tol->cam2_red.blob_tolerance = 80;
  tol->cam2_red.blob_minlength = 10;
  tol->cam2_red.r = 255;
  tol->cam2_red.g = 45;
  tol->cam2_red.b = 97;
  tol->cam1_red.blob_tolerance = 80;
  tol->cam1_red.blob_minlength = 10;
  tol->cam1_red.r = 224;
  tol->cam1_red.g = 49;
  tol->cam1_red.b = 88;
  tol->cam1_green.blob_tolerance = 70;
  tol->cam1_green.blob_minlength = 10;
  tol->cam1_green.r = 83;
  tol->cam1_green.g = 233;
  tol->cam1_green.b = 33;
  tol->cam2_green.blob_tolerance = 70;
  tol->cam2_green.blob_minlength = 10;
  tol->cam2_green.r = 107;
  tol->cam2_green.g = 205;
  tol->cam2_green.b = 0;
  
  log_fp = fopen ("colors.tol", "wb");
  fwrite (tol, sizeof (tolerance_file_t), 1, log_fp);
  fclose (log_fp);
}
