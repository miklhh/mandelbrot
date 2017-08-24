#include <stdint.h>
#include <thread>
#include <vector>

#include "render.h"

using std::vector;
using std::thread;
using std::complex;

/* Render data. */
static SDL_Window* window;
static SDL_Renderer* renderer;
static uint8_t threads;
static vector<thread> render_threads;

void render_init(uint8_t n_threads)
{
    threads = n_threads;
}

void render_mandelbrot(complex<double> upper_left, complex<double> lower_right)
{

}

void render_draw(SDL_Renderer * renderer)
{
}
