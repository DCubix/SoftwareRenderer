#include "bitmap.h"

#include <string.h>
#include "util.h"
#include "stb.h"
#include "vecmath.h"

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

void p3d_bitmap_from_file(Bitmap* bmp, const char* fileName) {
	int w, h, comp;

	Uint8* pixels = stbi_load(fileName, &w, &h, &comp, STBI_rgb_alpha);
	if (pixels) {
		Uint8* pixel_data = (Uint8*) malloc(w * h * 4 * sizeof(Uint8));
		for (int i = 0; i < w * h * 4; i += 4) {
			pixel_data[i + 0] = pixels[i + 0];
			pixel_data[i + 1] = pixels[i + 1];
			pixel_data[i + 2] = pixels[i + 2];
			if (w * h * comp == w * h * 4) {
				pixel_data[i + 3] = pixels[i + 3];
			} else {
				pixel_data[i + 3] = (Uint8) 255;
			}
		}

		bmp->width = w;
		bmp->height = h;
		bmp->pixels = pixel_data;

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
		bmp->surface = SDL_CreateRGBSurface(SDL_HWSURFACE, w, h, 32, rmask, gmask, bmask, amask);

		p3d_bitmap_invalidate(bmp);

		STBI_FREE(pixels);
	}
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
	Uint8* pixels = (Uint8*) bmp->surface->pixels;

	memcpy(pixels, bmp->pixels, n);

	SDL_UnlockSurface(bmp->surface);

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

void p3d_bitmap_set_pixels(Bitmap* bmp, Uint8* pixels) {
	memcpy(bmp->pixels, pixels, bmp->width * bmp->height * 4 * sizeof(Uint8));
	free(pixels);
}

Color p3d_color_mul(Color a, Color b) {
	Color r;
	r.r = min(max(a.r * b.r, 0.0f), 1.0f);
	r.g = min(max(a.g * b.g, 0.0f), 1.0f);
	r.b = min(max(a.b * b.b, 0.0f), 1.0f);
	r.a = min(max(a.a * b.a, 0.0f), 1.0f);
	return r;
}

Color p3d_color_blend(Color a, Color b, float fac) {
	Color r;
	r.r = (a.r * (1.0f - fac)) + (b.r * fac);
	r.g = (a.g * (1.0f - fac)) + (b.g * fac);
	r.b = (a.b * (1.0f - fac)) + (b.b * fac);
	r.a = b.a;
	return r;
}
