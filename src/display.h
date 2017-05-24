#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL.h>
#include "bitmap.h"

typedef struct p3d_display {
	SDL_Surface* screen;
	Bitmap buffer;
	int width, height;
} Display;

void p3d_display_new(Display* d, int width, int height, int zoom, const char* title);
void p3d_display_free(Display* d);
void p3d_display_swap_buffers(Display* d);

#endif // DISPLAY_H
