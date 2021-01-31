/*
 * linmath_additional.h
 *
 *  Created on: Aug 7, 2020
 *      Author: dizcza
 */

#ifndef LINMATH_H_LINMATH_ADDITIONAL_H_
#define LINMATH_H_LINMATH_ADDITIONAL_H_

#include "linmath.h"

#define LINMATH_MIN(x,y)                  ((x) < (y) ? (x) : (y))
#define LINMATH_MAX(x,y)                  ((x) > (y) ? (x) : (y))
#define LINMATH_CLAMP(x,a,b)              ((x) < (a) ? (a) : ((x) > (b) ? (b) : (x)))
#define LINMATH_DEGREES_TO_RADS(degrees)  (M_PI * (degrees) / 180.f)
#define LINMATH_RADS_TO_DEGREES(rads)     (180.f * (rads) / M_PI)


#define LINMATH_ADDITIONAL_MACROS_VEC(n) \
LINMATH_H_FUNC void vec##n##_dup(vec##n r, vec##n const src) \
{ \
	int i; \
	for(i=0; i<n; ++i) \
		r[i] = src[i]; \
} \
LINMATH_H_FUNC void vec##n##_interpolate(vec##n *buffer, uint32_t cnt, vec##n const from, vec##n const to) \
{ \
	vec##n inc; \
	vec##n##_sub(inc, to, from); \
	vec##n##_scale(inc, inc, 1.f / (cnt - 1)); \
	vec##n##_dup(buffer[0], from); \
	int i; \
	for (i = 1; i < cnt; ++i) { \
		vec##n##_add(buffer[i], buffer[i - 1], inc); \
	} \
} \
LINMATH_H_FUNC void vec##n##_pos_inf(vec##n v) { \
	int i; \
	for (i = 0; i < n; ++i) { \
		v[i] = FLT_MAX; \
	} \
} \
LINMATH_H_FUNC void vec##n##_neg_inf(vec##n v) { \
	int i; \
	for (i = 0; i < n; ++i) { \
		v[i] = -FLT_MAX; \
	} \
} \
LINMATH_H_FUNC int vec##n##_all_pos(vec##n v) { \
	int i; \
	int res = 1; \
	for (i = 0; i < n; ++i) { \
		res &= v[i] >= 0; \
	} \
	return res; \
} \
LINMATH_H_FUNC int vec##n##_all_neg(vec##n v) { \
	int i; \
	int res = 1; \
	for (i = 0; i < n; ++i) { \
		res &= v[i] <= 0; \
	} \
	return res; \
}

LINMATH_ADDITIONAL_MACROS_VEC(2)
LINMATH_ADDITIONAL_MACROS_VEC(3)
LINMATH_ADDITIONAL_MACROS_VEC(4)


LINMATH_H_FUNC void vec3_set(vec3 r, float x, float y, float z) {
	r[0] = x;
	r[1] = y;
	r[2] = z;
}

LINMATH_H_FUNC void vec4_set(vec4 r, float x, float y, float z, float w) {
	r[0] = x;
	r[1] = y;
	r[2] = z;
	r[3] = w;
}



# define LINMATH_H_DEFINE_VECI(n,pref,b) \
typedef pref##int##b##_t vec##n##pref##int##b[n]; \
LINMATH_H_FUNC void vec##n##pref##int##b##_floor(vec##n##pref##int##b vint, vec##n const vf) { \
	int i; \
	for (i = 0; i < n; ++i) { \
		vint[i] = (int##b##_t) floorf(vf[i]); \
	} \
} \
LINMATH_H_FUNC void vec##n##pref##int##b##_ceil(vec##n##pref##int##b vint, vec##n const vf) { \
	int i; \
	for (i = 0; i < n; ++i) { \
		vint[i] = (int##b##_t) ceilf(vf[i]); \
	} \
} \
LINMATH_H_FUNC void vec##n##pref##int##b##_from_vec##n(vec##n##pref##int##b vint, vec##n const vf) { \
	int i; \
	for (i = 0; i < n; ++i) { \
		vint[i] = (int##b##_t) vf[i]; \
	} \
} \
LINMATH_H_FUNC void vec##n##_from_vec##n##pref##int##b(vec##n vf, vec##n##pref##int##b const vint) { \
	int i; \
	for (i = 0; i < n; ++i) { \
		vf[i] = (float) vint[i]; \
	} \
}

LINMATH_H_DEFINE_VECI(2,,16);
LINMATH_H_DEFINE_VECI(3,,16);
LINMATH_H_DEFINE_VECI(4,,16);

LINMATH_H_DEFINE_VECI(2,u,32);
LINMATH_H_DEFINE_VECI(3,u,32);
LINMATH_H_DEFINE_VECI(4,u,32);



/**
 * Matrices are stored in column order.
 */
#define LINMATH_H_DEFINE_MAT(n) \
typedef vec##n mat##n##x##n[n]; \
LINMATH_H_FUNC void mat##n##x##n##_mul_vec##n(vec##n r, mat##n##x##n const M, vec##n const v) { \
	vec##n r_temp; \
	int i, j; \
	for (j = 0; j < n; ++j) { \
		r_temp[j] = 0.f; \
		for (i = 0; i < n; ++i) \
			r_temp[j] += M[i][j] * v[i]; \
	} \
	vec##n##_dup(r, r_temp); \
} \
LINMATH_H_FUNC void mat##n##x##n##_set_col(mat##n##x##n M, vec##n const v, int col_index) { \
	int i; \
	for (i = 0; i < n; ++i) { \
		M[col_index][i] = v[i]; \
	} \
} \
LINMATH_H_FUNC void mat##n##x##n##_set_row(mat##n##x##n M, vec##n const v, int row_index) { \
	int j; \
	for (j = 0; j < n; ++j) { \
		M[j][row_index] = v[j]; \
	} \
}

LINMATH_H_DEFINE_MAT(3);
//LINMATH_H_DEFINE_MAT(4);

LINMATH_H_FUNC void mat4x4_translate_gl(mat4x4 M, float x, float y,
		float z) {
	M[3][0] += x;
	M[3][1] += y;
	M[3][2] += z;
}



#endif /* LINMATH_H_LINMATH_ADDITIONAL_H_ */
