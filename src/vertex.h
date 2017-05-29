#ifndef VERTEX_H
#define VERTEX_H

#include "vecmath.h"
#include "util.h"

typedef struct p3d_vertex {
	vec4 position;
	vec2 uv;
	vec3 normal;
} Vertex;

Vertex p3d_vertex_transform(Vertex v, mat4 m);
void p3d_vertex_perspective_divide(Vertex* v);
bool p3d_vertex_is_inside_view_frustum(Vertex v);
float p3d_vertex_get_component(Vertex v, int index);
Vertex p3d_vertex_lerp(Vertex a, Vertex b, float amt);

#endif // VERTEX_H
