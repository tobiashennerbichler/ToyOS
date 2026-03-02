#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <stdint.h>
#include <stddef.h>

#define EPSILON 0.001f

typedef struct vector2_t {
    float x;
    float y;
} vector2_t;
typedef vector2_t point2_t;

typedef struct vector3_t {
    float x;
    float y;
    float z;
} vector3_t;
typedef vector3_t point3_t;

typedef struct matrix3x3_t {
    vector3_t rows[3];
} matrix3x3_t;

typedef struct triangle2_t {
    point2_t vertices[3];
} triangle2_t;

typedef struct triangle3_t {
    point3_t vertices[3];
} triangle3_t;

typedef struct bbox_t {
    uint16_t max_x;
    uint16_t min_x;
    uint16_t max_y;
    uint16_t min_y;
} bbox_t;

vector2_t vector_sub(vector2_t *v1, vector2_t *v2);
float dot_product(vector2_t *v1, vector2_t *v2);
float cross_product(vector2_t *v1, vector2_t *v2);
uint16_t round_float_to_uint16(float f);

#endif