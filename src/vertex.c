#include "vertex.h"

#include <math.h>

Vertex p3d_vertex_transform(Vertex v, mat4 m) {
	mat4 nmat = mat4_transpose(mat4_invert(m));
	Vertex r;
	r.position = mat4_mul_v4(m, v.position);
	r.normal = mat4_mul_v(m, v.normal, 0.0f);
	r.uv = v.uv;
	return r;
}

void p3d_vertex_perspective_divide(Vertex* v) {
	v->position = vec4_perspective_divide(v->position);
}

bool p3d_vertex_is_inside_view_frustum(Vertex v) {
	return abs(v.position.x) <= abs(v.position.w) &&
			abs(v.position.y) <= abs(v.position.w) &&
			abs(v.position.z) <= abs(v.position.w);
}

float p3d_vertex_get_component(Vertex v, int index) {
	switch (index) {
		case 0: return v.position.x;
		case 1: return v.position.y;
		case 2: return v.position.z;
		case 3: return v.position.w;
		default: return -1;
	}
}

Vertex p3d_vertex_lerp(Vertex a, Vertex b, float amt) {
	Vertex r;
	r.position.x = lerp(a.position.x, b.position.x, amt);
	r.position.y = lerp(a.position.y, b.position.y, amt);
	r.position.z = lerp(a.position.z, b.position.z, amt);
	r.position.w = lerp(a.position.w, b.position.w, amt);
	r.uv.x = lerp(a.uv.x, b.uv.x, amt);
	r.uv.y = lerp(a.uv.y, b.uv.y, amt);
	r.normal.x = lerp(a.normal.x, b.normal.x, amt);
	r.normal.y = lerp(a.normal.y, b.normal.y, amt);
	r.normal.z = lerp(a.normal.z, b.normal.z, amt);
	return r;
}
