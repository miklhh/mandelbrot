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
#include "render.h"


extern const int scr_width = 1500;
extern const int scr_height = 1000;
uint32_t max_iterations = 4000;

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static bool window_is_open = true;


void shutdown(std::string str)
{
	std::cout << "Error: " << str.c_str() << std::endl;
	std::cout << "Shuting down." << std::endl;
    exit(1);
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
		std::cerr << "Could not create window and renderer." << std::endl;
		return -1;
	}

    /* Initialize the mandelbrot renderer. */
    complex<double> ul = std::complex<double>(-0.747, 0.120);
    complex<double> lr = std::complex<double>(-0.746, 0.119);
    //complex<double> ul = std::complex<double>(-2, 1);
    //complex<double> lr = std::complex<double>(1, -1);
    render_init(4);
    std::thread render_thread(render_mandelbrot, ul, lr);


    /* Draw mandelbrot to the renderer and renderer to the screen. */
    bool once = true;
	while (window_is_open)
	{
        //if (render_test_complete() && once)
        //{
            render_draw_to_SDL_Renderer(renderer);
        //    render_create_bmp("hello.bmp");
            SDL_RenderPresent(renderer);
        //    once = false;
        //}


		/* Handle events. */
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT) { window_is_open = false; }
		}
        SDL_Delay(5);
	}

    render_thread.detach();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return 0;
}
