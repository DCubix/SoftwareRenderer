#ifndef RASTERIZER_H
#define RASTERIZER_H

#include "bitmap.h"
#include "vecmath.h"
#include "vertex.h"

#define MAX_TEXTURES 32

typedef struct p3d_point {
	union {
		int v[3];
		struct { int x, y, z; };
	};
} Point;

typedef struct p3d_rasterizer {
	Bitmap* buffer;
	float* zbuffer;

	Color color;

	mat4 viewport;

	int bound_texture, texture_count;
	Bitmap* textures[MAX_TEXTURES];
} Rasterizer;

void p3d_rasty_new(Rasterizer* r, Bitmap* buffer);
void p3d_rasty_free(Rasterizer* r);

void p3d_rasty_clear(Rasterizer* r, Color color);

Uint32 p3d_rasty_create_texture(Rasterizer* r, int width, int height, Uint8* pixels);
void p3d_rasty_bind_texture(Rasterizer* r, int slot);

void p3d_rasty_set_color(Rasterizer* r, Color col);

void p3d_rasty_line(Rasterizer* r, int x0, int y0, int x1, int y1);
void p3d_rasty_triangle(Rasterizer* r, Vertex p1, Vertex p2, Vertex p3);

vec3 p3d_barycentric(vec3 a, vec3 b, vec3 c, Point p);

#endif // RASTERIZER_H
