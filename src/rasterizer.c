#include "rasterizer.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <assert.h>

#define MAX_ITEMS 9999

void p3d_rasty_new(Rasterizer* r, Bitmap* buffer) {
	r->buffer = buffer;
	r->color.r = 1;
	r->color.g = 1;
	r->color.b = 1;
	r->color.a = 1;
	r->bound_texture = NULL;
	r->fog_enabled = false;
	r->fog_density = 0.6f;
	r->fog_color = ctor(Color, 0.0f, 0.18f, 0.25f, 1.0f);

	int zsize = buffer->width * buffer->height;
	r->zbuffer = (float*) malloc(zsize * sizeof(float));
	for (int i = 0; i < zsize; i++) {
		r->zbuffer[i] = 0;
	}

	mat4_viewport(&r->viewport, 0, 0, buffer->width, buffer->height);
	mat4_perspective(&r->projection, rad(70.0f), (float) buffer->width / (float) buffer->height, 0.01f, 500.f);
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

	vec3 pts2[3] = {
		{ 0, 0, 0 },
		{ 0, 0, 0 },
		{ 0, 0, 0 }
	};
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

	float sz = (float) max(r->buffer->width, r->buffer->height);
	float bcsoff = -0.01f / sz;

	for (int py = bboxmin.y; py <= bboxmax.y; ++py) {
		for (int px = bboxmin.x; px <= bboxmax.x; ++px) {
			vec2 P = { px, py };
			vec3 bc_screen = p3d_barycentric(pts2[0], pts2[1], pts2[2], P);
			if (bc_screen.x < bcsoff || bc_screen.y < bcsoff || bc_screen.z < bcsoff) { continue; }

			vec3 bc_clip = {
				bc_screen.x / pts[0].position.w,
				bc_screen.y / pts[1].position.w,
				bc_screen.z / pts[2].position.w
			};

			float depth = bc_clip.x + bc_clip.y + bc_clip.z;

			if (r->zbuffer[px + py * r->buffer->width] < depth) {
				Color col = { 0, 0, 0, 0 };
				if (r->bound_texture) {
					float u = (p1.uv.x * bc_clip.x + p2.uv.x * bc_clip.y + p3.uv.x * bc_clip.z) /
							  (bc_clip.x + bc_clip.y + bc_clip.z);
					float v = (p1.uv.y * bc_clip.x + p2.uv.y * bc_clip.y + p3.uv.y * bc_clip.z) /
							  (bc_clip.x + bc_clip.y + bc_clip.z);

					if (u < 0.0f) {
						u = 1.0f + u;
					} else if (u > 1.0f) {
						u = u - 1.0f;
					}
					if (v < 0.0f) {
						v = 1.0f + v;
					} else if (v > 1.0f) {
						v = v - 1.0f;
					}

					int tx = (int)(u * (float)(r->bound_texture->width-1) + 0.5f);
					int ty = (int)(v * (float)(r->bound_texture->height-1) + 0.5f);

					col = p3d_color_mul(r->color, p3d_bitmap_get(r->bound_texture, tx, ty));
				} else {
					col = r->color;
				}

				if (r->fog_enabled && col.a > 0) {
					float exponent = pow(depth * (1.0f - r->fog_density), 4);
					float f = 1.0f - (1.0f / exp(exponent));

					col.r = f * col.r + (1.0f - f) * r->fog_color.r;
					col.g = f * col.g + (1.0f - f) * r->fog_color.g;
					col.b = f * col.b + (1.0f - f) * r->fog_color.b;
				}

				if (col.a > 0) {
					r->zbuffer[px + py * r->buffer->width] = depth;
					p3d_bitmap_set(r->buffer, px, py, col);
				}
			}
		}
	}
}

void p3d_rasty_clipped_triangle(Rasterizer* r, Vertex p1, Vertex p2, Vertex p3) {
	if (p3d_vertex_is_inside_view_frustum(p1) &&
		p3d_vertex_is_inside_view_frustum(p2) &&
		p3d_vertex_is_inside_view_frustum(p3))
	{
		p3d_rasty_triangle(r, p1, p2, p3);
		return;
	}

	Array vertices, aux;
	array_new(&vertices, Vertex);
	array_new(&aux, Vertex);

	array_add(&vertices, &p1);
	array_add(&vertices, &p2);
	array_add(&vertices, &p3);

	if (p3d_clip_poly_axis(&vertices, &aux, 0) &&
		p3d_clip_poly_axis(&vertices, &aux, 1) &&
		p3d_clip_poly_axis(&vertices, &aux, 2))
	{
		Vertex initial = array_get(&vertices, 0, Vertex);
		for (int i = 1; i < vertices.length-1; i++) {
			Vertex v1 = array_get(&vertices, i, Vertex);
			Vertex v2 = array_get(&vertices, i+1, Vertex);
			p3d_rasty_triangle(r, initial, v1, v2);
		}
	}

	array_free(&vertices);
	array_free(&aux);
}

bool p3d_clip_poly_axis(Array* vertices, Array* aux, int comp) {
	p3d_clip_poly_component(vertices, comp, 1.0f, aux);
	array_clear(vertices);

	if (aux->length == 0) {
		return false;
	}

	p3d_clip_poly_component(aux, comp, -1.0f, vertices);
	array_clear(aux);

	return vertices->length != 0;
}

void p3d_clip_poly_component(Array* vertices, int comp, float fac, Array* result) {
	Vertex previousVertex = array_get(vertices, vertices->length-1, Vertex);
	float previousComp = p3d_vertex_get_component(previousVertex, comp) * fac;
	bool previousInside = previousComp <= previousVertex.position.w;

	for (int i = 0; i < vertices->length; i++) {
		Vertex currentVertex = array_get(vertices, i, Vertex);
		float currentComp = p3d_vertex_get_component(currentVertex, comp) * fac;
		bool currentInside = currentComp <= currentVertex.position.w;
		if (currentInside ^ previousInside) {
			float wMB = previousVertex.position.w - previousComp;
			float lerpAmt = wMB / (wMB - (currentVertex.position.w - currentComp));
			Vertex lv = p3d_vertex_lerp(previousVertex, currentVertex, lerpAmt);
			array_add(result, &lv);
		}
		if (currentInside) {
			array_add(result, &currentVertex);
		}

		previousComp = currentComp;
		previousInside = currentInside;
		previousVertex = currentVertex;
	}
}

vec3 p3d_barycentric(vec3 a, vec3 b, vec3 c, vec2 p) {
	vec3 s0 = ctor(vec3, c.x - a.x, b.x - a.x, a.x - p.x);
	vec3 s1 = ctor(vec3, c.y - a.y, b.y - a.y, a.y - p.y);

	vec3 u = vec3_cross(s0, s1);

	vec3 r = { -1, 1, 1 };
	if (abs(u.z) < 1.0f) {
		return r;
	}
	r.x = 1.0f - (u.x + u.y) / u.z;
	r.y = u.y / u.z;
	r.z = u.x / u.z;
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
		Vertex p0 = model.vertices[model.indices[i + 0]];
		Vertex p1 = model.vertices[model.indices[i + 1]];
		Vertex p2 = model.vertices[model.indices[i + 2]];
		Vertex v0 = p3d_vertex_transform(p0, m);
		Vertex v1 = p3d_vertex_transform(p1, m);
		Vertex v2 = p3d_vertex_transform(p2, m);

		p3d_rasty_clipped_triangle(r, v0, v1, v2);
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

void p3d_rasty_sprite(Rasterizer* r, int srcx, int srcy, int srcw, int srch, int dstx, int dsty) {
	if (!r->bound_texture) return;
	for (int y = srcy; y < srcy + srch; y++) {
		for (int x = srcx; x < srcx + srcw; x++) {
			Color col = p3d_bitmap_get(r->bound_texture, x, y);
			if (col.a > 0) {
				p3d_bitmap_set(r->buffer, dstx + x, dsty + y, col);
			}
		}
	}
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

void p3d_rasty_model_from_file(Model* m, const char* fileName) {
	FILE* fp = fopen(fileName, "r");
	if (!fp) {
		return;
	}

	int index_count = 0;
	Array vert_indices, uv_indices, uvs, vertices;
	array_new(&vert_indices, int);
	array_new(&uv_indices, int);
	array_new(&uvs, vec2);
	array_new(&vertices, vec4);

	char line[512];
	while (fgets(line, 512, fp) != NULL)  {
		line[line_size(line)-1] = '\0';
		if (line[0] == 'v' && line[1] == ' ') {
			char v;
			float vx, vy, vz;
			sscanf(line, "%c %f %f %f", &v, &vx, &vy, &vz);
			vec4 pos = ctor(vec4, vx, vy, vz, 1);
			array_add(&vertices, &pos);
		} else if (line[0] == 'v' && line[1] == 't') {
			char v[2];
			float tx, ty;
			sscanf(line, "%s %f %f", v, &tx, &ty);
			vec2 uv = ctor(vec2, tx, ty);
			array_add(&uvs, &uv);
		} else if (line[0] == 'f' && line[1] == ' ') {
			char v;
			int idx[3][2];
			int read = sscanf(line, "%c %d/%d %d/%d %d/%d",
					&v,
					&idx[0][0], &idx[0][1],
					&idx[1][0], &idx[1][1],
					&idx[2][0], &idx[2][1]
			);
			if (read == 7) {
				for (int i = 0; i < 3; i++) {
					int pos = idx[i][0]-1;
					int uv = idx[i][1]-1;
					array_add(&vert_indices, &pos);
					array_add(&uv_indices, &uv);
					index_count++;
				}
			}
		} else if (line[0] == '#') {
			continue;
		}
	}
	fclose(fp);

	Vertex* f_verts = (Vertex*) malloc(vertices.length * sizeof(Vertex));
	int* f_inds = (int*) malloc(index_count * sizeof(int));

	for (int i = 0; i < index_count; i++) {
		int vertex_index = array_get(&vert_indices, i, int);
		int uv_index = array_get(&uv_indices, i, int);
		vec4 pos = array_get(&vertices, vertex_index, vec4);
		vec2 uv = array_get(&uvs, uv_index, vec2);
		f_verts[vertex_index].position = ctor(vec4, pos.x, pos.y, pos.z, pos.w);
		f_verts[vertex_index].normal = ctor(vec3, 0, 0, 0);
		f_verts[vertex_index].uv = ctor(vec2, uv.x, uv.y);
		f_inds[i] = vertex_index;
	}

	array_free(&vertices);
	array_free(&uvs);
	array_free(&vert_indices);
	array_free(&uv_indices);

	m->indices = f_inds;
	m->vertices = f_verts;
	m->num_indices = index_count;
	m->num_vertices = vertices.length;
}

void p3d_rasty_model_free(Model* m) {
	free(m->indices);
	free(m->vertices);
}

void p3d_rasty_model_calc_normals(Model* m) {
	for (int i = 0; i < m->num_indices; i += 3) {
		int i0 = m->indices[i + 0];
		int i1 = m->indices[i + 1];
		int i2 = m->indices[i + 2];
		Vertex v0 = m->vertices[i0];
		Vertex v1 = m->vertices[i1];
		Vertex v2 = m->vertices[i2];

		if (v0.position.y > v1.position.y) swap(Vertex, v0, v1);
		if (v0.position.y > v2.position.y) swap(Vertex, v0, v2);
		if (v1.position.y > v2.position.y) swap(Vertex, v1, v2);

		vec3 p0 = vec3_from_vec4(v0.position);
		vec3 p1 = vec3_from_vec4(v1.position);
		vec3 p2 = vec3_from_vec4(v2.position);

		vec3 e0 = vec3_sub(p1, p0);
		vec3 e1 = vec3_sub(p2, p0);

		vec3 n = vec3_normalize(vec3_cross(e0, e1));

		m->vertices[i + 0].normal = n;
		m->vertices[i + 1].normal = n;
		m->vertices[i + 2].normal = n;
	}

}

void p3d_rasty_line_3d(Rasterizer* r, vec3 p0, vec3 p1) {
	mat4 m = mat4_mul_m(r->projection, r->view);
	vec4 v0 = mat4_mul_v4(m, ctor(vec4, p0.x, p0.y, p0.z, 1.0f));
	vec4 v1 = mat4_mul_v4(m, ctor(vec4, p1.x, p1.y, p1.z, 1.0f));

	vec4 pts[2] = { // Viewport transform
		mat4_mul_v4(r->viewport, v0),
		mat4_mul_v4(r->viewport, v1)
	};

	vec2 pts2[2] = {
		{ 0, 0 },
		{ 0, 0 }
	};
	for (int i = 0; i < 2; i++) {
		pts2[i].x = pts[i].x / pts[i].w;
		pts2[i].y = pts[i].y / pts[i].w;
	}

	p3d_rasty_line(r, (int) pts2[0].x, (int) pts2[0].y, (int) pts2[1].x, (int) pts2[1].y);
}

void p3d_rasty_set_fog_enabled(Rasterizer* r, bool enabled) {
	r->fog_enabled = enabled;
}

void p3d_rasty_set_fog_density(Rasterizer* r, float density) {
	r->fog_density = density;
}

void p3d_rasty_set_fog_color(Rasterizer* r, Color color) {
	r->fog_color = color;
}
