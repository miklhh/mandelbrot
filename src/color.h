/*
 * Functions for handling coloring of the mandelbrot fractal. 
 *
 * Function explanations:
 *
 * (1) rgb_t get_color(
 *              int iterations, 
 *              const std::complex<double> & zn, 
 *              const std::complex<double> & c);
 *     --
 *     Function for getting the propriate color for a pixel. 'Iterations'
 *     should be the number of iterations requiered to escape the escape
 *     boundry. 'Zn' should be the the complex number that escaped the
 *     boundry when the complex number c is the start number in the set.
 *     --
 */

#ifndef _MANDELBROT_COLOR_H
#define _MANDELBROT_COLOR_H

#include <complex>

#include "types.h"

/* Get the propriete color for a pixel. */
rgb_t get_color(
    int iterations, 
    const std::complex<double> & zn, 
    const std::complex<double> & c);

#endif