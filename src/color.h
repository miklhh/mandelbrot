#ifndef _MANDELBROT_COLOR_H
#define _MANDELBROT_COLOR_H

#include <complex>

#include "types.h"

/* Get the propriete color for a pixel. */
rgb_t get_color(int iterations, std::complex<double> zn, std::complex<double> c);

#endif