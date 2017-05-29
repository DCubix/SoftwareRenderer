#include <stdio.h>

#include "vecmath.h"
#include "display.h"
#include "rasterizer.h"
#include "util.h"

#include <math.h>

int main(int argc, char *argv[]) {
	Display d;
	p3d_display_new(&d, 640, 480, 2, "Test");

	Rasterizer ras;
	p3d_rasty_new(&ras, &d.buffer);

	p3d_rasty_set_fog_enabled(&ras, true);
	p3d_rasty_set_fog_density(&ras, 0.08f);
	p3d_rasty_set_fog_color(&ras, ctor(Color, 0.1f, 0.05f, 0.05f, 1.0f));

	Bitmap tex;
	p3d_bitmap_from_file(&tex, "window.png");

	Model model;
	p3d_rasty_model_from_file(&model, "cube.obj");

	mat4 m, R, T;

	SDL_Event e;
	bool done = false;
	float an = 0.0f;

	float timeStep = 1.0f / 60.0f;
	float lastTime = SDL_GetTicks() / 1000.0f;
	float accum = 0.0f;

	while (!done) {
		bool canRender = false;
		float currentTime = SDL_GetTicks() / 1000.0f;
		float delta = currentTime - lastTime;
		lastTime = currentTime;
		accum += delta;

		while (accum >= timeStep) {
			accum -= timeStep;
			while (SDL_PollEvent(&e)) {
				if (e.type == SDL_QUIT) {
					done = true;
				}
			}
			an += 0.5f * timeStep;

			mat4_rotation(&R, ctor(vec3, 0, 1, 0), an);
			mat4_translation(&T, ctor(vec3, 0, 0, 2.0f));
			m = mat4_mul_m(T, R);

			canRender = true;
		}

		if (canRender) {
			p3d_rasty_clear(&ras, ctor(Color, 0, 0, 0, 1));

			p3d_rasty_bind_texture(&ras, &tex);

			p3d_rasty_set_color(&ras, ctor(Color, 1.0f, 1.0f, 1.0f, 1));
			p3d_rasty_model(&ras, model, m);

//			for (int y = 0; y < d.buffer.height; y++) {
//				for (int x = 0; x < d.buffer.width; x++) {
//					float z = min(max(ras.zbuffer[x + y * d.buffer.width], 0.0f), 1.0f);
//					p3d_bitmap_set(&d.buffer, x, y, ctor(Color, z, z, z, 1));
//				}
//			}

			p3d_display_swap_buffers(&d);
		}
	}

	p3d_rasty_free(&ras);
	p3d_display_free(&d);

	SDL_Quit();
	return 0;
}
