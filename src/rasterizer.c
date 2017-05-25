#include "rasterizer.h"

#include "util.h"
#include <math.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

#define MAX_ITEMS 9999

void p3d_rasty_new(Rasterizer* r, Bitmap* buffer) {
	r->buffer = buffer;
	r->color.r = 1;
	r->color.g = 1;
	r->color.b = 1;
	r->color.a = 1;
	r->bound_texture = NULL;

	int zsize = buffer->width * buffer->height;
	r->zbuffer = (float*) malloc(zsize * sizeof(float));
	for (int i = 0; i < zsize; i++) {
		r->zbuffer[i] = 0;
	}

	mat4_viewport(&r->viewport, 0, 0, buffer->width, buffer->height);
	mat4_perspective(&r->projection, rad(70.0f), (float) buffer->width / (float) buffer->height, 0.01f, 100.f);
	mat4_identity(&r->view);
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
				if (r->bound_texture) {
					float u = (p1.uv.x * bc_clip.x + p2.uv.x * bc_clip.y + p3.uv.x * bc_clip.z) /
							  (bc_clip.x + bc_clip.y + bc_clip.z);
					float v = (p1.uv.y * bc_clip.x + p2.uv.y * bc_clip.y + p3.uv.y * bc_clip.z) /
							  (bc_clip.x + bc_clip.y + bc_clip.z);

					if (u > 1.0f) {
						u = u - 1.0f;
					} else if (u < 0.0f) {
						u = u + 1.0f;
					}
					if (v > 1.0f) {
						v = v - 1.0f;
					} else if (v < 0.0f) {
						v = v + 1.0f;
					}

					float tx = (int)(u * r->bound_texture->width);
					float ty = (int)(v * r->bound_texture->height);

					Color col = p3d_color_mul(r->color, p3d_bitmap_get(r->bound_texture, tx, ty));
					if (col.a > 0) { // Draws only if alpha is greater than 0
						r->zbuffer[index] = depth;
						p3d_bitmap_set(r->buffer, P.x, P.y, col);
					}
				} else {
					r->zbuffer[index] = depth;
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
	free(r->zbuffer);
	p3d_bitmap_free(r->buffer);
}

void p3d_rasty_bind_texture(Rasterizer* r, Bitmap* texture) {
	r->bound_texture = texture;
}

void p3d_rasty_set_color(Rasterizer* r, Color col) {
	r->color = col;
}

void p3d_rasty_model_new(Model* m, Vertex* v, Uint32* i, int nv, int ni) {
	m->vertices = v;
	m->indices = i;
	m->num_indices = ni;
	m->num_vertices = nv;
}

void p3d_rasty_model(Rasterizer* r, Model model, mat4 transform) {
	mat4 m = mat4_mul_m(mat4_mul_m(r->projection, r->view), transform);
	for (int i = 0; i < model.num_indices; i += 3) {
		Vertex v0 = p3d_vertex_transform(model.vertices[model.indices[i + 0]], m);
		Vertex v1 = p3d_vertex_transform(model.vertices[model.indices[i + 1]], m);
		Vertex v2 = p3d_vertex_transform(model.vertices[model.indices[i + 2]], m);
		p3d_rasty_triangle(r, v0, v1, v2);
	}
}

void p3d_rasty_set_view_matrix(Rasterizer* r, mat4 vm) {
	r->view = vm;
}

void p3d_rasty_set_projection_matrix(Rasterizer* r, mat4 p) {
	r->projection = p;
}

void p3d_rasty_billboard(Rasterizer* r, vec3 pos) {
	mat4 la;

	mat4 viewrot = {
		 r->view.value[0][0], r->view.value[0][1], r->view.value[0][2], 0,
		 r->view.value[1][0], r->view.value[1][1], r->view.value[1][2], 0,
		 r->view.value[2][0], r->view.value[2][1], r->view.value[2][2], 0,
		 0, 0, 0, 1 ,
	};

	vec4 camPos = (mat4_get_column(r->view, 3));
	camPos = mat4_mul_v4(viewrot, camPos);

	vec3 npos = vec3_mul_s(pos, -1);
	mat4_lookat(&la, vec3_from_vec4(camPos), npos, ctor(vec3, 0, -1, 0));
	mat4_translate(&la, npos);

	Vertex verts[4] = {
		{ {-1, -1, 0, 1}, { 0, 0 } },
		{ { 1, -1, 0, 1}, { 1, 0 } },
		{ { 1,  1, 0, 1}, { 1, 1 } },
		{ {-1,  1, 0, 1}, { 0, 1 } }
	};

	mat4 m = mat4_mul_m(r->projection, la);
	for (int i = 0; i < 4; i++) {
		verts[i] = p3d_vertex_transform(verts[i], m);
	}

	p3d_rasty_triangle(r, verts[0], verts[1], verts[2]);
	p3d_rasty_triangle(r, verts[2], verts[3], verts[0]);
}

////

static int line_size(char* line) {
	int sz = 0;
	while (*line != '\0') {
		sz++;
		line++;
	}
	return sz;
}

static int count_slashes(char* str) {
	int c = 0;
	while (*str != '\0') {
		if (*str == '/') {
			c++;
		}
		str++;
	}
	return c;
}

static bool has_repeated_slashes(char* str) {
	int len = line_size(str);
	for (int i = 0; i < len; i++) {
		int j = i + 1;
		if (j > len-1) { break; }
		if (str[i] == '/' && str[j] == '/') {
			return true;
		}
	}
	return false;
}

void p3d_rasty_model_from_file(Model* m, const char* fileName) {
	FILE* fp = fopen(fileName, "r");
	if (!fp) {
		return;
	}

	int vertex_count = 0, index_count = 0, uv_count = 0;
	vec4 vertices[MAX_ITEMS];
	vec2 uvs[MAX_ITEMS];
	int uv_indices[MAX_ITEMS];
	int vert_indices[MAX_ITEMS];

	char line[512];
	while (fgets(line, 512, fp) != NULL)  {
		line[line_size(line)-1] = '\0';
		if (line[0] == 'v' && line[1] == ' ') {
			char v;
			float vx, vy, vz;
			sscanf(line, "%c %f %f %f", &v, &vx, &vy, &vz);
			vertices[vertex_count++] = ctor(vec4, vx, vy, vz, 1);
		} else if (line[0] == 'v' && line[1] == 't') {
			char v[2];
			float tx, ty;
			sscanf(line, "%s %f %f", v, &tx, &ty);
			uvs[uv_count++] = ctor(vec2, tx, ty);
		} else if (line[0] == 'f' && line[1] == ' ') {
			char v;
			char v0[16], v1[16], v2[16];
			sscanf(line, "%c %s %s %s", &v, v0, v1, v2);
			char* vs[3] = { v0, v1, v2 };
			for (int i = 0; i < 3; i++) {
				char* vstr = vs[i];
				int slashes = count_slashes(vstr);
				bool repeated = has_repeated_slashes(vstr);
				if (!repeated && slashes == 1) { // POS/UV
					int pos, uv;
					sscanf(vstr, "%d/%d", &pos, &uv);
					vert_indices[index_count] = pos;
					uv_indices[index_count] = uv;
					index_count++;
				}
			}
		}
	}
	fclose(fp);

	Vertex f_verts[MAX_ITEMS];
	int f_indices[MAX_ITEMS];

	for (int i = 0; i < index_count; i++) {
		int vertex_index = vert_indices[i];
		int uv_index = uv_indices[i];
		f_verts[vertex_index].position = vertices[vertex_index-1];
		f_verts[vertex_index].uv = uvs[uv_index-1];
		f_indices[i] = vertex_index;
	}

	m->indices = f_indices;
	m->vertices = f_verts;
	m->num_indices = index_count;
	m->num_vertices = vertex_count;
}
