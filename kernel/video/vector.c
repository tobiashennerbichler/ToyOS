#include "video/vector.h"

vector2_t vector_sub(vector2_t *v1, vector2_t *v2) {
    return (vector2_t) {
        .x = v1->x - v2->x,
        .y = v1->y - v2->y
    };
}

float dot_product(vector2_t *v1, vector2_t *v2) {
    return v1->x * v2->x + v1->y * v2->y;
}

float cross_product(vector2_t *v1, vector2_t *v2) {
    return v1->x * v2->y - v1->y * v2->x;
}