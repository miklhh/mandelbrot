/*
 * Types used throughout the program.
 */

#ifndef _MANDELBROT_TYPES_H
#define _MANDELBROT_TYPES_H

#include <stdint.h>

/* Scale data type. */
struct scale_t
{
    double x;
    double y;
};

/* Offset data type. */
struct offset_t
{
    double x;
    double y;
};

/* Red-Green-Blue data type. */
struct rgb_t
{
    uint32_t red : 8;
    uint32_t green : 8;
    uint32_t blue : 8;
    uint32_t alpha : 8;
};

/* Hue-Saturation-Light data type. */
struct hsl_t
{
    double hue;         // 'Hue' mesuared in radians [0, 2*pi].
    double saturation;  // 'Saturation' [0, 1].
    double light;       // 'Light' [0, 1].
};

/* Pixel segment sata-type. */
struct segment_t
{
    int x_bgn;
    int y_bgn;
    int x_end;
    int y_end;
};

#endif