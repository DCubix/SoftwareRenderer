#include <stdio.h>

#include "vecmath.h"
#include "display.h"
#include "rasterizer.h"
#include "util.h"
#include <math.h>

#define N 1.0f

int main(int argc, char *argv[]) {
	Display d;
	p3d_display_new(&d, 640, 480, 4, "Test");

	Rasterizer ras;
	p3d_rasty_new(&ras, &d.buffer);

	Bitmap* buff = &d.buffer;
	Bitmap tex;
	p3d_bitmap_new(&tex, 4, 4);

	for (int i = 0; i < 4; i++) {
		bool k = i%2==0;
		p3d_bitmap_set(&tex, 0, i, k ? P3D_COLOR_GREY : P3D_COLOR_WHITE);
		p3d_bitmap_set(&tex, 1, i, k ? P3D_COLOR_WHITE : P3D_COLOR_GREY);
		p3d_bitmap_set(&tex, 2, i, k ? P3D_COLOR_GREY : P3D_COLOR_WHITE);
		p3d_bitmap_set(&tex, 3, i, k ? P3D_COLOR_WHITE : P3D_COLOR_GREY);
	}

	Uint32 tex_id = p3d_rasty_create_texture(&ras, tex.width, tex.height, tex.pixels);

	Vertex cube[8] = {
		{ {-N, -N, -N, 1}, { 0.0f, 0.0f } },
		{ { N, -N, -N, 1}, { 1.0f, 0.0f } },
		{ { N,  N, -N, 1}, { 1.0f, 1.0f } },
		{ {-N,  N, -N, 1}, { 0.0f, 1.0f } },
		{ {-N, -N,  N, 1}, { 0.0f, 0.0f } },
		{ { N, -N,  N, 1}, { 1.0f, 0.0f } },
		{ { N,  N,  N, 1}, { 1.0f, 1.0f } },
		{ {-N,  N,  N, 1}, { 0.0f, 1.0f } }
	};
	int faces[12][3] = {
		{ 0, 1, 2 },
		{ 2, 3, 0 },
		{ 1, 5, 6 },
		{ 6, 2, 1 },
		{ 5, 4, 7 },
		{ 7, 6, 5 },
		{ 4, 0, 3 },
		{ 3, 7, 4 },
		{ 2, 6, 7 },
		{ 7, 3, 2 },
		{ 1, 5, 4 },
		{ 4, 0, 1 }
	};
	Color colors[12] = {
		{ 1.0f, 0.5f, 0.5f, 1.0f },
		{ 1.0f, 1.0f, 0.5f, 1.0f },
		{ 1.0f, 0.0f, 1.0f, 1.0f },
		{ 0.0f, 0.5f, 0.5f, 1.0f },
		{ 1.0f, 0.5f, 0.5f, 1.0f },
		{ 1.0f, 0.0f, 0.5f, 1.0f },
		{ 1.0f, 0.5f, 0.5f, 1.0f },
		{ 0.5f, 0.5f, 1.0f, 1.0f },
		{ 1.0f, 0.5f, 0.5f, 1.0f },
		{ 0.2f, 0.4f, 0.5f, 1.0f },
		{ 1.0f, 0.5f, 0.5f, 1.0f },
		{ 0.7f, 1.0f, 0.5f, 1.0f }
	};

	mat4 p, vp, m, R, T;
	mat4_perspective(&p, rad(70.0f), (float) buff->width / (float) buff->height, 0.001f, 200.0f);
	mat4_viewport(&vp, 0, 0, buff->width, buff->height);

	SDL_Event e;
	int done = 0;
	float an = 0.0f;
	while (!done) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				done = 1;
			}
		}
		an += 0.005f;

		mat4_rotation(&R, ctor(vec3, sin(an), 1, cos(an)), an);
		mat4_translation(&T, ctor(vec3, 0, 0, -4));
		m = mat4_mul_m(T, R);

		p3d_rasty_clear(&ras, ctor(Color, 0, 0, 0, 1));
		p3d_rasty_bind_texture(&ras, tex_id);
		p3d_rasty_set_color(&ras, ctor(Color, 1.0f, 0.5f, 0.1f, 1.0f));

		mat4 mp = mat4_mul_m(p, m);
		for (int i = 0; i < 12; i++) {
			Vertex p0 = p3d_vertex_transform(cube[faces[i][0]], mp);
			Vertex p1 = p3d_vertex_transform(cube[faces[i][1]], mp);
			Vertex p2 = p3d_vertex_transform(cube[faces[i][2]], mp);

			p3d_rasty_triangle(&ras, p0, p1, p2);
		}

//		for (int y = 0; y < buff->height; y++) {
//			for (int x = 0; x < buff->width; x++) {
//				int index = x + y * buff->width;
//				float depth = ras.zbuffer[index];
//				p3d_bitmap_set(buff, x, y, ctor(Color, depth, depth, depth, 1));
//			}
//		}

		p3d_display_swap_buffers(&d);
	}

	p3d_rasty_free(&ras);
	p3d_display_free(&d);

	SDL_Quit();
	return 0;
}
