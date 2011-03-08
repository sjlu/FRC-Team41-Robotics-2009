#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include "net.h"
#include "log.h"
#include "draw.h"

int
main (int argc, char ** argv)
{
  int loop = 1;
  SDL_Event event;
  SDL_Surface * screen;
  packet_t * packet;
  log_open ("dashboard.log");
  if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_TIMER) == -1) {
    SDL_Quit ();
    log_write ("EE: SDL initialization failed: %s", SDL_GetError());
    exit (1);
  }
  if ((screen = SDL_SetVideoMode (1024, 768, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN)) == NULL ) {
    SDL_Quit ();
    log_write ("EE: Could not create output surface: %s", SDL_GetError());
    exit (1);
  }
  if (TTF_Init () == -1) {
    SDL_Quit ();
    log_write ("EE: TTF initialization failed: %s", TTF_GetError());
    exit (1);
  }
  net_init (&packet);
  draw_init (screen, packet);
  while (loop) {
    while (SDL_PollEvent (&event)) {
      switch (event.type) {
        case SDL_KEYDOWN:
          switch (event.key.keysym.sym) {
            case 'q':
              loop = 0;
            break;
          }
        break;
      }
    }
    net_receive ();
    draw ();
    SDL_Flip (screen);
  }
}
