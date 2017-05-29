#include "raycaster.h"

#include <math.h>
#include <stdlib.h>

void p3d_casty_new(RayCaster* r, Bitmap* buffer, int mapW, int mapH) {
	r->buffer = buffer;
	r->camera.fov = 60.0f;
	r->camera.rotation = 0.0f;
	r->camera.position = ctor(vec2, 22, 22);
	r->floor_texture = -1;
	r->ceiling_texture = -1;

	array_new(&r->textures, Bitmap);

	r->map = (int*) malloc(mapW * mapH * sizeof(int));
	r->map_width = mapW;
	r->map_height = mapH;
	memset(r->map, 0, mapW * mapH * sizeof(int));
}

void p3d_casty_free(RayCaster* r) {
	p3d_bitmap_free(r->buffer);
	free(r->map);
	array_free(&r->textures);
}

void p3d_casty_set_camera_rotation(RayCaster* r, float rot) {
	r->camera.rotation = rot;
	r->camera.plane.x = 0.0f;
	r->camera.plane.y = tan(r->camera.fov / 2.0f);
	r->camera.plane = vec2_rotate(r->camera.plane, r->camera.rotation);
}

void p3d_casty_set_camera_fov(RayCaster* r, float fov) {
	r->camera.fov = fov;
	r->camera.plane.x = 0.0f;
	r->camera.plane.y = tan(fov / 2.0f);
	r->camera.plane = vec2_rotate(r->camera.plane, r->camera.rotation);
}

void p3d_casty_line(RayCaster* r, int x0, int y0, int x1, int y1, Color col) {
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
			p3d_bitmap_set(r->buffer, y, x, col);
		} else {
			p3d_bitmap_set(r->buffer, x, y, col);
		}
		error2 += derror2;
		if (error2 > dx) {
			y += y1 > y0 ? 1 : -1;
			error2 -= dx * 2;
		}
	}
}

void p3d_casty_clear(RayCaster* r, Color color) {
	p3d_bitmap_clear(r->buffer, color);
}

void p3d_casty_flush(RayCaster* r) {
	int w = r->buffer->width;
	int h = r->buffer->height;
	vec2 dir = {
		cos(r->camera.rotation),
		sin(r->camera.rotation)
	};

	for (int x = 0; x < w; x++) {
		float camX = 2.0f * x / (float) w - 1;
		vec2 rayPos = r->camera.position;
		vec2 rayDir = {
			dir.x + r->camera.plane.x * camX,
			dir.y + r->camera.plane.y * camX
		};

		Point rayPosMap = { (int)rayPos.x, (int)rayPos.y };

		vec2 sideDist;
		vec2 deltaDist = {
			sqrt(1.0f + (rayDir.y * rayDir.y) / (rayDir.x * rayDir.x)),
			sqrt(1.0f + (rayDir.x * rayDir.x) / (rayDir.y * rayDir.y))
		};
		float perpWallDist;

		int stepX, stepY;
		bool hit = false;
		int side;
		if (rayDir.x < 0) {
			stepX = -1;
			sideDist.x = (rayPos.x - rayPosMap.x) * deltaDist.x;
		} else {
			stepX = 1;
			sideDist.x = (rayPosMap.x + 1.0f - rayPos.x) * deltaDist.x;
		}

		if (rayDir.y < 0) {
			stepY = -1;
			sideDist.y = (rayPos.y - rayPosMap.y) * deltaDist.y;
		} else {
			stepY = 1;
			sideDist.y = (rayPosMap.y + 1.0f - rayPos.y) * deltaDist.y;
		}

		// DDA
		int rayIts = 0;
		while (!hit) {
			if (rayIts > MAX_RAY_ITERATIONS) {
				break;
			}
			// Jump to next map square, OR in x-direction, OR in y-direction
			if (sideDist.x < sideDist.y) {
				sideDist.x += deltaDist.x;
				rayPosMap.x += stepX;
				side = 0;
			} else {
				sideDist.y += deltaDist.y;
				rayPosMap.y += stepY;
				side = 1;
			}

			// Check if ray has hit a wall
			if (rayPosMap.x >= 0 && rayPosMap.y >= 0) {
				if (p3d_casty_map_get(r, rayPosMap.x, rayPosMap.y) > 0) {
					hit = true;
				}
			}
			rayIts++;
		}

		//if (!hit) { continue; }
		// Calculate distance projected on camera direction (oblique distance will give fisheye effect!)
		if (side == 0) perpWallDist = (rayPosMap.x - rayPos.x + (1 - stepX) / 2) / rayDir.x;
		else           perpWallDist = (rayPosMap.y - rayPos.y + (1 - stepY) / 2) / rayDir.y;

		int lineHeight = (int)(h / perpWallDist);
		int drawStart = -lineHeight / 2 + h / 2;
		int drawEnd = lineHeight / 2 + h / 2;

		if (drawStart < 0) {
			drawStart = 0;
		}
		if (drawEnd >= h) {
			drawEnd = h - 1;
		}

		int texID = p3d_casty_map_get(r, rayPosMap.x, rayPosMap.y) - 1;

		float wallX;
		if (side == 0) wallX = rayPos.y + perpWallDist * rayDir.y;
		else           wallX = rayPos.x + perpWallDist * rayDir.x;
		wallX -= floor(wallX);
		if (texID > -1 && texID <= r->textures.length-1) {
			Bitmap tex = array_get(&r->textures, texID, Bitmap);

			// x coordinate on the texture
			int texX = (int) (wallX * (float) tex.width);
			if(side == 0 && rayDir.x > 0) texX = tex.width - texX - 1;
			if(side == 1 && rayDir.y < 0) texX = tex.width - texX - 1;

			for (int y = drawStart; y < drawEnd; y++) {
				int d = y * 256 - h * 128 + lineHeight * 128;
				int texY = ((d * tex.height) / lineHeight) / 256;
				Color col = p3d_bitmap_get(&tex, texX, texY);

				if (col.a > 0) {
					p3d_bitmap_set(r->buffer, x, y, col);
				}
			}
		} else {
			for (int y = drawStart; y < drawEnd; y++) {
				p3d_bitmap_set(r->buffer, x, y, P3D_COLOR_WHITE);
			}
		}

		//FLOOR CASTING
		float floorXWall, floorYWall;

		//4 different wall directions possible
		if(side == 0 && rayDir.x > 0) {
			floorXWall = rayPosMap.x;
			floorYWall = rayPosMap.y + wallX;
		} else if(side == 0 && rayDir.x < 0) {
			floorXWall = rayPosMap.x + 1.0;
			floorYWall = rayPosMap.y + wallX;
		} else if(side == 1 && rayDir.y > 0)	{
			floorXWall = rayPosMap.x + wallX;
			floorYWall = rayPosMap.y;
		} else {
			floorXWall = rayPosMap.x + wallX;
			floorYWall = rayPosMap.y + 1.0;
		}

		float distWall, distPlayer, currentDist;

		distWall = perpWallDist;
		distPlayer = 0.0;

		if (drawEnd < 0) drawEnd = h; //becomes < 0 when the integer overflows

		//draw the floor from drawEnd to the bottom of the screen
		for (int y = drawEnd + 1; y < h; y++) {
			currentDist = h / (2.0 * y - h); //you could make a small lookup table for this instead

			float weight = (currentDist - distPlayer) / (distWall - distPlayer);

			float currentFloorX = weight * floorXWall + (1.0 - weight) * r->camera.position.x;
			float currentFloorY = weight * floorYWall + (1.0 - weight) * r->camera.position.y;

			int ftex = r->floor_texture-1;
			int ctex = r->ceiling_texture-1;
			if (ftex > -1 && ftex <= r->textures.length-1) {
				Bitmap tex = array_get(&r->textures, ftex, Bitmap);
				int texX, texY;
				texX = (int)(currentFloorX * tex.width) % tex.width;
				texY = (int)(currentFloorY * tex.height) % tex.height;

				//floor
				Color col = p3d_bitmap_get(&tex, texX, texY);
				p3d_bitmap_set(r->buffer, x, y, col);
			}

			if (ctex > -1 && ctex <= r->textures.length-1) {
				Bitmap tex = array_get(&r->textures, ctex, Bitmap);
				int texX, texY;
				texX = (int)(currentFloorX * tex.width) % tex.width;
				texY = (int)(currentFloorY * tex.height) % tex.height;

				//ceiling (symmetrical!)
				Color col = p3d_bitmap_get(&tex, texX, texY);
				p3d_bitmap_set(r->buffer, x, h - y, col);
			}
		}
	}
}

char p3d_casty_map_get(RayCaster* r, int x, int y) {
	if (x < 0 || y < 0 || x >= r->map_width || y >= r->map_height) {
		return -1;
	}
	return r->map[x + y * r->map_width];
}

void p3d_casty_map_set(RayCaster* r, int x, int y, char type) {
	if (x < 0 || y < 0 || x >= r->map_width || y >= r->map_height) {
		return;
	}
	r->map[x + y * r->map_width] = type;
}

void p3d_casty_register_texture(RayCaster* r, Bitmap* tex) {
	array_add(&r->textures, tex);
}
