/*
 * Renderer module used to render the mandelbrot set. This module defines the
 * interface in which the mandelbrot is drawn, and the main program should
 * interact with the 'mandelbrot drawer' through this module only.
 *
 * Function explanations:
 *
 * (1) void render_init(int n_threads);
 *     --
 *     Function for initializing the render module. The render module should
 *     only be initialized once. Parameter 'n_threads' specifies how many
 *     threads the render module may use.
 *     --
 *
 * (2) void render_mandelbrot(
 *              std::complex<double> upper_left,
 *              std::complex<double> lower_right);
 *     --
 *     Renders the mandebrot set. Parameter 'upper_left' represents the upper
 *     left coordinate that should be rendered and parameter 'lower_right'
 *     vice versa.
 *     --
 *
 * (3) void render_draw_to_SDL_Renderer(SDL_Renderer* renderer);
 *     --
 *     Draws the image to the SDL Renderer pointer to by 'renderer'.
 *     --
 *  
 * (4) bool render_test_complete();
 *     --
 *     Returns true if the rendering is completed.
 *     --
 *
 * (5) int render_create_bmp(char* file_name);
 *     --
 *     Create a bitmap file on the local machine. Parameter 'file_name' is the
 *     decides the file name of the image.
 *     --
 *
 * (6) void render_destroy()
 *     --
 *     Destroy the render module and retun all of its resources.
 *     --
 */

#ifndef _MANDELBROT_RENDER_H
#define _MANDELBROT_RENDER_H

#include <SDL2/SDL.h>
#include <complex>

#include "types.h"

void render_init(int n_threads);
void render_mandelbrot(
    const std::complex<double> & point,
    const scale_t & scale);
void render_mandelbrot(
    const std::complex<double> & upper_left, 
    const std::complex<double> & lower_right);
void render_draw_to_SDL_Renderer(SDL_Renderer* renderer);
bool render_test_complete();
int  render_create_bmp(const char* file_name);
void render_destroy();


#endif
