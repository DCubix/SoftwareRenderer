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

	int zsize = buffer->width * buffer->height;
	r->zbuffer = (float*) malloc(zsize * sizeof(float));
	for (int i = 0; i < zsize; i++) {
		r->zbuffer[i] = -FLT_MAX;
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

void p3d_rasty_triangle(Rasterizer* r, vec4 p1, vec4 p2, vec4 p3) {
	vec2 bboxmin = { FLT_MAX, FLT_MAX };
	vec2 bboxmax = { -FLT_MAX, -FLT_MAX };
	vec2 clamp = { r->buffer->width-1, r->buffer->height-1 };

	vec4 pts[3] = { // Viewport transform
		mat4_mul_v4(r->viewport, p1),
		mat4_mul_v4(r->viewport, p2),
		mat4_mul_v4(r->viewport, p3)
	};

	vec2 pts2[3];
	for (int i = 0; i < 3; i++) {
		pts2[i].x = pts[i].x / pts[i].w;
		pts2[i].y = pts[i].y / pts[i].w;
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
			vec3 bc_clip = {
				bc_screen.x / pts[0].w,
				bc_screen.y / pts[1].w,
				bc_screen.z / pts[2].w
			};
			int index = (P.x + P.y * r->buffer->width);

			bc_clip = vec3_div_s(bc_clip, (bc_clip.x + bc_clip.y + bc_clip.z));
			float fragDepth = vec3_dot(vec3_from_vec4(p3), bc_clip);

			if (bc_screen.x < 0.0f ||
				bc_screen.y < 0.0f ||
				bc_screen.z < 0.0f ||
				r->zbuffer[index] > fragDepth)
			{
				continue;
			}

			r->zbuffer[index] = fragDepth;
			p3d_bitmap_set(r->buffer, (int) P.x, (int) P.y, r->color);
		}
	}
}

vec3 p3d_barycentric(vec2 A, vec2 B, vec2 C, Point p) {
	vec3 s[2];

	for (int i = 2; i >= 0; i--) {
		s[i].v[0] = C.v[i] - A.v[i];
		s[i].v[1] = B.v[i] - A.v[i];
		s[i].v[2] = A.v[i] - p.v[i];
	}

	vec3 u = vec3_cross(s[0], s[1]);

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
		r->zbuffer[i] = -FLT_MAX;
	}
}
