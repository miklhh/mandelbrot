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
    uint32_t red : 8;
    uint32_t green : 8;
    uint32_t blue : 8;
    uint32_t alpha : 8;
};

#endif