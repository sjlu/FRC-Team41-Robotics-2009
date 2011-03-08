#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include "net.h"
#include "log.h"
#include "draw.h"

struct {
  SDL_Surface * dest;
  SDL_Surface * back;
  TTF_Font * arial_20;
  packet_t * packet;
} draw_global;

void
draw_packetnum ()
{
  render_text (draw_global.dest, draw_global.arial_20, 255, 255, 255, 10, 10, "Packet #: %d", draw_global.packet->num);
}

void
draw_back ()
{
  easy_blit (0, 0, draw_global.back, draw_global.dest);
}

void
draw_battery ()
{
  render_text (draw_global.dest, draw_global.arial_20, 255, 255, 255, 10, 40, "Battery: %f", draw_global.packet->battery);  
}

void
draw_current ()
{
  int i;
  for (i = 0; i <= 3; i++)
    render_text (draw_global.dest, draw_global.arial_20, 255, 255, 255, 10, 70 + (i * 30), "Speed %d: %f", i, draw_global.packet->speed[i]);
}

void
draw_pwm ()
{
  int i;
  for (i = 0; i <= 3; i++)
    render_text (draw_global.dest, draw_global.arial_20, 255, 255, 255, 10, 190 + (i * 30), "Current %d: %f", i, draw_global.packet->current[i]);
}

void
draw ()
{
  draw_back ();
  draw_packetnum (); 
  draw_battery ();
  draw_current ();
  draw_pwm ();
}

void
easy_blit (int x, int y, SDL_Surface * src, SDL_Surface * dest)
{
  SDL_Rect off;
  off.x = x;
  off.y = y;
  SDL_BlitSurface (src, NULL, dest, &off);
}

int
render_text (SDL_Surface * dest, TTF_Font * font, int r, int g, int b, int x, int y, char * fmt, ...)
{
  static char buffer[2048];
  va_list va_args;
  memset (buffer, '\0', 2048);
  va_start (va_args, fmt);
  vsnprintf (buffer, sizeof (buffer), fmt, va_args);
  va_end (va_args);
  printf ("hi");
  SDL_Color white = {r, g, b};
  SDL_Surface * temp = TTF_RenderText_Blended (font, buffer, white);
  if (temp != NULL) {
    easy_blit (x, y, temp, dest);
    SDL_FreeSurface (temp);
    return 0;
  }
  log_write ("EE: Text render failed: %s or %s", TTF_GetError(), SDL_GetError());
  return 1;
}

void
draw_init (SDL_Surface * dest, packet_t * packet)
{
  draw_global.dest = dest;
  draw_global.packet = packet;
  if ((draw_global.arial_20 = TTF_OpenFont ("arial.ttf", 20)) == NULL ) {
    SDL_Quit ();
    TTF_Quit ();
    log_write ("EE: Could not load font: %s", TTF_GetError());
    exit (1);
  }
  draw_global.back = IMG_Load ("back.png");
}

void
draw_quit ()
{
  TTF_CloseFont (draw_global.arial_20);  
}
