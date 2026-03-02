#include "video/rasterization.h"

float near = 1;
float aspect_ratio = SCREEN_WIDTH / (float) SCREEN_HEIGHT;

triangle3_t triangles[] = {
    {{
        {12.75951899999, -2.470218, 19.806297},
        {6.758076, 32.25783899999, 31.478708},
        {14.939601, -42.525517, 22.585014}
    }},
    //{{
    //    {-23.18206599, 16.7943649999, 4.5449619999},
    //    {2.4220649999, 26.974043, 31.788353},
    //    {2.19909, 28.224001, 32.760967}
    //}}
};

bbox_t get_bounding_box(triangle2_t *t) {
    bbox_t bb = {
        .max_x = 0,
        .min_x = SCREEN_WIDTH - 1,
        .max_y = 0,
        .min_y = SCREEN_HEIGHT - 1
    };
    for(size_t i = 0; i < 3; i++) {
        // Bound to [0, SCREEN_DIM - 1]
        float bound_x = t->vertices[i].x > (SCREEN_WIDTH - 1 - EPSILON) ? (float) (SCREEN_WIDTH - 1) :
                        t->vertices[i].x < EPSILON ? 0.0f : t->vertices[i].x;
        float bound_y = t->vertices[i].y > (SCREEN_HEIGHT - 1 - EPSILON) ? (float) (SCREEN_HEIGHT - 1) :
                        t->vertices[i].y < EPSILON ? 0.0f : t->vertices[i].y;
        uint16_t x = (uint16_t) bound_x;
        uint16_t y = (uint16_t) bound_y;
        x = (bound_x - x) > 0.5f ? x + 1 : x;
        y = (bound_y - y) > 0.5f ? y + 1 : y;
        bb.max_x = x > bb.max_x ? x : bb.max_x;
        bb.min_x = x < bb.min_x ? x : bb.min_x;
        bb.max_y = y > bb.max_y ? y : bb.max_y;
        bb.min_y = y < bb.min_y ? y : bb.min_y;
    }
    return bb;
}

// NOTE: ASSUMES THAT VERTICES ARE ORIENTED COUNTER-CLOCKWISE
// For each point in the bounding box spanned by the triangle, compute if the pixel is inside it (compute with center of pixel)
// Requires computing cross product of each edge vector to each point vector
// As optimization only compute cross product once and then increment linearly from there
static void fill_triangle(triangle2_t *proj_t) {
    bbox_t bb = get_bounding_box(proj_t);
    // Triangle lies completely outside of screen
    if(bb.max_x == 0 || bb.min_x == SCREEN_WIDTH - 1 ||
       bb.max_y == 0 || bb.min_y == SCREEN_HEIGHT - 1) {
        return;
    }

    vector2_t v1v2 = vector_sub(&proj_t->vertices[1], &proj_t->vertices[0]);
    vector2_t v2v3 = vector_sub(&proj_t->vertices[2], &proj_t->vertices[1]);
    vector2_t v3v1 = vector_sub(&proj_t->vertices[0], &proj_t->vertices[2]);
    vector2_t p = { bb.min_x + 0.5f, bb.min_y + 0.5f };
    vector2_t v1p = vector_sub(&p, &proj_t->vertices[0]);
    vector2_t v2p = vector_sub(&p, &proj_t->vertices[1]);
    vector2_t v3p = vector_sub(&p, &proj_t->vertices[2]);

    // V1V2 x V1P = (v2.x - v1.x) * (p.y - v1.y) - (v2.y - v1.y) * (p.x - v1.x)
    float start_e1 = cross_product(&v1v2, &v1p);
    float start_e2 = cross_product(&v2v3, &v2p);
    float start_e3 = cross_product(&v3v1, &v3p);
    for(uint16_t y = bb.min_y; y < bb.max_y + 1; y++) {
        float e1 = start_e1;
        float e2 = start_e2;
        float e3 = start_e3;
        for(uint16_t x = bb.min_x; x < bb.max_x + 1; x++) {
            if(e1 > -EPSILON && e2 > -EPSILON && e3 > -EPSILON) {
                _set_pixel(x, y, YELLOW);
            }

            e1 -= v1v2.y;
            e2 -= v2v3.y;
            e3 -= v3v1.y;
        }
        start_e1 += v1v2.x;
        start_e2 += v2v3.x;
        start_e3 += v3v1.x;
    }
}

void render_image() {
    fill_screen(BLUE);
    // Projection to canvas
    for(size_t ti = 0; ti < sizeof(triangles) / sizeof(triangle3_t); ti++) {
        triangle2_t projected_triangle = {0};
        for(size_t vi = 0; vi < sizeof(triangles[ti].vertices) / sizeof(point3_t); vi++) {
            // Perspective divide
            projected_triangle.vertices[vi].x = -near * triangles[ti].vertices[vi].x / triangles[ti].vertices[vi].z;
            projected_triangle.vertices[vi].y = -near * triangles[ti].vertices[vi].y / triangles[ti].vertices[vi].z;
            projected_triangle.vertices[vi].y *= aspect_ratio;

            // Transform to NDC (range [-1:1] -> [0:1])
            projected_triangle.vertices[vi].x = (projected_triangle.vertices[vi].x + 1) / 2;
            projected_triangle.vertices[vi].y = (projected_triangle.vertices[vi].y + 1) / 2;

            // Adjust to screen size
            projected_triangle.vertices[vi].x *= SCREEN_WIDTH;
            projected_triangle.vertices[vi].y *= SCREEN_HEIGHT;
        }

        fill_triangle(&projected_triangle);
    }
}