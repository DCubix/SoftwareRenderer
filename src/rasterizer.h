#ifndef RASTERIZER_H
#define RASTERIZER_H

#include "array.h"
#include "bitmap.h"
#include "vecmath.h"
#include "vertex.h"
#include "util.h"

typedef struct p3d_rasterizer {
	Bitmap* buffer;
	float* zbuffer;
	Color color;
	mat4 viewport, projection, view;
	Bitmap* bound_texture;

	bool fog_enabled;
	float fog_density;
	Color fog_color;
} Rasterizer;

typedef struct p3d_model {
	Vertex* vertices;
	Uint32* indices;
	int num_vertices, num_indices;
} Model;

void p3d_rasty_model_new(Model* m, Vertex* v, Uint32* i, int nv, int ni);
void p3d_rasty_model_from_file(Model* m, const char* fileName);
void p3d_rasty_model_calc_normals(Model* m);
void p3d_rasty_model_free(Model* m);

void p3d_rasty_new(Rasterizer* r, Bitmap* buffer);
void p3d_rasty_free(Rasterizer* r);

void p3d_rasty_clear(Rasterizer* r, Color color);

void p3d_rasty_bind_texture(Rasterizer* r, Bitmap* texture);

void p3d_rasty_set_color(Rasterizer* r, Color col);
void p3d_rasty_set_fog_enabled(Rasterizer* r, bool enabled);
void p3d_rasty_set_fog_density(Rasterizer* r, float density);
void p3d_rasty_set_fog_color(Rasterizer* r, Color color);

bool p3d_clip_poly_axis(Array* vertices, Array* aux, int comp);
void p3d_clip_poly_component(Array* vertices, int comp, float fac, Array* result);

void p3d_rasty_line(Rasterizer* r, int x0, int y0, int x1, int y1);
void p3d_rasty_line_3d(Rasterizer* r, vec3 p0, vec3 p1);
void p3d_rasty_triangle(Rasterizer* r, Vertex p1, Vertex p2, Vertex p3);
void p3d_rasty_clipped_triangle(Rasterizer* r, Vertex p1, Vertex p2, Vertex p3);
void p3d_rasty_model(Rasterizer* r, Model model, mat4 transform);
void p3d_rasty_billboard(Rasterizer* r, vec3 pos);
void p3d_rasty_sprite(Rasterizer* r, int srcx, int srcy, int srcw, int srch, int dstx, int dsty);

void p3d_rasty_set_view_matrix(Rasterizer* r, mat4 vm);
void p3d_rasty_set_projection_matrix(Rasterizer* r, mat4 p);

vec3 p3d_barycentric(vec3 a, vec3 b, vec3 c, vec2 p);

#endif // RASTERIZER_H
