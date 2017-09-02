#ifndef _MANDELBROT_SUPERSAMPLING_H
#define _MANDELBROT_SUPERSAMPLING_H

#include "types.h"

/* Get a 4x supersampled pixelcolor. */
rgb_t get_px_1x_ss(int px_x, int px_y, scale_t scale, offset_t offset);
rgb_t get_px_4x_ss(int px_x, int px_y, scale_t scale, offset_t offset);
rgb_t get_px_8x_ss(int px_x, int px_y, scale_t scale, offset_t offset);

#endif