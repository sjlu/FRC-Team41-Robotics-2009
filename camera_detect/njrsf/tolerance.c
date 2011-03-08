#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#define BLOB_SDL
#define IMG_TYPE SDL_Surface *

#include "blob.h"
#include "tolerance.h"

int
get_file_better (char * filename, char ** res)
{
  FILE * fp = fopen (filename, "rb");
  int size;
  if (fp == NULL)
    return 1;
  fseek (fp, 0, SEEK_END);
  size = ftell (fp);
  fseek (fp, 0, SEEK_SET);
  *res = malloc (size + 1);
  memset (*res, '\0', size+ 1);
  if (size != fread (*res, sizeof (char), size, fp))
    return 2;
  fclose (fp);
  return size;
}

tolerance_file_t *
get_tolerance (char * filename)
{
  tolerance_file_t * ret;
  get_file_better (filename, (char **) &ret);
  return ret;
}
