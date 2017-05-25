#ifndef VERTEX_H
#define VERTEX_H

#include "vecmath.h"

typedef struct p3d_vertex {
	vec4 position;
	vec2 uv;
} Vertex;

Vertex p3d_vertex_transform(Vertex v, mat4 m);
void p3d_vertex_perspective_divide(Vertex* v);

#endif // VERTEX_H
