#ifndef VECMATH_H
#define VECMATH_H

#include <math.h>
#include <stdlib.h>

typedef struct vm_vec2 {
	union {
		float v[2];
		struct { float x, y; };
	};
} vec2;

typedef struct vm_vec3 {
	union {
		float v[3];
		struct { float x, y, z; };
	};
} vec3;

typedef struct vm_vec4 {
	union {
		float v[4];
		struct { float x, y, z, w; };
	};
} vec4;

typedef struct vm_mat4 {
	float value[4][4];
} mat4;

// VEC2
vec2 vec2_add(vec2 a, vec2 b);
vec2 vec2_sub(vec2 a, vec2 b);
vec2 vec2_mul_v(vec2 a, vec2 b);
vec2 vec2_mul_s(vec2 a, float b);
float vec2_dot(vec2 a, vec2 b);
float vec2_cross(vec2 a, vec2 b);
float vec2_length(vec2 v);
vec2 vec2_normalize(vec2 v);

// VEC3
vec3 vec3_add(vec3 a, vec3 b);
vec3 vec3_sub(vec3 a, vec3 b);
vec3 vec3_mul_v(vec3 a, vec3 b);
vec3 vec3_mul_s(vec3 a, float b);
vec3 vec3_div_s(vec3 a, float b);
float vec3_dot(vec3 a, vec3 b);
vec3 vec3_cross(vec3 a, vec3 b);
float vec3_length(vec3 v);
vec3 vec3_normalize(vec3 v);
vec3 vec3_from_vec4(vec4 v);

// VEC4
vec4 vec4_perspective_divide(vec4 v);
vec4 vec4_negate(vec4 v);

// MAT4
float mat4_get(mat4 m, int row, int col);
void mat4_set(mat4* m, int row, int col, float v);
void mat4_set_index(mat4* m, int index, float v);

void mat4_identity(mat4* m);
void mat4_translation(mat4* m, vec3 v);
void mat4_translate(mat4* m, vec3 v);
void mat4_scaling(mat4* m, vec3 v);
void mat4_rotation(mat4* m, vec3 axis, float a);
void mat4_perspective(mat4* m, float fov, float aspect, float znear, float zfar);
void mat4_viewport(mat4* m, int x, int y, int w, int h);
void mat4_lookat(mat4* m, vec3 eye, vec3 center, vec3 up);
void mat4_cancel_translation(mat4* m);
vec4 mat4_get_row(mat4 m, int index);
vec4 mat4_get_column(mat4 m, int index);

mat4 mat4_mul_m(mat4 a, mat4 b);
vec3 mat4_mul_v(mat4 a, vec3 b, float w);
vec4 mat4_mul_v4(mat4 a, vec4 b);

void vec2_print(vec2 v);
void vec3_print(vec3 v);
void vec4_print(vec4 v);
void mat4_print(mat4 m);

#define Mxy(row, col) col + row * 4
///<    MYX
#define M00 Mxy(0, 0)
#define M10 Mxy(1, 0)
#define M20 Mxy(2, 0)
#define M30 Mxy(3, 0)
#define M01 Mxy(0, 1)
#define M11 Mxy(1, 1)
#define M21 Mxy(2, 1)
#define M31 Mxy(3, 1)
#define M02 Mxy(0, 2)
#define M12 Mxy(1, 2)
#define M22 Mxy(2, 2)
#define M32 Mxy(3, 2)
#define M03 Mxy(0, 3)
#define M13 Mxy(1, 3)
#define M23 Mxy(2, 3)
#define M33 Mxy(3, 3)

#define deg(x) x * 180.0f / M_PI
#define rad(x) x * M_PI / 180.0f

#define max(x, y) (((x) > (y)) ? (x) : (y))
#define min(x, y) (((x) < (y)) ? (x) : (y))
#define randrange(lower, upper) ((rand() % (upper + 1 - lower)) + lower)
#define randrangef(lower, upper) (lower + (rand() / (float) RAND_MAX) * (upper - lower))
#define index_get(x, y, width) (int)(x + y * width)

#endif // VECMATH_H
