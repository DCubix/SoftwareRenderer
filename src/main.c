#include <stdio.h>

#include "vecmath.h"
#include "display.h"
#include "rasterizer.h"
#include "util.h"

#include <math.h>

#define N 1.0f
#define U 1.0f
#define V 1.0f

int main(int argc, char *argv[]) {
	Display d;
	p3d_display_new(&d, 640, 480, 2, "Test");

	Rasterizer ras;
	p3d_rasty_new(&ras, &d.buffer);

	Bitmap tex;
	p3d_bitmap_from_file(&tex, "window.png");

	Model cube;
	p3d_rasty_model_from_file(&cube, "cube.obj");

	mat4 m, R, T;

	SDL_Event e;
	bool done = false;
	float an = 0.0f;
	while (!done) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				done = true;
			}
		}
		an += 0.01f;

		mat4_rotation(&R, ctor(vec3, sin(an), 1, cos(an)), an);
		mat4_translation(&T, ctor(vec3, 0, 0, -4));
		m = mat4_mul_m(T, R);

		p3d_rasty_clear(&ras, ctor(Color, 0, 0, 0, 1));

		p3d_rasty_bind_texture(&ras, &tex);

		p3d_rasty_set_color(&ras, ctor(Color, 1, 1, 1, 1));
		p3d_rasty_model(&ras, cube, m);

		p3d_rasty_bind_texture(&ras, &d.buffer);
		p3d_rasty_billboard(&ras, ctor(vec3, sin(an * 2.0f) * 2.0f, 0, -1));

//		for (int y = 0; y < d.buffer.height; y++) {
//			for (int x = 0; x < d.buffer.width; x++) {
//				float z = ras.zbuffer[x + y * d.buffer.width];
//				p3d_bitmap_set(&d.buffer, x, y, ctor(Color, z, z, z, 1));
//			}
//		}

		p3d_display_swap_buffers(&d);
	}

	p3d_rasty_free(&ras);
	p3d_display_free(&d);

	SDL_Quit();
	return 0;
}
