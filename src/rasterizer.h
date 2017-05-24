#ifndef RASTERIZER_H
#define RASTERIZER_H

#include "bitmap.h"
#include "vecmath.h"

typedef struct p3d_point {
	union {
		int v[3];
		struct { int x, y, z; };
	};
} Point;

typedef struct p3d_rasterizer {
	Bitmap* buffer;
	Color color;
	float* zbuffer;
	mat4 viewport;
} Rasterizer;

void p3d_rasty_new(Rasterizer* r, Bitmap* buffer);

void p3d_rasty_clear(Rasterizer* r, Color color);

void p3d_rasty_line(Rasterizer* r, int x0, int y0, int x1, int y1);
void p3d_rasty_triangle(Rasterizer* r, vec4 p1, vec4 p2, vec4 p3);

vec3 p3d_barycentric(vec2 A, vec2 B, vec2 C, Point p);

#endif // RASTERIZER_H
