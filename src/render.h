#ifndef _MANDELBROT_RENDER_H
#define _MANDELBROT_RENDER_H

#include <SDL.h>
#include <complex>


using std::complex;

/* Description:
 * (1) void render_init                 Initializes the render module.
 * (2) void render_mandelbrot           Renders the mandelbrot set.
 * (3) void render_draw_to_SDL_Renderer Draw mandelbrot set to SDL_Renderer.
 * (4) bool render_test_complete        Returns true if image is done rendering.
 * (5) int  render_create_bmp           Drop a bitmap to computer. Returns 0 on success.
 * */

void render_init(int n_threads);
void render_mandelbrot(complex<double> upper_left, complex<double> lower_right);
void render_draw_to_SDL_Renderer(SDL_Renderer* renderer);
bool render_test_complete();
int  render_create_bmp(char* file_name);


#endif