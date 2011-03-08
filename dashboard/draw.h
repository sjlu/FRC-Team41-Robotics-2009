void draw_back ();

void draw_battery ();

void draw ();

void easy_blit (int x, int y, SDL_Surface * src, SDL_Surface * dest);

int render_text (SDL_Surface * dest, TTF_Font * font, int r, int g, int b, int x, int y, char * fmt, ...);

void draw_init (SDL_Surface * dest, packet_t * packet);

void draw_quit ();
