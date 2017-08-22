#include <SDL2/SDL.h>
#include <iostream>
#include <complex>
#include <cmath>
#include <string>

const int scr_width = 1280;
const int scr_height = 720;

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static bool window_is_open = true;

/* Scaling variables. */

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

	//double scale_x = upper_left.real() / double(scr_width / 2);
	//double scale_y = upper_left.imag() / double(scr_height / 2);
	double scale_x = lower_right.real() - upper_left.real();
	double scale_y = lower_right.imag() - upper_left.imag();
	double offset_x = upper_left.real() + (lower_right.real() - upper_left.real()) / 2;
	double offset_y = lower_right.imag() + (upper_left.imag() - lower_right.imag()) / 2;
	offset_y = -offset_y; 	// Flip mathematical Y.;
	scale_x /= scr_width;
	scale_y /= scr_height;
	
	for (int px_y = 0; px_y < scr_height; px_y++)
	{
		for (int px_x = 0; px_x < scr_width; px_x++)
		{

			/* z = x + yi */
			double x = double(px_x - scr_width/2);
			double y = double(px_y - scr_height/2);
			complex<double> c(x * scale_x + offset_x, y * scale_y + offset_y);
			complex<double> z(0.0);

			/* DEBUGING! */
			std::cout << "C real: " << c.real() << " C imag: " << c.imag() << std::endl;

			for (int i = 0; i < 255; i++)
			{
				z = z * z + c;
				if (abs(z) > 2.0) { break; }
			}

			std::cout << "Drawing... X: " << px_x << ", Y: " << px_y << ", ";
			if (abs(z) < 2.0)
			{
				SDL_RenderDrawPoint(renderer, px_x, px_y);
				SDL_RenderPresent(renderer);
			}
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

	while (window_is_open)
	{
		/* Draw mandelbrot to the renderer and renderer to the screen. */
		using std::complex;
		complex<double> ul = std::complex<double>(-1.8, -0.1);
		complex<double> lr = std::complex<double>(-1.6, 0.1);
		draw_mandelbrot(ul, lr);
		SDL_RenderPresent(renderer);

		/* Handle events. */
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT) { window_is_open = false; }
		}
	}
}
