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

#include "cv.h"
#include "highgui.h"

// SDL Stuff:
//   SDL.h main sdl header
//   SDL_image.h to decode the jpg
//   SDL_ttf.h to display fonts
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#define BLOB_SDL
#define IMG_TYPE SDL_Surface *

#include "blob.h"

#include "depth.h"
#include "tolerance.h"
#include "target.h"

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
  tolerance_file_t * tol = get_tolerance ("colors.tol");
  memset (tol, '\0', sizeof (tolerance_file_t));
  int do_blob = -1;
  int do_text = -1;
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
  CvCapture * capture;
  CvCapture * capture_2;
  IplImage * cam_img;
  IplImage * cam_img_2;
  FILE * color_fp;
  if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_TIMER) == -1) {
    SDL_Quit ();
    printf ("SDL Initialization failed\n");
    exit (1);
  }
  if ((screen = SDL_SetVideoMode (800, 600, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN )) == NULL) {
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
  int start = SDL_GetTicks ();
  int lastfps;
  TTF_Font * arial = TTF_OpenFont ("arial.ttf", 12);
  while (1) {
    fpstimer = SDL_GetTicks ();
    numloops++;
    if (fpstimer - start > 1000) {
      start = SDL_GetTicks ();
      lastfps = numloops;
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
            case 'v':
              do_text = -do_text;
            break;
            case 'i':
              tol->cam1_green.blob_minlength++;
            break;
            case 'k':
              tol->cam1_green.blob_minlength--;
            break;
            case 'o':
              tol->cam1_green.blob_tolerance++;
            break;
            case 'l':
              tol->cam1_green.blob_tolerance--;
            break;
            case 'y':
              tol->cam1_red.blob_minlength++;
            break;
            case 'h':
              tol->cam1_red.blob_minlength--;
            break;
            case 'u':
              tol->cam1_red.blob_tolerance++;
            break;
            case 'j':
              tol->cam1_red.blob_tolerance--;
            break;
            case 'w':
              tol->cam2_green.blob_minlength++;
            break;
            case 's':
              tol->cam2_green.blob_minlength--;
            break;
            case 'e':
              tol->cam2_green.blob_tolerance++;
            break;
            case 'd':
              tol->cam2_green.blob_tolerance--;
            break;
            case 'r':
              tol->cam2_red.blob_minlength++;
            break;
            case 'f':
              tol->cam2_red.blob_minlength--;
            break;
            case 't':
              tol->cam2_red.blob_tolerance++;
            break;
            case 'g':
              tol->cam2_red.blob_tolerance--;
            break;
            case 'z':
              color_fp = fopen ("colors.tol", "wb");
              fwrite (tol, sizeof (tolerance_file_t), 1, color_fp);
              fclose (color_fp);
            break;
          }
        break;
        case SDL_MOUSEBUTTONDOWN:
          mouse_button = SDL_GetMouseState (&x,&y);
          if (mouse_button & SDL_BUTTON(1)) {
            if (x > 352) {
              set_tracking (x, y, screen, &(tol->cam2_green));
            }
            else {
              set_tracking (x, y, screen, &(tol->cam1_green)); 
            }
          }
          else {
            if (x > 352) {
              set_tracking (x, y, screen, &(tol->cam2_red));
            }
            else {
              set_tracking (x, y, screen, &(tol->cam1_red)); 
            }
          }
        break;
      }
    }
    cam_img = cvQueryFrame (capture);
    image = ipl_to_surface (cam_img);
    cam_img_2 = cvQueryFrame (capture_2);
    image_2 = ipl_to_surface (cam_img_2);
    easy_blit (0, 0, back, screen);
    if (do_blob == 1) {
      cam1_green = find (image, &(tol->cam1_green));
      cam1_red = find (image, &(tol->cam1_red));
      cam2_green = find (image_2, &(tol->cam2_green));
      cam2_red = find (image_2, &(tol->cam2_red));
      vision_targets = target (cam1_red, cam1_green, ALLIANCE_BLUE);
      print_blobs_lighter (image, cam1_green, 0, 150, 0);
      print_blobs_lighter (image, cam1_red, 150, 0, 0);
      print_blobs_lighter (image_2, cam2_green, 0, 150, 0);
      print_blobs_lighter (image_2, cam2_red, 150, 0, 0);
      if (vision_targets != NULL) {
        render_text (screen, arial, 100, 490, "Found target!");
        print_blobs (image, vision_targets, 0, 0, 0);
        free_blobs (vision_targets);
      }
    }
    if (do_text == 1) {
      render_text (screen, arial, 600, 308, "FPS: %d", (lastfps));
      //print_blobs (image_2, cam2_red, 0, 0, 0);
      render_text (screen, arial, 10, 308, "Hotkey list:");
      render_text (screen, arial, 20, 328, "i - increase left green blob minimum length: %d", tol->cam1_green.blob_minlength);
      render_text (screen, arial, 20, 348, "k - decrease left green blob minimum length");
      render_text (screen, arial, 20, 368, "o - increase left green check tolerance: %d", tol->cam1_green.blob_tolerance);
      render_text (screen, arial, 20, 388, "l - decrease left green check tolerance");
      render_text (screen, arial, 20, 408, "y - increase left red blob minimum length: %d", tol->cam1_red.blob_minlength);
      render_text (screen, arial, 20, 428, "h - decrease left red blob minimum length");
      render_text (screen, arial, 20, 448, "u - increase left red check tolerance: %d", tol->cam1_red.blob_tolerance);
      render_text (screen, arial, 20, 468, "j - decrease left red check tolerance");
      render_text (screen, arial, 50, 508, "Green check color: %d, %d, %d", tol->cam1_green.r, tol->cam1_green.g, tol->cam1_green.b);
      SPG_RectFilled (screen, 20, 500, 40, 520, SDL_MapRGB (screen->format, tol->cam1_green.r, tol->cam1_green.g, tol->cam1_green.b));
      render_text (screen, arial, 50, 548, "Red check color: %d, %d, %d", tol->cam1_red.r, tol->cam1_red.g, tol->cam1_red.b);
      SPG_RectFilled (screen, 20, 540, 40, 560, SDL_MapRGB (screen->format, tol->cam1_red.r, tol->cam1_red.g, tol->cam1_red.b));
      render_text (screen, arial, 320, 328, "w - increase right green blob minimum length: %d", tol->cam2_green.blob_minlength);
      render_text (screen, arial, 320, 348, "s - decrease right green blob minimum length");
      render_text (screen, arial, 320, 368, "e - increase right green check tolerance: %d", tol->cam2_green.blob_tolerance);
      render_text (screen, arial, 320, 388, "d - decrease right green check tolerance");
      render_text (screen, arial, 320, 408, "r - increase right red blob minimum length: %d", tol->cam2_red.blob_minlength);
      render_text (screen, arial, 320, 428, "f - decrease right red blob minimum length");
      render_text (screen, arial, 320, 448, "t - increase right red check tolerance: %d", tol->cam2_red.blob_tolerance);
      render_text (screen, arial, 320, 468, "g - decrease right red check tolerance");
      render_text (screen, arial, 350, 508, "Green check color: %d, %d, %d", tol->cam2_green.r, tol->cam2_green.g, tol->cam2_green.b);
      SPG_RectFilled (screen, 320, 500, 340, 520, SDL_MapRGB (screen->format, tol->cam2_green.r, tol->cam2_green.g, tol->cam2_green.b));
      render_text (screen, arial, 350, 548, "Red check color: %d, %d, %d", tol->cam2_red.r, tol->cam2_red.g, tol->cam2_red.b);
      SPG_RectFilled (screen, 320, 540, 340, 560, SDL_MapRGB (screen->format, tol->cam2_red.r, tol->cam2_red.g, tol->cam2_red.b));
      if ((cam1_green != NULL) && (cam2_green != NULL)) {
        a1 = image_angle (cam1_green->center_x);
        a2 = image_angle (cam2_green->center_x);
        render_text (screen, arial, 580, 348, "Image 1 centroid: %d, %d", cam1_green->center_x, cam1_green->center_y);
        render_text (screen, arial, 580, 368, "Image 2 centroid: %d, %d", cam2_green->center_x, cam2_green->center_y);
        render_text (screen, arial, 580, 388, "Depth, Method 1: %f", find_depth (a1, a2, 0));
        render_text (screen, arial, 580, 408, "Depth, Method 2: %f", find_depth (a1, a2, 1));
        render_text (screen, arial, 580, 428, "Depth, Method 3: %f", find_depth (a1, a2, 2));
        render_text (screen, arial, 580, 448, "Angle, Left: %f", a1 * (180/PI));
        render_text (screen, arial, 580, 468, "Angle, Right: %f", a2 * (180/PI));
        SPG_RectFilled (screen, 780, (int)(35 * find_depth (a1, a2, 2)) - 300, 800, 20 + (int)(35 * find_depth (a1, a2, 1)) - 300, SDL_MapRGB (screen->format, tol->cam2_green.r, tol->cam2_green.g, tol->cam2_green.b));
      }
    }
    if (do_blob == 1) {
      free_blobs (cam1_green);
      free_blobs (cam1_red);
      free_blobs (cam2_green);
      free_blobs (cam2_red);
    }
    easy_blit (0, 0, image, screen);
    easy_blit (352, 0, image_2, screen);
    SDL_FreeSurface (image);
    SDL_FreeSurface (image_2);
    SDL_Flip (screen);
    log_fp = fopen ("colors.tol", "wb");
    fwrite (&tol, sizeof (tolerance_file_t), 1, log_fp);
    fclose (log_fp);
  }
  return 0;
}
