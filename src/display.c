#include "display.h"
#include "util.h"

#include <string.h>
#include <SDL_rotozoom.h>

void p3d_display_new(Display* d, int width, int height, int zoom, const char* title) {
	if (zoom < 1) { zoom = 1; }

	SDL_Init(SDL_INIT_VIDEO);
	d->width = width;
	d->height = height;
	d->screen = SDL_SetVideoMode(width, height, 32, SDL_HWSURFACE);

	SDL_WM_SetCaption(title, NULL);
	p3d_bitmap_new(&d->buffer, width / zoom, height / zoom);
}

void p3d_display_swap_buffers(Display* d) {
	p3d_bitmap_invalidate(&d->buffer);

	double zx = (double)d->width / (double)d->buffer.width;
	double zy = (double)d->height / (double)d->buffer.height;
	SDL_Surface* zommed_buff = zoomSurface(d->buffer.surface, zx, zy, 0);
	SDL_BlitSurface(
				zommed_buff,
				NULL,
				d->screen,
				NULL
	);

	SDL_Flip(d->screen);
	SDL_FreeSurface(zommed_buff);
}

void p3d_display_free(Display* d) {
	SDL_FreeSurface(d->screen);
}
