#ifndef _MANDELBROT_RENDER_H
#define _MANDELBROT_RENDER_H

#include <SDL.h>
#include <complex>


using std::complex;

/* Description:
 * (1) void render_init             Initializes the render module.
 * (2) void render_mandelbrot       Renders the mandelbrot set.
 * (3) void render_draw             Draw current mandelbrot state to SDL_Renderer.
 * (4) bool render_test_complete    Returns true if image is done rendering.
 * */

void render_init(uint8_t n_threads);
void render_mandelbrot(complex<double> upper_left, complex<double> lower_right);
void render_draw(SDL_Renderer* renderer);
bool render_test_complete();


#endif