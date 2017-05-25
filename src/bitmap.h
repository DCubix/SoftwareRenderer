#ifndef BITMAP_H
#define BITMAP_H

#include <SDL.h>

typedef struct p3dcolor {
	float r, g, b, a;
} Color;

static const Color P3D_COLOR_BLACK		= { 0, 0, 0, 1 };
static const Color P3D_COLOR_GREY		= { 0.5f, 0.5f, 0.5f, 1 };
static const Color P3D_COLOR_WHITE		= { 1, 1, 1, 1 };
static const Color P3D_COLOR_RED		= { 1, 0, 0, 1 };
static const Color P3D_COLOR_GREEN		= { 0, 1, 0, 1 };
static const Color P3D_COLOR_BLUE		= { 0, 0, 1, 1 };
static const Color P3D_COLOR_MAGENTA	= { 1, 0, 1, 1 };

typedef struct p3d_bitmap {
	int width, height;
	Uint8* pixels;
	SDL_Surface* surface;
} Bitmap;

void p3d_bitmap_new(Bitmap* bmp, int width, int height);
void p3d_bitmap_from_file(Bitmap* bmp, const char* fileName);
void p3d_bitmap_free(Bitmap* bmp);

void p3d_bitmap_clear(Bitmap* bmp, Color color);
void p3d_bitmap_set(Bitmap* bmp, int x, int y, Color color);
Color p3d_bitmap_get(Bitmap* bmp, int x, int y);

Color p3d_color_mul(Color a, Color b);

void p3d_bitmap_set_pixels(Bitmap* bmp, Uint8* pixels);

void p3d_bitmap_invalidate(Bitmap* bmp);

#endif // BITMAP_H
