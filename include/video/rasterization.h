#ifndef __RASTERIZATION_H__
#define __RASTERIZATION_H__

#include "video/graphics.h"
#include "libc/vector.h"

void render_image();
static void fill_triangle(triangle2_t *proj_t);
static bbox_t get_bounding_box(triangle2_t *t);

#endif