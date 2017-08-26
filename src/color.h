#ifndef _MANDELBROT_COLOR_H
#define _MANDELBROT_COLOR_H

#include <complex>
#include <stdint.h>
#include <SDL.h>

#include "types.h"

/* Get the propriete color for a pixel. */
color_t get_color(uint32_t iterations, std::complex<double> zn);


#endif