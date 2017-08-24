#include <SDL.h>
#include <iostream>
#include <complex>
#include <cmath>
#include <string>
#include <thread>

#include "color.h"
#include "types.h"
#include "supersampling.h"

extern const int scr_width = 1600;
extern const int scr_height = 900;
uint32_t max_iterations = 40000;

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static bool window_is_open = true;



void shutdown(std::string str)
{
	std::cout << "Error: " << str.c_str() << std::endl;
	std::cout << "Shuting down." << std::endl;
	window_is_open = false;
}


using std::complex;
void draw_mandelbrot(complex<double> upper_left, complex<double> lower_right)
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
	
    /* Iterate over each pixel on the screen and draw. */
	for (int px_y = 0; px_y < scr_height; px_y++)
	{
		for (int px_x = 0; px_x < scr_width; px_x++)
		{
            // Debuging.
            //std::cout << "Drawing pixel, X: " << px_x << ", Y: " << px_y << std::endl;
			/* Use the scales and offsets to calculate x and y. */
			double x = double((px_x - scr_width/2) * scale_x + offset_x);
			double y = double((px_y - scr_height/2) * scale_y + offset_y);
			complex<double> c(x, y);
			complex<double> z(0.0);

            color_t color = get_px_4x_ss(px_x, px_y, scale, offset);
            // Debuging.
            std::cout
                << "Color: R: " << (uint32_t)color.red << " "
                << "Color: G: " << (uint32_t)color.green << " "
                << "Color: B: " << (uint32_t)color.blue << std::endl;

            /* Get the color for the current pixel. */
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

    /* Draw mandelbrot to the renderer and renderer to the screen. */
    using std::complex;
    complex<double> ul = std::complex<double>(-0.747, 0.120);
    complex<double> lr = std::complex<double>(-0.746, 0.119);
    std::thread render(draw_mandelbrot, ul, lr);
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
