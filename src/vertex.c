#include "vertex.h"

Vertex p3d_vertex_transform(Vertex v, mat4 m) {
	Vertex r;
	r.position = mat4_mul_v4(m, v.position);
	r.uv = v.uv;
	return r;
}

void p3d_vertex_perspective_divide(Vertex* v) {
	v->position = vec4_perspective_divide(v->position);
}
