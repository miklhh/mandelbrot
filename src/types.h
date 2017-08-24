#ifndef _MANDELBROT_TYPES_H
#define _MANDELBROT_TYPES_H

#include <stdint.h>

/* Scaling and offset. */
struct scale_t
{
    double x;
    double y;
};

struct offset_t
{
    double x;
    double y;
};

struct color_t
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
};

#endif