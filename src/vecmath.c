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
	r.x = _v.x;
	r.y = _v.y;
	r.z = _v.z;
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

void mat4_translate(mat4* m, vec3 v) {
	m->value[0][3] += v.x;
	m->value[1][3] += v.y;
	m->value[2][3] += v.z;
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
	float hw = w / 2.0f;
	float hh = h / 2.0f;

	mat4_identity(m);
	m->value[0][3] = x + hw;
	m->value[1][3] = y + hh;
	m->value[0][0] = hw;
	m->value[1][1] = -hh;
}

void mat4_lookat(mat4* m, vec3 eye, vec3 center, vec3 up) {
	vec3 z = vec3_normalize(vec3_sub(eye, center));
	vec3 x = vec3_normalize(vec3_cross(up, z));
	vec3 y = vec3_normalize(vec3_cross(z, x));
	mat4 minv, tr;
	mat4_identity(&minv);
	mat4_identity(&tr);

	for (int i = 0; i < 3; i++) {
		minv.value[0][i] = x.v[i];
		minv.value[1][i] = y.v[i];
		minv.value[2][i] = z.v[i];
		tr.value[i][3] = -center.v[i];
	}
	*m = mat4_mul_m(minv, tr);
}

void mat4_cancel_translation(mat4* m) {
	m->value[0][3] = 0;
	m->value[1][3] = 0;
	m->value[2][3] = 0;
	m->value[3][3] = 1;
	m->value[3][0] = 0;
	m->value[3][1] = 0;
	m->value[3][2] = 0;
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

vec4 vec4_perspective_divide(vec4 v) {
	vec4 r;
	r.x = v.x / v.w;
	r.y = v.y / v.w;
	r.z = v.z / v.w;
	r.w = v.w;
	return r;
}

vec4 mat4_get_row(mat4 m, int index) {
	vec4 r;
	r.x = m.value[index][0];
	r.y = m.value[index][1];
	r.z = m.value[index][2];
	r.w = m.value[index][3];
	return r;
}

vec4 mat4_get_column(mat4 m, int index) {
	vec4 r;
	r.x = m.value[0][index];
	r.y = m.value[1][index];
	r.z = m.value[2][index];
	r.w = m.value[3][index];
	return r;
}

vec4 vec4_negate(vec4 v) {
	return ctor(vec4, -v.x, -v.y, -v.z, -v.w);
}

mat4 mat4_transpose(mat4 m) {
	mat4 r;
	r.value[0][0] = m.value[0][0];
	r.value[0][1] = m.value[1][0];
	r.value[0][2] = m.value[2][0];
	r.value[0][3] = m.value[3][0];

	r.value[1][0] = m.value[0][1];
	r.value[1][1] = m.value[1][1];
	r.value[1][2] = m.value[2][1];
	r.value[1][3] = m.value[3][1];

	r.value[2][0] = m.value[0][2];
	r.value[2][1] = m.value[1][2];
	r.value[2][2] = m.value[2][2];
	r.value[2][3] = m.value[3][2];

	r.value[3][0] = m.value[0][3];
	r.value[3][1] = m.value[1][3];
	r.value[3][2] = m.value[2][3];
	r.value[3][3] = m.value[3][3];
	return r;
}

mat4 mat4_invert(mat4 m) {
	mat4 r, result;
	float tmp[12];
	float src[16];
	float det;
	for (int i = 0; i < 4; i++) {
		src[i + 0 ] = m.value[i][0];
		src[i + 4 ] = m.value[i][1];
		src[i + 8 ] = m.value[i][2];
		src[i + 12] = m.value[i][3];
	}

	tmp[0] = src[10] * src[15];
	tmp[1] = src[11] * src[14];
	tmp[2] = src[9] * src[15];
	tmp[3] = src[11] * src[13];
	tmp[4] = src[9] * src[14];
	tmp[5] = src[10] * src[13];
	tmp[6] = src[8] * src[15];
	tmp[7] = src[11] * src[12];
	tmp[8] = src[8] * src[14];
	tmp[9] = src[10] * src[12];
	tmp[10] = src[8] * src[13];
	tmp[11] = src[9] * src[12];

	r.value[0][0] = tmp[0]*src[5] + tmp[3]*src[6] + tmp[4]*src[7];
	r.value[0][0] -= tmp[1]*src[5] + tmp[2]*src[6] + tmp[5]*src[7];
	r.value[0][1] = tmp[1]*src[4] + tmp[6]*src[6] + tmp[9]*src[7];
	r.value[0][1] -= tmp[0]*src[4] + tmp[7]*src[6] + tmp[8]*src[7];
	r.value[0][2] = tmp[2]*src[4] + tmp[7]*src[5] + tmp[10]*src[7];
	r.value[0][2] -= tmp[3]*src[4] + tmp[6]*src[5] + tmp[11]*src[7];
	r.value[0][3] = tmp[5]*src[4] + tmp[8]*src[5] + tmp[11]*src[6];
	r.value[0][3] -= tmp[4]*src[4] + tmp[9]*src[5] + tmp[10]*src[6];
	r.value[1][0] = tmp[1]*src[1] + tmp[2]*src[2] + tmp[5]*src[3];
	r.value[1][0] -= tmp[0]*src[1] + tmp[3]*src[2] + tmp[4]*src[3];
	r.value[1][1] = tmp[0]*src[0] + tmp[7]*src[2] + tmp[8]*src[3];
	r.value[1][1] -= tmp[1]*src[0] + tmp[6]*src[2] + tmp[9]*src[3];
	r.value[1][2] = tmp[3]*src[0] + tmp[6]*src[1] + tmp[11]*src[3];
	r.value[1][2] -= tmp[2]*src[0] + tmp[7]*src[1] + tmp[10]*src[3];
	r.value[1][3] = tmp[4]*src[0] + tmp[9]*src[1] + tmp[10]*src[2];
	r.value[1][3] -= tmp[5]*src[0] + tmp[8]*src[1] + tmp[11]*src[2];

	tmp[0] = src[2]*src[7];
	tmp[1] = src[3]*src[6];
	tmp[2] = src[1]*src[7];
	tmp[3] = src[3]*src[5];
	tmp[4] = src[1]*src[6];
	tmp[5] = src[2]*src[5];

	tmp[6] = src[0]*src[7];
	tmp[7] = src[3]*src[4];
	tmp[8] = src[0]*src[6];
	tmp[9] = src[2]*src[4];
	tmp[10] = src[0]*src[5];
	tmp[11] = src[1]*src[4];

	r.value[2][0] = tmp[0]*src[13] + tmp[3]*src[14] + tmp[4]*src[15];
	r.value[2][0] -= tmp[1]*src[13] + tmp[2]*src[14] + tmp[5]*src[15];
	r.value[2][1] = tmp[1]*src[12] + tmp[6]*src[14] + tmp[9]*src[15];
	r.value[2][1] -= tmp[0]*src[12] + tmp[7]*src[14] + tmp[8]*src[15];
	r.value[2][2] = tmp[2]*src[12] + tmp[7]*src[13] + tmp[10]*src[15];
	r.value[2][2] -= tmp[3]*src[12] + tmp[6]*src[13] + tmp[11]*src[15];
	r.value[2][3] = tmp[5]*src[12] + tmp[8]*src[13] + tmp[11]*src[14];
	r.value[2][3] -= tmp[4]*src[12] + tmp[9]*src[13] + tmp[10]*src[14];
	r.value[3][0] = tmp[2]*src[10] + tmp[5]*src[11] + tmp[1]*src[9];
	r.value[3][0] -= tmp[4]*src[11] + tmp[0]*src[9] + tmp[3]*src[10];
	r.value[3][1] = tmp[8]*src[11] + tmp[0]*src[8] + tmp[7]*src[10];
	r.value[3][1] -= tmp[6]*src[10] + tmp[9]*src[11] + tmp[1]*src[8];
	r.value[3][2] = tmp[6]*src[9] + tmp[11]*src[11] + tmp[3]*src[8];
	r.value[3][2] -= tmp[10]*src[11] + tmp[2]*src[8] + tmp[7]*src[9];
	r.value[3][3] = tmp[10]*src[10] + tmp[4]*src[8] + tmp[9]*src[9];
	r.value[3][3] -= tmp[8]*src[9] + tmp[11]*src[10] + tmp[5]*src[8];

	det = src[0]*r.value[0][0]+src[1]*r.value[0][1]+src[2]*r.value[0][2]+src[3]*r.value[0][3];
	det = 1.0f / det;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.value[i][j] = r.value[i][j] * det;
		}
	}
	return result;
}

vec2 vec2_rotate(vec2 p, float a) {
	float c = cos(a);
	float s = sin(a);

	vec2 r;
	r.x = (c * p.x) - (s * p.y);
	r.y = (s * p.x) + (c * p.y);
	return r;
}
