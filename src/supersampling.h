/*
 * Functions for handling supersampling.
 *
 * Function explanations:
 *
 * (1) void supersample_init(const scale_t & scale, const offset_t & offset);
 *     -- 
 *     Function for setting up the supersampler. 'Scale' is the scale of
 *     the mandelbrot set that should be rendered and 'offset' is the offset
 *     from origo (0 + 0i).
 *     --
 *
 * (2) rgb_t get_px_1x_ss(int px_x, int px_y, scale_t scale, offset_t offset);
 *     --
 *     Function for getting the 1-times supersampled color of a pixel.
 *     --
 */


#ifndef _MANDELBROT_SUPERSAMPLING_H
#define _MANDELBROT_SUPERSAMPLING_H

#include "types.h"

/* Get a 4x supersampled pixelcolor. */
void supersample_init(const scale_t & scale, const offset_t & offset);
rgb_t get_px_1x_ss(int px_x, int px_y);
rgb_t get_px_4x_ss(int px_x, int px_y);
rgb_t get_px_8x_ss(int px_x, int px_y);

#endif