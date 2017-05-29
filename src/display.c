#include "display.h"
#include "util.h"

#include <string.h>

void p3d_display_new(Display* d, int width, int height, int zoom, const char* title) {
	if (zoom < 1) { zoom = 1; }

	SDL_Init(SDL_INIT_VIDEO);
	d->width = width;
	d->height = height;
	d->screen = SDL_SetVideoMode(width, height, 32, SDL_HWSURFACE);

	SDL_WM_SetCaption(title, NULL);
	p3d_bitmap_new(&d->buffer, width / zoom, height / zoom);
}

static SDL_Surface* SDL_ResizeSurface(SDL_Surface* surf, int new_width, int new_height) {
	Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif
	SDL_Surface* ssurf = SDL_CreateRGBSurface(SDL_HWSURFACE, new_width, new_height, 32, rmask, gmask, bmask, amask);
	SDL_LockSurface(ssurf);
	Uint8* orig_pixels = (Uint8*) surf->pixels;
	Uint8* pixels = (Uint8*) ssurf->pixels;

	int x_ratio = (int)((surf->w << 16) / new_width) + 1;
	int y_ratio = (int)((surf->h << 16) / new_height) + 1;
	int x2, y2;
	for (int y = 0; y < new_height; y++) {
		for (int x = 0; x < new_width; x++) {
			x2 = ((x * x_ratio) >> 16);
			y2 = ((y * y_ratio) >> 16);
			int index1 = (x + y * new_width) * 4;
			int index2 = (x2 + y2 * surf->w) * 4;
			pixels[index1 + 0] = orig_pixels[index2 + 0];
			pixels[index1 + 1] = orig_pixels[index2 + 1];
			pixels[index1 + 2] = orig_pixels[index2 + 2];
			pixels[index1 + 3] = orig_pixels[index2 + 3];
		}
	}

	SDL_UnlockSurface(ssurf);
	return ssurf;
}

void p3d_display_swap_buffers(Display* d) {
	p3d_bitmap_invalidate(&d->buffer);

	SDL_Surface* rbuff = NULL;
	bool freeBuff = false;
	if (d->buffer.width < d->width || d->buffer.height < d->height) {
		rbuff = SDL_ResizeSurface(d->buffer.surface, d->width, d->height);
		freeBuff = true;
	} else {
		rbuff = d->buffer.surface;
	}
	SDL_BlitSurface(
				rbuff,
				NULL,
				d->screen,
				NULL
	);

	SDL_Flip(d->screen);
	if (freeBuff) {
		SDL_FreeSurface(rbuff);
	}
}

void p3d_display_free(Display* d) {
	SDL_FreeSurface(d->screen);
}
