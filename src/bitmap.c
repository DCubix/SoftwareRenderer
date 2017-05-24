#include "bitmap.h"

#include <string.h>
#include "util.h"

void p3d_bitmap_new(Bitmap* bmp, int width, int height) {
	bmp->width = width;
	bmp->height = height;
	bmp->pixels = (Uint8*) calloc(width * height * 4, sizeof(Uint8));

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

	bmp->surface = SDL_CreateRGBSurface(SDL_HWSURFACE, width, height, 32, rmask, gmask, bmask, amask);
}

void p3d_bitmap_free(Bitmap* bmp) {
	free(bmp->pixels);
	bmp->pixels = NULL;
	SDL_FreeSurface(bmp->surface);
	bmp->surface = NULL;
}

void p3d_bitmap_invalidate(Bitmap* bmp) {
	SDL_LockSurface(bmp->surface);

	size_t n = bmp->width * bmp->height * 4;
	Uint8* bmp_pixels = (Uint8*) malloc(n * sizeof(Uint8));
	for (int y = 0; y < bmp->height; y++) {
		for (int x = 0; x < bmp->width; x++) {
			size_t i = (x + y * bmp->width) * 4;
			bmp_pixels[i + 0] = bmp->pixels[i + 0]; // b
			bmp_pixels[i + 1] = bmp->pixels[i + 1]; // g
			bmp_pixels[i + 2] = bmp->pixels[i + 2]; // r
			bmp_pixels[i + 3] = bmp->pixels[i + 3]; // a
		}
	}

	Uint8* pixels = (Uint8*) bmp->surface->pixels;
	SDL_memcpy(pixels, bmp_pixels, n);

	SDL_UnlockSurface(bmp->surface);

	free(bmp_pixels);
}

void p3d_bitmap_set(Bitmap* bmp, int x, int y, Color color) {
	if (x < 0 || x >= bmp->width || y < 0 || y >= bmp->height) {
		return;
	}

	Uint8 r = (Uint8) (color.r * 255);
	Uint8 g = (Uint8) (color.g * 255);
	Uint8 b = (Uint8) (color.b * 255);
	Uint8 a = (Uint8) (color.a * 255);
	Uint32 index = (x + bmp->width * y) * 4;

	bmp->pixels[index + 0] = r;
	bmp->pixels[index + 1] = g;
	bmp->pixels[index + 2] = b;
	bmp->pixels[index + 3] = a;
}

Color p3d_bitmap_get(Bitmap* bmp, int x, int y) {
	if (x < 0 || x >= bmp->width || y < 0 || y >= bmp->height) {
		x = 0; y = 0;
	}
	Uint32 index = (x + bmp->width * y) * 4;
	Uint8 r = bmp->pixels[index + 0];
	Uint8 g = bmp->pixels[index + 1];
	Uint8 b = bmp->pixels[index + 2];
	Uint8 a = bmp->pixels[index + 3];
	Color ret;
	ret.r = (float) r / 255.0f;
	ret.g = (float) g / 255.0f;
	ret.b = (float) b / 255.0f;
	ret.a = (float) a / 255.0f;
	return ret;
}

void p3d_bitmap_clear(Bitmap* bmp, Color color) {
	for (int y = 0; y < bmp->height; y++) {
		for (int x = 0; x < bmp->width; x++) {
			p3d_bitmap_set(bmp, x, y, color);
		}
	}
}
