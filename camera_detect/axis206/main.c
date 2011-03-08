// #define DEBUG_IMG

// Russ's Axis206 code.  written  January 2008
// For Team 41, First Robotics Competition

// Includes:
//   winsock.h for sockets to get image from axis camera
//   stdlib.h, stdio.h, string.h for ansi C functions
#include <winsock.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

// SDL Stuff:
//   SDL.h main sdl header
//   SDL_image.h to decode the jpg
//   SDL_ttf.h to display fonts
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include "axis206.h"
#include "blob_sdl.h"
#include "depth.h"
#include "tolerance.h"
#include "target.h"

#define CAM1 "41.0.0.3"
#define CAM2 "41.0.0.2"

#define DEBUG_IMG

int
render_text (SDL_Surface * dest, TTF_Font * font, int x, int y, char * fmt, ...)
{
  static char buffer[2048];
  va_list va_args;
  memset (buffer, '\0', 2048);
  va_start (va_args, fmt);
  vsnprintf (buffer, sizeof (buffer), fmt, va_args);
  va_end (va_args);
  printf ("hi");
  SDL_Color white = {255, 255, 255};
  SDL_Surface * temp = TTF_RenderText_Blended (font, buffer, white);
  if (temp != NULL) {
    easy_blit (x, y, temp, dest);
    SDL_FreeSurface (temp);
    return 0;
  }
  return 1;
}

int
main (int argc, char ** argv)
{
  int numloops = 0;
  int fpstimer;
  double a1, a2;
  tolerance_file_t tol;
  tol.cam1_red.blob_tolerance = 60;
  tol.cam1_red.blob_minlength = 10;
  tol.cam1_red.r = 255;
  tol.cam1_red.g = 30;
  tol.cam1_red.b = 90;
  tol.cam1_green.blob_tolerance = 60;
  tol.cam1_green.blob_minlength = 10;
  tol.cam1_green.r = 255;
  tol.cam1_green.g = 30;
  tol.cam1_green.b = 90;
  tol.cam2_red.blob_tolerance = 60;
  tol.cam2_red.blob_minlength = 10;
  tol.cam2_red.r = 255;
  tol.cam2_red.g = 30;
  tol.cam2_red.b = 90;
  tol.cam2_green.blob_tolerance = 60;
  tol.cam2_green.blob_minlength = 10;
  tol.cam2_green.r = 255;
  tol.cam2_green.g = 30;
  tol.cam2_green.b = 90;
  int do_blob = -1;
  Uint8 mouse_button;
  SDL_Surface * image;
  SDL_Surface * image_2;
  SDL_Surface * screen;
  SDL_Surface * back = IMG_Load ("back.png");
  SDL_Color white = {255, 255, 255};
  SDL_Event event;
  char * jpeg_buff;
  char * jpeg_buff_2;
  int jpeg_buff_size;
  int jpeg_buff_size_2;
  int x, y;
  FILE * log_fp;
  blob * cam1_green;
  blob * cam1_red;
  blob * cam2_green;
  blob * cam2_red;
  blob * vision_targets = NULL;
  if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_TIMER) == -1) {
    SDL_Quit ();
    printf ("SDL Initialization failed\n");
    exit (1);
  }
  if ((screen = SDL_SetVideoMode (1280, 800, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN )) == NULL) {
    SDL_Quit ();
    printf ("Could not create output surface\n");
    exit (1);
  }
  if (TTF_Init () == -1) {
    SDL_Quit ();
    printf ("TTF_Init: %s\n", TTF_GetError());
    exit (1);
  }
  int start = SDL_GetTicks ();
  float lastfps;
  TTF_Font * arial = TTF_OpenFont ("arial.ttf", 12);
  while (1) {
    fpstimer = SDL_GetTicks ();
    numloops++;
    if (fpstimer - start > 10000) {
      start = SDL_GetTicks ();
      lastfps = (float) numloops / 10.0;
      numloops = 0;
    }
    while (SDL_PollEvent (&event)) {
      switch (event.type) {
        case SDL_KEYDOWN:
          switch (event.key.keysym.sym) {
            case 'q':
              TTF_Quit ();
              SDL_Quit ();
              exit (0);
            break;
            case 'b':
              do_blob = -do_blob;
            break;
            case 'i':
              tol.cam1_green.blob_minlength++;
            break;
            case 'k':
              tol.cam1_green.blob_minlength--;
            break;
            case 'o':
              tol.cam1_green.blob_tolerance++;
            break;
            case 'l':
              tol.cam1_green.blob_tolerance--;
            break;
            case 'y':
              tol.cam1_red.blob_minlength++;
            break;
            case 'h':
              tol.cam1_red.blob_minlength--;
            break;
            case 'u':
              tol.cam1_red.blob_tolerance++;
            break;
            case 'j':
              tol.cam1_red.blob_tolerance--;
            break;
            case 'w':
              tol.cam2_green.blob_minlength++;
            break;
            case 's':
              tol.cam2_green.blob_minlength--;
            break;
            case 'e':
              tol.cam2_green.blob_tolerance++;
            break;
            case 'd':
              tol.cam2_green.blob_tolerance--;
            break;
            case 'r':
              tol.cam2_red.blob_minlength++;
            break;
            case 'f':
              tol.cam2_red.blob_minlength--;
            break;
            case 't':
              tol.cam2_red.blob_tolerance++;
            break;
            case 'g':
              tol.cam2_red.blob_tolerance--;
            break;
          }
        break;
        case SDL_MOUSEBUTTONDOWN:
          mouse_button = SDL_GetMouseState (&x,&y);
          if (mouse_button & SDL_BUTTON(1)) {
            if (x > 640) {
              set_tracking (x, y, screen, &(tol.cam2_green));
            }
            else {
              set_tracking (x, y, screen, &(tol.cam1_green)); 
            }
          }
          else {
            if (x > 640) {
              set_tracking (x, y, screen, &(tol.cam2_red));
            }
            else {
              set_tracking (x, y, screen, &(tol.cam1_red)); 
            }
          }
        break;
      }
    }
    #ifndef DEBUG_IMG
    jpeg_buff = get_jpeg (CAM1, &jpeg_buff_size);
    image = IMG_Load_RW (SDL_RWFromMem (jpeg_buff, jpeg_buff_size), 0);
    jpeg_buff_2 = get_jpeg (CAM2, &jpeg_buff_size_2);
    image_2 = IMG_Load_RW (SDL_RWFromMem (jpeg_buff, jpeg_buff_size_2), 0);
    #endif
    #ifdef DEBUG_IMG
    image = IMG_Load ("cam1.jpg");
    image_2 = IMG_Load ("cam2.jpg");
    #endif
    easy_blit (0, 0, back, screen);
    if (do_blob == 1) {
      //render_text (screen, arial, 1130, 500, "FPS: %f", (lastfps));
      cam1_green = find (image, &(tol.cam1_green));
      cam1_red = find (image, &(tol.cam1_red));
      cam2_green = find (image_2, &(tol.cam2_green));
      cam2_red = find (image_2, &(tol.cam2_red));
      vision_targets = target (cam1_red, cam1_green, ALLIANCE_BLUE);
      print_blobs_lighter (image, cam1_green, 0, 150, 0);
      free_blobs (cam1_green);
      print_blobs_lighter (image, cam1_red, 150, 0, 0);
      free_blobs (cam1_red);
      print_blobs_lighter (image_2, cam2_green, 0, 150, 0);
      print_blobs_lighter (image_2, cam2_red, 150, 0, 0);
      free_blobs (cam2_green);
      free_blobs (cam2_red);
      if (vision_targets != NULL) {
        render_text (screen, arial, 100, 490, "Found target!");
        print_blobs (image, vision_targets, 0, 0, 0);
        free_blobs (vision_targets);
      }
      //print_blobs (image_2, cam2_red, 0, 0, 0);
      render_text (screen, arial, 10, 490, "Hotkey list:");
      render_text (screen, arial, 20, 510, "i - increase left green blob minimum length: %d", tol.cam1_green.blob_minlength);
      render_text (screen, arial, 20, 530, "k - decrease left green blob minimum length");
      render_text (screen, arial, 20, 550, "o - increase left green check tolerance: %d", tol.cam1_green.blob_tolerance);
      render_text (screen, arial, 20, 570, "l - decrease left green check tolerance");
      render_text (screen, arial, 20, 590, "y - increase left red blob minimum length: %d", tol.cam1_red.blob_minlength);
      render_text (screen, arial, 20, 610, "h - decrease left red blob minimum length");
      render_text (screen, arial, 20, 630, "u - increase left red check tolerance: %d", tol.cam1_red.blob_tolerance);
      render_text (screen, arial, 20, 650, "j - decrease left red check tolerance");
      render_text (screen, arial, 50, 690, "Green check color: %d, %d, %d", tol.cam1_green.r, tol.cam1_green.g, tol.cam1_green.b);
      SPG_RectFilled (screen, 20, 682, 40, 702, SDL_MapRGB (screen->format, tol.cam1_green.r, tol.cam1_green.g, tol.cam1_green.b));
      render_text (screen, arial, 50, 730, "Red check color: %d, %d, %d", tol.cam1_red.r, tol.cam1_red.g, tol.cam1_red.b);
      SPG_RectFilled (screen, 20, 722, 40, 742, SDL_MapRGB (screen->format, tol.cam1_red.r, tol.cam1_red.g, tol.cam1_red.b));
      render_text (screen, arial, 320, 510, "w - increase right green blob minimum length: %d", tol.cam2_green.blob_minlength);
      render_text (screen, arial, 320, 530, "s - decrease right green blob minimum length");
      render_text (screen, arial, 320, 550, "e - increase right green check tolerance: %d", tol.cam2_green.blob_tolerance);
      render_text (screen, arial, 320, 570, "d - decrease right green check tolerance");
      render_text (screen, arial, 320, 590, "r - increase right red blob minimum length: %d", tol.cam2_red.blob_minlength);
      render_text (screen, arial, 320, 610, "f - decrease right red blob minimum length");
      render_text (screen, arial, 320, 630, "t - increase right red check tolerance: %d", tol.cam2_red.blob_tolerance);
      render_text (screen, arial, 320, 650, "g - decrease right red check tolerance");
      render_text (screen, arial, 350, 690, "Green check color: %d, %d, %d", tol.cam2_green.r, tol.cam2_green.g, tol.cam2_green.b);
      SPG_RectFilled (screen, 320, 682, 340, 702, SDL_MapRGB (screen->format, tol.cam2_green.r, tol.cam2_green.g, tol.cam2_green.b));
      render_text (screen, arial, 350, 730, "Red check color: %d, %d, %d", tol.cam2_red.r, tol.cam2_red.g, tol.cam2_red.b);
      SPG_RectFilled (screen, 320, 722, 340, 742, SDL_MapRGB (screen->format, tol.cam2_red.r, tol.cam2_red.g, tol.cam2_red.b));
  
      /*if ((cam1_green != NULL) && (cam2_red != NULL)) {
        a1 = image_angle (cam1_green->center_x);
        a2 = image_angle (cam2_red->center_x);
        render_text (screen, arial, 10, 760, "Image 1 centroid: %d, %d", cam1_green->center_x, cam1_green->center_y);
        render_text (screen, arial, 10, 780, "Image 2 centroid: %d, %d", cam2_red->center_x, cam2_red->center_y);
        render_text (screen, arial, 10, 800, "Depth, Method 1: %f", find_depth (a1, a2, 0));
        render_text (screen, arial, 10, 820, "Depth, Method 2: %f", find_depth (a1, a2, 1));
        render_text (screen, arial, 10, 840, "Depth, Method 3: %f", find_depth (a1, a2, 2));
        render_text (screen, arial, 10, 860, "Angle, Left: %f", a1 * (180/PI));
        render_text (screen, arial, 10, 880, "Angle, Right: %f", a2 * (180/PI));
      }*/
    }
    easy_blit (0, 0, image, screen);
    easy_blit (640, 0, image_2, screen);
    #ifndef DEBUG_IMG
    SDL_FreeSurface (image);
    SDL_FreeSurface (image_2);
    #endif
    SDL_Flip (screen);
    log_fp = fopen ("colors.tol", "wb");
    fwrite (&tol, sizeof (tolerance_file_t), 1, log_fp);
    fclose (log_fp);
  }
  return 0;
}
