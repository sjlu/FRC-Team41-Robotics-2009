#define DEBUG_WIN

// #define DISABLE_IMG

// Russ's Axis206 code.  written  January 2008
// For Team 41, First Robotics Competition

// Includes:
//   winsock.h for sockets to get image from axis camera
//   stdlib.h, stdio.h, string.h for ansi C functions

// GPIOs:
// Wire# Color   GPIO#
// 1     White   58 
// 2     Grey    59
// 3     Purple  60
// 4     Blue    61
// 5     Green   62
// 6     Yellow  63
// 7     Orange  64
// 8     Red     65

#define GPIO_ON  "out set"
#define GPIO_OFF "out clear"

#ifndef DEBUG_WIN
#include <sys/socket.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include <gd.h>

#ifndef DEBUG_WIN
#include "axis206.h"
#define FONTPATH "/usr/share/fonts/arial.ttf"
#define LOGPATH "/media/card/img/blob.log"
#define TOLPATH "/media/card/img/colors.tol"
#define OUTPATH "/media/card/img/"
#endif // !DEBUG_WIN

#ifdef DEBUG_WIN
#define FONTPATH "arial.ttf"
#define LOGPATH "blob.log"
#define TOLPATH "colors.tol"
#define OUTPATH ""
#endif //DEBUG_WIN

#include "blob_gd.h"
#include "depth.h"
#include "log.h"
#include "target.h"
#include "tolerance.h"

#define CAM1 "41.0.0.2"
#define CAM2 "41.0.0.3"

void
gpio (int gpio_num, char * set_string)
{
  static char fn_buff[128];
  snprintf (fn_buff, 127, "echo \"GPIO %s\" >> /proc/gpio/GPIO%d", set_string, gpio_num);
  system (fn_buff);
  log_write ("Running cmd: %s", fn_buff);
}

void
usage ()
{
  printf ("usage: blob b|r OUTPUT_PREFIX\n");
}

int
render_text (gdImagePtr output, int col, int x, int y, char * fmt, ...)
{
  static char buffer[2048];
  va_list va_args;
  memset (buffer, '\0', 2048);
  va_start (va_args, fmt);
  vsnprintf (buffer, sizeof (buffer), fmt, va_args);
  va_end (va_args);
  log_write (buffer);
  gdImageStringFT (output, NULL, col, FONTPATH, 10, 0, x, y, buffer);
  return 0;
}

int
main (int argc, char ** argv)
{
  if (argc != 3) {
    usage ();
  }
  #ifdef DEBUG_WIN_X
  if (fork() != 0) {
    printf ("Forked to background, exiting.\n");
    exit (0);
  }
  #endif
  log_open (LOGPATH);
  int fill_color = gdTrueColorAlpha (100, 100, 100, gdAlphaTransparent / 2);
  int line_color = gdTrueColorAlpha (175, 175, 175, gdAlphaOpaque);
  int text_color = gdTrueColorAlpha (255, 255, 255, gdAlphaOpaque);
  FILE * img_out;
  int img_time;
  char img_filename[64];
  gdImagePtr camera_1;
  gdImagePtr camera_2;
  gdImagePtr output;
  int numloops = 0;
  int fpstimer;
  double a1, a2;
  tolerance_file_t * tol;
  int alliance;
  tol = get_tolerance (TOLPATH);
  log_write ("Got tolerance values from %s.", TOLPATH);
  if (argv[1][0] == 98) {
    log_write ("Seeking blue targets.");
    alliance = ALLIANCE_BLUE;
  } else {
    log_write ("Seeking red targets.");
    alliance = ALLIANCE_RED;
  }
  char * jpeg_buff;
  char * jpeg_buff_2;
  int jpeg_buff_size;
  int jpeg_buff_size_2;
  int x, y;
  blob * cam1_green;
  blob * cam2_green;
  blob * cam1_red;
  blob * cam2_red;
  blob * target_blob;
  int loop_ctr;
  loop_ctr = 0;
  float lastfps;
  log_write ("Beginning main loop.");
  while (1) {
    #ifndef DISABLE_IMG
    output = gdImageCreateTrueColor (1280, 480);
    #endif

    #ifndef DEBUG_WIN
    
    log_write ("Getting first jpeg.");
    jpeg_buff = get_jpeg (CAM1, &jpeg_buff_size);
    if (jpeg_buff == NULL) {
      log_write ("First jpeg buffer is null!");
      exit (1);
    }
    if (jpeg_buff_size <= 0) {
      log_write ("First jpeg buffer size is invalid!");
      exit (1);
    }
    log_write ("First jpeg received. Size: %d", jpeg_buff_size);
    camera_1 = gdImageCreateFromJpegPtr (jpeg_buff_size, jpeg_buff);
    if (camera_1 == NULL) {
      log_write ("gdImageCreateFromJpegPtr () failed for jpeg 1!");
      exit (1);
    }
    log_write ("Getting second jpeg.");
    jpeg_buff_2 = get_jpeg (CAM2, &jpeg_buff_size_2);
    if (jpeg_buff_2 == NULL) {
      log_write ("Second jpeg buffer is null!");
      exit (1);
    }
    if (jpeg_buff_size_2 <= 0) {
      log_write ("Second jpeg buffer size is invalid!");
      exit (1);
    }
    log_write ("Second jpeg received. Size: %d", jpeg_buff_size_2);
    camera_2 = gdImageCreateFromJpegPtr (jpeg_buff_size_2, jpeg_buff_2);
    if (camera_2 == NULL) {
      log_write ("gdImageCreateFromJpegPtr () failed for jpeg 2!");
      exit (1);
    }
    
    #endif // !DEBUG_WIN
    
    #ifdef DEBUG_WIN
    
    img_out = fopen ("cam1.jpg", "rb");
    camera_1 = gdImageCreateFromJpeg (img_out);
    fclose (img_out);
    img_out = fopen ("cam2.jpg", "rb");
    camera_2 = gdImageCreateFromJpeg (img_out);
    fclose (img_out);
    if (camera_1 == NULL) {
      log_write ("Camera 1 image did not load properly.");
      exit (1);
    }
    if (camera_2 == NULL) {
      log_write ("Camera 2 image did not load properly.");
      exit (1);
    }
    
    #endif // DEBUG_WIN
    
    log_write ("Detecting blobs on camera 1.");
    cam1_red = find (camera_1, &(tol->cam1_red));
    cam1_green = find (camera_1, &(tol->cam1_green));
    log_write ("Detecting blobs on camera 2.");
    cam2_red = find (camera_2, &(tol->cam2_red));
    cam2_green = find (camera_2, &(tol->cam2_green));
    if ((target_blob = target (cam1_red, cam1_green, alliance)) != NULL) {
      if ((target_blob->center_x + 50) > 400) {
        log_write ("Sending left motor command.");
        gpio (59, GPIO_ON);
        gpio (58, GPIO_OFF);
      }
      else if ((target_blob->center_x + 50) < 240) {
        log_write ("Sending right motor command.");
        gpio (58, GPIO_ON);
        gpio (59, GPIO_OFF);
      }
      else {
        log_write ("Sending left & right motor commands.");
        gpio (58, GPIO_ON);
        gpio (59, GPIO_ON);
      }
      #ifndef DISABLE_IMG
      //print_blobs (camera_1, target_blob, 100, 100, 100);
      #endif
      free_blobs (target_blob);
    } else {
      log_write ("EE: No targets found.");
    }
    if (cam1_red != NULL && cam1_green != NULL) {
      log_write ("Blob detection completed.  Building image.");
      #ifndef DISABLE_IMG
      print_blobs (camera_1, cam1_red, 200, 0, 0);
      //print_blobs (camera_2, cam2_red, 200, 0, 0);
      print_blobs (camera_1, cam1_green, 0, 180, 20);
      //print_blobs (camera_2, cam2_green, 0, 180, 20);
      #endif
      free_blobs (cam1_red);
      free_blobs (cam2_red);
      free_blobs (cam1_green);
      free_blobs (cam2_green);
      #ifndef DISABLE_IMG
      gdImageCopy (output, camera_1, 0, 0, 0, 0, 640, 480);
      gdImageCopy (output, camera_2, 640, 0, 0, 0, 640, 480);
      #endif
      /*gdImageFilledRectangle (output, 540, 340, 760, 450, fill_color);
      gdImageRectangle (output, 540, 340, 760, 450, line_color);
      time_t rawtime;
      time (&rawtime);
      char * time_str = ctime (&rawtime);
      time_str[strlen(time_str)-1] = '\0';
      render_text (output, text_color, 550, 360, "%s", time_str);
      if (cam1_red != NULL && cam2_red != NULL) {
        render_text (output, text_color, 550, 375, "Left Centroid: 300, 424", cam1_red->center_x, cam1_red->center_y);
        render_text (output, text_color, 550, 390, "Right Centroid: 422, 233", cam2_red->center_x, cam2_red->center_y);
        a1 = image_angle (cam1_red->center_x);
        a2 = image_angle (cam2_red->center_x);
        render_text (output, text_color, 550, 405, "Left Angle: %f", a1 * (180/PI));
        render_text (output, text_color, 550, 420, "Right Angle: %f", a2 * (180/PI));
        render_text (output, text_color, 550, 435, "Depth: %f", find_depth (a1, a2));
      }*/
      #ifndef DISABLE_IMG
      img_time = time (NULL);
      snprintf (img_filename, sizeof (img_filename), "%s%s%d.jpg", OUTPATH, argv[2], img_time);
      log_write ("Image built. Writing to file: %s", img_filename);
      img_out = fopen (img_filename, "wb");
      gdImageJpeg (output, img_out, 100);
      fclose (img_out);
      #endif
    } else {
      log_write ("EE: Some cams didn't detect red+green blobs.");
    }
    #ifndef DISABLE_IMG
    gdImageDestroy (output);
    #endif
    gdImageDestroy (camera_1);
    gdImageDestroy (camera_2);
    log_write ("Loop %d finished.", loop_ctr);
    loop_ctr++;
  }
  return 0;
}
