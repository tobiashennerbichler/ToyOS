#ifndef __VECTOR_H__
#define __VECTOR_H__

// TODO: likely libc later

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

vector2_t vector_sub(vector2_t *v1, vector2_t *v2);
float dot_product(vector2_t *v1, vector2_t *v2);
float cross_product(vector2_t *v1, vector2_t *v2);

#endif