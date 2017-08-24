#ifndef _MANDELBROT_RENDER_H
#define _MANDELBROT_RENDER_H

#include <SDL.h>
#include <complex>

extern const int scr_width;
extern const int scr_height;


using std::complex;

/* Description:
 * (1) render_init          Initializes the render module.
 * (2) render_mandelbrot    Renders the mandelbrot set.
 * (3) render_draw          Draw current mandelbrot state to SDL_Renderer.
 * */

void render_init(uint8_t n_threads);
void render_mandelbrot(complex<double> upper_left, complex<double> lower_right);
void render_draw(SDL_Renderer* renderer);


#endif