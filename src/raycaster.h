#ifndef RAYCASTER_H
#define RAYCASTER_H

#define MAX_RAY_ITERATIONS 512

#include "array.h"
#include "bitmap.h"
#include "vecmath.h"
#include "util.h"

typedef struct p3d_camera {
	float rotation, fov;
	vec2 plane, position;
} Camera;

typedef struct p3d_raycaster {
	Bitmap* buffer;
	Camera camera;
	int* map;
	int map_width, map_height;
	int floor_texture, ceiling_texture;
	Array textures;
} RayCaster;

void p3d_casty_new(RayCaster* r, Bitmap* buffer, int mapW, int mapH);
void p3d_casty_free(RayCaster* r);

void p3d_casty_clear(RayCaster* r, Color color);
void p3d_casty_line(RayCaster* r, int x0, int y0, int x1, int y1, Color col);

void p3d_casty_flush(RayCaster* r);

char p3d_casty_map_get(RayCaster* r, int x, int y);
void p3d_casty_map_set(RayCaster* r, int x, int y, char type);

void p3d_casty_register_texture(RayCaster* r, Bitmap* tex);

void p3d_casty_set_camera_rotation(RayCaster* r, float rot);
void p3d_casty_set_camera_fov(RayCaster* r, float fov);

#endif // RAYCASTER_H
