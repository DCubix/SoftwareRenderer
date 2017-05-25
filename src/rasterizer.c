#include "rasterizer.h"

#include "util.h"
#include <math.h>

#include <stdio.h>
#include <string.h>
#include <float.h>

void p3d_rasty_new(Rasterizer* r, Bitmap* buffer) {
	r->buffer = buffer;
	r->color.r = 1;
	r->color.g = 1;
	r->color.b = 1;
	r->color.a = 1;
	r->bound_texture = -1;
	r->texture_count = 0;

	for (int i = 0; i < MAX_TEXTURES; i++) {
		r->textures[i] = NULL;
	}

	int zsize = buffer->width * buffer->height;
	r->zbuffer = (float*) malloc(zsize * sizeof(float));
	for (int i = 0; i < zsize; i++) {
		r->zbuffer[i] = 0;
	}

	mat4_viewport(&r->viewport, 0, 0, buffer->width, buffer->height);
}

void p3d_rasty_line(Rasterizer* r, int x0, int y0, int x1, int y1) {
	bool steep = false;

	if (abs(x0 - x1) < abs(y0 - y1)) {
		swap(int, x0, y0);
		swap(int, x1, y1);
		steep = true;
	}
	if (x0 > x1) {
		swap(int, x0, x1);
		swap(int, y0, y1);
	}

	int dx = x1 - x0;
	int dy = y1 - y0;
	int derror2 = abs(dy) * 2;
	int error2 = 0;
	int y = y0;
	for (int x = x0; x <= x1; x++) {
		if (steep) {
			p3d_bitmap_set(r->buffer, y, x, r->color);
		} else {
			p3d_bitmap_set(r->buffer, x, y, r->color);
		}
		error2 += derror2;
		if (error2 > dx) {
			y += y1 > y0 ? 1 : -1;
			error2 -= dx * 2;
		}
	}
}

void p3d_rasty_triangle(Rasterizer* r, Vertex p1, Vertex p2, Vertex p3) {
	vec2 bboxmin = { FLT_MAX, FLT_MAX };
	vec2 bboxmax = { -FLT_MAX, -FLT_MAX };
	vec2 clamp = { r->buffer->width-1, r->buffer->height-1 };

	Vertex pts[3] = { // Viewport transform
		p3d_vertex_transform(p1, r->viewport),
		p3d_vertex_transform(p2, r->viewport),
		p3d_vertex_transform(p3, r->viewport),
	};

	vec3 pts2[3];
	for (int i = 0; i < 3; i++) {
		pts2[i].x = pts[i].position.x / pts[i].position.w;
		pts2[i].y = pts[i].position.y / pts[i].position.w;
		pts2[i].z = pts[i].position.z / pts[i].position.w;
	}

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			bboxmin.v[j] = max(0.0f, min(bboxmin.v[j], pts2[i].v[j]));
			bboxmax.v[j] = min(clamp.v[j], max(bboxmax.v[j], pts2[i].v[j]));
		}
	}

	Point P = { 0, 0, 0 };
	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
			vec3 bc_screen = p3d_barycentric(pts2[0], pts2[1], pts2[2], P);
			if (bc_screen.x < 0.0f || bc_screen.y < 0.0f ||	bc_screen.z < 0.0f) { continue; }
			vec3 bc_clip = {
				bc_screen.x / pts[0].position.w,
				bc_screen.y / pts[1].position.w,
				bc_screen.z / pts[2].position.w
			};

			int index = P.x + P.y * r->buffer->width;
			float depth = bc_clip.x + bc_clip.y + bc_clip.z;
			depth = 1.0f - depth;

			if (r->zbuffer[index] < depth) {
				r->zbuffer[index] = depth;
				Bitmap* tex = NULL;
				if (r->bound_texture > -1) {
					tex = r->textures[r->bound_texture];
				}
				if (tex) {
					float u = (p1.uv.x * bc_clip.x + p2.uv.x * bc_clip.y + p3.uv.x * bc_clip.z) /
							  (bc_clip.x + bc_clip.y + bc_clip.z);
					float v = (p1.uv.y * bc_clip.x + p2.uv.y * bc_clip.y + p3.uv.y * bc_clip.z) /
							  (bc_clip.x + bc_clip.y + bc_clip.z);
					float tx = (int)(u * tex->width);
					float ty = (int)(v * tex->height);

					Color col = p3d_color_mul(r->color, p3d_bitmap_get(tex, tx, ty));
					if (col.a > 0) {
						p3d_bitmap_set(r->buffer, P.x, P.y, col);
					}
				} else {
					p3d_bitmap_set(r->buffer, P.x, P.y, r->color);
				}
			}
		}
	}
}

vec3 p3d_barycentric(vec3 a, vec3 b, vec3 c, Point p) {
	vec3 s0 = ctor(vec3, c.x - a.x, b.x - a.x, a.x - p.x);
	vec3 s1 = ctor(vec3, c.y - a.y, b.y - a.y, a.y - p.y);

	vec3 u = vec3_cross(s0, s1);

	vec3 r = { -1, 1, 1 };
	if (abs(u.z) > 1e-2) {
		r.x = 1.0f - (u.x + u.y) / u.z;
		r.y = u.y / u.z;
		r.z = u.x / u.z;
		return r;
	}
	return r;
}

void p3d_rasty_clear(Rasterizer* r, Color color) {
	p3d_bitmap_clear(r->buffer, color);
	for (int i = 0; i < r->buffer->width * r->buffer->height; i++) {
		r->zbuffer[i] = 0;
	}
}

void p3d_rasty_free(Rasterizer* r) {
	for (int i = 0; i < MAX_TEXTURES; i++) {
		if (r->textures[i]) {
			p3d_bitmap_free(r->textures[i]);
			r->textures[i] = NULL;
		}
	}
}

Uint32 p3d_rasty_create_texture(Rasterizer* r, int width, int height, Uint8* pixels) {
	Uint32 tex = r->texture_count;
	Bitmap* bmp = (Bitmap*) malloc(sizeof(Bitmap));
	p3d_bitmap_new(bmp, width, height);
	p3d_bitmap_set_pixels(bmp, pixels);
	r->textures[tex % MAX_TEXTURES] = bmp;
	r->texture_count++;
	return tex;
}

void p3d_rasty_bind_texture(Rasterizer* r, int slot) {
	if (r->textures[slot] == NULL) {
		return;
	}
	r->bound_texture = slot;
}

void p3d_rasty_set_color(Rasterizer* r, Color col) {
	r->color = col;
}
