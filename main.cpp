#include <SDL.h>
#include <iostream>
#include <complex>
#include <cmath>
#include <string>

const int scr_width = 600;
const int scr_height = 400;

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static bool window_is_open = true;


void shutdown(std::string str)
{
	std::cout << "Error: " << str.c_str() << std::endl;
	std::cout << "Shuting down." << std::endl;
	window_is_open = false;
}

/* Coloring functinos. */
uint8_t color_r(uint8_t x)
{
    return 45 * log(x - 20) + 5;
}

uint8_t color_g(uint8_t x)
{
    return 45 * log(x - 40) + 10;
    //return 255 / exp(pow(x - 255, 2) / 9000);
}

uint8_t color_b(uint8_t x)
{
    return 45 * log(x);
}

/* Function sets the color to draw. */
void set_color(uint8_t iterations, double scale_x, double scale_y)
{
	const uint8_t alpha = 0;
	//uint8_t red = uint8_t(255 - (3 * abs(iterations))^20 );
	//uint8_t green = uint8_t(255 - (3 * abs(iterations))^20 );
    
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;

    /* If it converges. */
    if (iterations == 255)
    
    {
        red = 0;
        green = 0;
        blue = 0;
    }
    else
    {
        red = color_r(iterations);
        green = color_g(iterations);
        blue = color_b(iterations);
    }


	SDL_SetRenderDrawColor(renderer, red, green, blue, alpha);
}

using std::complex;
void draw_mandelbrot(complex<double> upper_left, complex<double> lower_right)
{ using std::complex;

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
			//std::cout << "C real: " << c.real() << " C imag: " << c.imag() << std::endl;

			uint8_t iterations = 0;
			for (iterations = 0; iterations < 255; iterations++)
			{
				z = z * z + c;
				if (abs(z) > 2.0) { break; }
			}

			//std::cout << "Drawing... X: " << px_x << ", Y: " << px_y << ", ";

			//std::cout << "Iterations: " << int(iterations) << std::endl;
			set_color(iterations, scale_x, scale_y);
			//std::cout << "ABS(z) = " << abs(z) << std::endl;
			SDL_RenderDrawPoint(renderer, px_x, px_y);
			SDL_RenderPresent(renderer);
			
			/*
			if (abs(z) <= 2.0)
			{
				std::cout << "Iterations: " << iterations << std::endl;
				set_color(iterations, scale_x, scale_y);
				std::cout << "ABS(z) = " << abs(z) << std::endl;
				SDL_RenderDrawPoint(renderer, px_x, px_y);
				SDL_RenderPresent(renderer);
			}
			*/
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

	SDL_RenderPresent(renderer);
	while (window_is_open)
	{
		/* Draw mandelbrot to the renderer and renderer to the screen. */
		using std::complex;
		complex<double> ul = std::complex<double>(-2, 1);
		complex<double> lr = std::complex<double>(1, -1);
		draw_mandelbrot(ul, lr);
		SDL_RenderPresent(renderer);

		/* Handle events. */
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT) { window_is_open = false; }
		}
	}
}
