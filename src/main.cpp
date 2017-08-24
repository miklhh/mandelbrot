#include <SDL.h>
#include <iostream>
#include <complex>
#include <cmath>
#include <string>
#include <thread>
#include <vector>
#include <atomic>

#include "color.h"
#include "types.h"
#include "supersampling.h"
#include "render.h"


extern const int scr_width = 600;
extern const int scr_height = 400;
uint32_t max_iterations = 40000;

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static bool window_is_open = true;

/* Render buffer. */
static std::atomic<color_t> render_buffer[scr_height][scr_width];



void shutdown(std::string str)
{
	std::cout << "Error: " << str.c_str() << std::endl;
	std::cout << "Shuting down." << std::endl;
	window_is_open = false;
}

void draw_mandelbrot_iterate(int y_start, int y_end, scale_t scale, offset_t offset)
{
    /* Iterate over each pixel on the screen and draw. */
    for (int px_y = y_start; px_y <= y_end; px_y++)
    {
        for (int px_x = 0; px_x < scr_width; px_x++)
        {
            /* Get the color for the current pixel and store in the buffer. */
            color_t color = get_px_4x_ss(px_x, px_y, scale, offset);
            //render_buffer[px_y][px_x] = color;
            render_buffer[px_y][px_x].store(color);
        }
    }
}

using std::complex;
void draw_mandelbrot(complex<double> upper_left, complex<double> lower_right, uint8_t threads)
{ 
    using std::complex;

    /* Mathematical calculations for scalings */
    scale_t scale;
    offset_t offset;
    scale.x = (lower_right.real() - upper_left.real()) / scr_width;
    scale.y = (lower_right.imag() - upper_left.imag()) / scr_height;
    offset.x = upper_left.real() + (lower_right.real() - upper_left.real()) / 2;
    offset.y = lower_right.imag() + (upper_left.imag() - lower_right.imag()) / 2;
    offset.y = -offset.y;   // Flip to get mathmatical y-value.

	double scale_x = lower_right.real() - upper_left.real();
	double scale_y = lower_right.imag() - upper_left.imag();
	double offset_x = upper_left.real() + (lower_right.real() - upper_left.real()) / 2;
	double offset_y = lower_right.imag() + (upper_left.imag() - lower_right.imag()) / 2;
	offset_y = -offset_y; 	// Flip mathematical Y.;
	scale_x /= scr_width;
	scale_y /= scr_height;
	

    /* Create vector of threads. */
    using std::thread;
    using std::vector;
    vector<thread> thread_vector;

    /* Zero the render buffer. */
    for (int y = 0; y < scr_height; y++)
    {
        for (int x = 0; x < scr_width; x++)
        {
            //render_buffer[y][x] = color_t{ 0, 0, 0 };
            render_buffer[y][x].store(color_t{ 0, 0, 0 });
        }
    }
    
    /* Let the threads go to work.*/
    for (uint8_t i = 0; i < threads; i++)
    {
        int lines_per_thread = scr_height / threads;
        int y_start = i * lines_per_thread;
        int y_end = (i + 1) * lines_per_thread;
        thread_vector.push_back( thread(draw_mandelbrot_iterate, y_start, y_end, scale, offset) );
    }

    while(1)
    {
        for (int y = 0; y < scr_height; y++)
        {
            for (int x = 0; x < scr_width; x++)
            {
                SDL_SetRenderDrawColor(
                    renderer,
                    render_buffer[y][x].load().red,
                    render_buffer[y][x].load().green,
                    render_buffer[y][x].load().blue,
                    render_buffer[y][x].load().alpha);
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }


    /* Iterate over each pixel on the screen and draw. */
	for (int px_y = 0; px_y < scr_height; px_y++)
	{
		for (int px_x = 0; px_x < scr_width; px_x++)
		{
            /* Get the color for the current pixel. */
            color_t color = get_px_4x_ss(px_x, px_y, scale, offset);
            SDL_SetRenderDrawColor(renderer, color.red, color.green, color.blue, color.alpha);
			SDL_RenderDrawPoint(renderer, px_x, px_y);
			SDL_RenderPresent(renderer);
		}
	}
}

int main (int argc, char *argv[])
{
	/* Init SDL. */
	if (SDL_Init(SDL_INIT_VIDEO) != 0) 
	{
		std::cout << "Could not init SDL." << std::endl;
		return -1;
	}

	SDL_Event event;
	if (SDL_CreateWindowAndRenderer(scr_width, scr_height, 0, &window, &renderer) != 0)
	{
		std::cout << "Could not create window and renderer." << std::endl;
		return -1;
	}

    /* Initialize the mandelbrot renderer. */
    render_init(4);

    /* Draw mandelbrot to the renderer and renderer to the screen. */
    using std::complex;
    complex<double> ul = std::complex<double>(-0.747, 0.120);
    complex<double> lr = std::complex<double>(-0.746, 0.119);
    std::thread render(draw_mandelbrot, ul, lr, 4);
    render.detach();

	while (window_is_open)
	{
		/* Handle events. */
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT) { window_is_open = false; }
		}
	}

    return 0;
}
