#ifndef _MANDELBROT_COLOR_H
#define _MANDELBROT_COLOR_H

#include <complex>
#include <stdint.h>
#include <SDL.h>

#include "types.h"

/* Get color based on how many iterations have been calculated and value on the 
 * last Z. */
color_t get_color(
    uint32_t iterations,
    std::complex<double> zn);

/* Coloring functions for red, green and blue.*/
uint8_t color_r(uint32_t n, std::complex<double> zn);
uint8_t color_g(uint32_t n, std::complex<double> zn);
uint8_t color_b(uint32_t n, std::complex<double> zn);

#endif