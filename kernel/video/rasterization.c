#include "video/rasterization.h"

float near = 1;
float aspect_ratio = SCREEN_WIDTH / (float) SCREEN_HEIGHT;

point3_t vertices[3] = {
    {0, 0, -1},
    {2, 0, -1},
    {1, 2, -1}
};

vector2_t projected_vertices[3] = {
    {0, 0},
    {0, 0},
    {0, 0}
};

void render_image() {
    _fill_screen(BLUE);
    // Projection to canvas
    for(size_t i = 0; i < sizeof(vertices) / sizeof(point3_t); i++) {
        point2_t new_point = {0, 0};
        // Perspective divide
        new_point.x = -near * vertices[i].x / vertices[i].z;
        new_point.y = -near * vertices[i].y / vertices[i].z;
        new_point.y *= aspect_ratio;

        // Transform to NDC (range [-1:1] -> [0:1])
        new_point.x = (new_point.x + 1) / 2;
        new_point.y = (new_point.y + 1) / 2;

        new_point.x *= SCREEN_WIDTH;
        new_point.y *= SCREEN_HEIGHT;

        //TODO: do not round up/down projected vertices but only the bounding box coordinates
        projected_vertices[i].x = (float) (uint32_t) new_point.x;
        projected_vertices[i].y = (float) (uint32_t) new_point.y;
        if(new_point.x - projected_vertices[i].x > 0.5)
            projected_vertices[i].x += 1;
        if(new_point.y - projected_vertices[i].x > 0.5)
            projected_vertices[i].y += 1;
    }

    projected_vertices[0] = (vector2_t) {SCREEN_WIDTH / 2, 0};
    projected_vertices[1] = (vector2_t) {100, SCREEN_HEIGHT / 2};
    projected_vertices[2] = (vector2_t) {SCREEN_WIDTH - 100, SCREEN_HEIGHT / 2};

    // Compute Bounding box
    uint32_t max_x = projected_vertices[0].x, min_x = projected_vertices[0].x;
    uint32_t max_y = projected_vertices[0].x, min_y = projected_vertices[0].y;
    for(size_t i = 1; i < 3; i++) {
        max_x = projected_vertices[i].x > max_x ? projected_vertices[i].x : max_x;
        min_x = projected_vertices[i].x < min_x ? projected_vertices[i].x : min_x;
        max_y = projected_vertices[i].y > max_y ? projected_vertices[i].y : max_y;
        min_y = projected_vertices[i].y < min_y ? projected_vertices[i].y : min_y;
    }

    //TODO: might need to do some ordering of triangle vertices for them to be counterclockwise
    // Compute pixels inside triangle
    // Steps:
    // - Compute vectors between vertices (V1V2, V2V3, V3V1)
    // - Set position to middle of pixel
    // - Compute dot product for: V1P x V1V2, V2P x V2V3, V3P x V3V1
    //    - If all have the same sign (or are 0), the point is inside, otherwise outside
    //    - Results in computations:
    //       - V1P x V1V2 = (p.x - v1.x) * (v2.y - v1.y) - (p.y - v1.y) * (v2.x - v1.x) = (p.x - v1.x) * a1 - (p.y - v1.y) * b1
    //       - V2P x V2V3 = (p.x - v2.x) * (v3.y - v2.y) - (p.y - v2.y) * (v3.x - v2.x) = (p.x - v2.x) * a2 - (p.y - v2.y) * b2
    //       - V3P x V3V1 = (p.x - v3.x) * (v1.y - v3.y) - (p.y - v3.y) * (v1.x - v3.x) = (p.x - v3.x) * a3 - (p.y - v3.y) * b3
    //    - If p.x increases, the whole result increases by a
    //    - If p.y increases, the whole result increases by b
    vector2_t v1v2 = vector_sub(&projected_vertices[1], &projected_vertices[0]);
    vector2_t v2v3 = vector_sub(&projected_vertices[2], &projected_vertices[1]);
    vector2_t v3v1 = vector_sub(&projected_vertices[0], &projected_vertices[2]);
    vector2_t p = { min_x + 0.5f, min_y + 0.5f };
    vector2_t v1p = vector_sub(&p, &projected_vertices[0]);
    vector2_t v2p = vector_sub(&p, &projected_vertices[1]);
    vector2_t v3p = vector_sub(&p, &projected_vertices[2]);
    float start_e1 = cross_product(&v1p, &v1v2);
    float start_e2 = cross_product(&v2p, &v2v3);
    float start_e3 = cross_product(&v3p, &v3v1);
    size_t y_length = (max_y - min_y + 1);
    size_t x_length = (max_x - min_x + 1);
    for(size_t i = 0; i < y_length; i++) {
        float e1 = start_e1;
        float e2 = start_e2;
        float e3 = start_e3;
        for(size_t j = 0; j < x_length; j++) {
            if(e1 >= -EPSILON && e2 >= -EPSILON && e3 >= -EPSILON) {
                _set_pixel(min_x + j, min_y + i, YELLOW);
            }

            e1 += v1v2.y;
            e2 += v2v3.y;
            e3 += v3v1.y;
        }
        start_e1 -= v1v2.x;
        start_e2 -= v2v3.x;
        start_e3 -= v3v1.x;
    }
}