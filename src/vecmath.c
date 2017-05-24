#include "vecmath.h"
#include "util.h"

#include <stdio.h>

vec2 vec2_add(vec2 a, vec2 b) {
	vec2 r;
	r.x = a.x + b.x;
	r.y = a.y + b.y;
	return r;
}

vec2 vec2_sub(vec2 a, vec2 b) {
	vec2 r;
	r.x = a.x - b.x;
	r.y = a.y - b.y;
	return r;
}

vec2 vec2_mul_v(vec2 a, vec2 b) {
	vec2 r;
	r.x = a.x * b.x;
	r.y = a.y * b.y;
	return r;
}

vec2 vec2_mul_s(vec2 a, float b) {
	vec2 r;
	r.x = a.x * b;
	r.y = a.y * b;
	return r;
}

float vec2_dot(vec2 a, vec2 b) {
	return a.x * b.x + a.y * b.y;
}

float vec2_cross(vec2 a, vec2 b) {
	return a.x * b.y - a.y * b.x;
}

float vec2_length(vec2 v) {
	return sqrt(vec2_dot(v, v));
}

vec2 vec2_normalize(vec2 v) {
	float l = vec2_length(v);
	vec2 r;
	r.x = v.x / l;
	r.y = v.y / l;
	return r;
}

////

vec3 vec3_add(vec3 a, vec3 b) {
	vec3 r;
	r.x = a.x + b.x;
	r.y = a.y + b.y;
	r.z = a.z + b.z;
	return r;
}

vec3 vec3_sub(vec3 a, vec3 b) {
	vec3 r;
	r.x = a.x - b.x;
	r.y = a.y - b.y;
	r.z = a.z - b.z;
	return r;
}

vec3 vec3_mul_v(vec3 a, vec3 b) {
	vec3 r;
	r.x = a.x * b.x;
	r.y = a.y * b.y;
	r.z = a.z * b.z;
	return r;
}

vec3 vec3_mul_s(vec3 a, float b) {
	vec3 r;
	r.x = a.x * b;
	r.y = a.y * b;
	r.z = a.z * b;
	return r;
}

float vec3_dot(vec3 a, vec3 b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3 vec3_cross(vec3 a, vec3 b) {
	vec3 r;
	r.x = a.y * b.z - b.y * a.z;
	r.y = a.z * b.x - b.z * a.x;
	r.z = a.x * b.y - b.x * a.y;
	return r;
}

float vec3_length(vec3 v) {
	return sqrt(vec3_dot(v, v));
}

vec3 vec3_normalize(vec3 v) {
	float l = vec3_length(v);
	vec3 r;
	r.x = v.x / l;
	r.y = v.y / l;
	r.z = v.z / l;
	return r;
}

////

float mat4_get(mat4 m, int row, int col) {
	return m.value[row][col];
}

void mat4_set(mat4* m, int row, int col, float v) {
	m->value[row][col] = v;
}

void mat4_set_index(mat4* m, int index, float v) {
	int col = index % 4;
	int row = index / 4;
	m->value[row][col] = v;
}

void mat4_identity(mat4* m) {
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			if (i == j) {
				mat4_set(m, j, i, 1.0f);
			} else {
				mat4_set(m, j, i, 0.0f);
			}
		}
	}
}

mat4 mat4_mul_m(mat4 a, mat4 b) {
	mat4 r;
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			mat4_set(&r, j, i,
					 mat4_get(a, j, 0) * mat4_get(b, 0, i) +
					 mat4_get(a, j, 1) * mat4_get(b, 1, i) +
					 mat4_get(a, j, 2) * mat4_get(b, 2, i) +
					 mat4_get(a, j, 3) * mat4_get(b, 3, i)
			);
		}
	}
	return r;
}

vec3 mat4_mul_v(mat4 a, vec3 b, float w) {
	vec3 r;
	vec4 _v = mat4_mul_v4(a, ctor(vec4, b.x, b.y, b.z, w));
	r.x = _v.x / _v.w;
	r.y = _v.y / _v.w;
	r.z = _v.z / _v.w;
	return r;
}

vec4 mat4_mul_v4(mat4 a, vec4 b) {
	vec4 v;
	v.x = b.x * a.value[0][0] + b.y * a.value[0][1] + b.z * a.value[0][2] + b.w * a.value[0][3];
	v.y = b.x * a.value[1][0] + b.y * a.value[1][1] + b.z * a.value[1][2] + b.w * a.value[1][3];
	v.z = b.x * a.value[2][0] + b.y * a.value[2][1] + b.z * a.value[2][2] + b.w * a.value[2][3];
	v.w = b.x * a.value[3][0] + b.y * a.value[3][1] + b.z * a.value[3][2] + b.w * a.value[3][3];
	return v;
}

void mat4_translation(mat4* m, vec3 v) {
	mat4_identity(m);
	m->value[0][3] = v.x;
	m->value[1][3] = v.y;
	m->value[2][3] = v.z;
}

void mat4_scaling(mat4* m, vec3 v) {
	mat4_identity(m);
	m->value[0][0] = v.x;
	m->value[1][1] = v.y;
	m->value[2][2] = v.z;
}

void mat4_rotation(mat4* m, vec3 axis, float a) {
	float c = cos(a);
	float s = sin(a);
	float t = 1.0f - c;

	vec3 naxis = vec3_normalize(axis);
	float x = naxis.x;
	float y = naxis.y;
	float z = naxis.z;

	m->value[0][0] = 1 + t * (x * x - 1);
	m->value[0][1] = z * s + t * x * y;
	m->value[0][2] = -y * s + t * x * z;
	m->value[0][3] = 0.0f;

	m->value[1][0] = -z * s + t * x * y;
	m->value[1][1] = 1 + t * (y * y - 1);
	m->value[1][2] = x * s + t * y * z;
	m->value[1][3] = 0.0f;

	m->value[2][0] = y * s + t * x * z;
	m->value[2][1] = -x * s + t * y * z;
	m->value[2][2] = 1 + t * (z * z - 1);
	m->value[2][3] = 0.0f;

	m->value[3][0] = 0.0f;
	m->value[3][1] = 0.0f;
	m->value[3][2] = 0.0f;
	m->value[3][3] = 1.0f;
}

void mat4_perspective(mat4* m, float fov, float aspect, float znear, float zfar) {
	float tanHalfFOV = tan(fov / 2.0f);
	float zrange = znear - zfar;

	m->value[0][0] = 1.0f / (tanHalfFOV * aspect);
	m->value[0][1] = 0;
	m->value[0][2] = 0;
	m->value[0][3] = 0;
	m->value[1][0] = 0;
	m->value[1][1] = 1.0f / tanHalfFOV;
	m->value[1][2] = 0;
	m->value[1][3] = 0;
	m->value[2][0] = 0;
	m->value[2][1] = 0;
	m->value[2][2] = (-znear - zfar) / zrange;
	m->value[2][3] = 2 * zfar * znear / zrange;
	m->value[3][0] = 0;
	m->value[3][1] = 0;
	m->value[3][2] = 1;
	m->value[3][3] = 0;
}

void mat4_viewport(mat4* m, int x, int y, int w, int h) {
	mat4_identity(m);
	m->value[0][3] = x + w / 2.0f;
	m->value[1][3] = y + h / 2.0f;
	m->value[2][3] = 1.0f;

	m->value[0][0] = w / 2.0f;
	m->value[1][1] = h / 2.0f;
	m->value[2][2] = 0.0f;
}

void vec2_print(vec2 v) {
	printf("< %.4f, %.4f >\n", v.x, v.y);
}

void vec3_print(vec3 v) {
	printf("< %.4f, %.4f, %.4f >\n", v.x, v.y, v.z);
}

void vec4_print(vec4 v) {
	printf("< %.4f, %.4f, %.4f, %.4f >\n", v.x, v.y, v.z, v.w);
}

void mat4_print(mat4 m) {
	for (int row = 0; row < 4; row++) {
		printf("[%.4f, %.4f, %.4f, %.4f]\n",
			m.value[row][0], m.value[row][1], m.value[row][2], m.value[row][3]
		);
	}
}

vec3 vec3_from_vec4(vec4 v) {
	vec3 r;
	r.x = v.x;
	r.y = v.y;
	r.z = v.z;
	return r;
}

vec3 vec3_div_s(vec3 a, float b) {
	vec3 r;
	r.x = a.x / b;
	r.y = a.y / b;
	r.z = a.z / b;
	return r;
}
