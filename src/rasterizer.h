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
	mat4 viewport, projection, view;
	Bitmap* bound_texture;
} Rasterizer;

typedef struct p3d_model {
	Vertex* vertices;
	Uint32* indices;
	int num_vertices, num_indices;
} Model;

void p3d_rasty_model_new(Model* m, Vertex* v, Uint32* i, int nv, int ni);
void p3d_rasty_model_from_file(Model* m, const char* fileName);

void p3d_rasty_new(Rasterizer* r, Bitmap* buffer);
void p3d_rasty_free(Rasterizer* r);

void p3d_rasty_clear(Rasterizer* r, Color color);

void p3d_rasty_bind_texture(Rasterizer* r, Bitmap* texture);

void p3d_rasty_set_color(Rasterizer* r, Color col);

void p3d_rasty_line(Rasterizer* r, int x0, int y0, int x1, int y1);
void p3d_rasty_triangle(Rasterizer* r, Vertex p1, Vertex p2, Vertex p3);
void p3d_rasty_model(Rasterizer* r, Model model, mat4 transform);
void p3d_rasty_billboard(Rasterizer* r, vec3 pos);

void p3d_rasty_set_view_matrix(Rasterizer* r, mat4 vm);
void p3d_rasty_set_projection_matrix(Rasterizer* r, mat4 p);

vec3 p3d_barycentric(vec3 a, vec3 b, vec3 c, Point p);

#endif // RASTERIZER_H
