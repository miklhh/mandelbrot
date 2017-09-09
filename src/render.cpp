#include <stdint.h>
#include <thread>
#include <vector>
#include <atomic>
#include <iostream>

#include "render.h"
#include "supersampling.h"
#include "types.h"
#include "thread_pool.h"

using std::vector;
using std::thread;
using std::complex;
using std::atomic;

/* Render data. */
extern const int scr_width;
extern const int scr_height;
static vector<vector<rgb_t>> render_buffer;
static mandelbrot_threadpool* thread_pool = NULL;
static uint8_t threads;
static bool rendering_complete;
static scale_t scale;
static offset_t offset;
bool render_initialized = false;


/* Render a single pixel to the */
static rgb_t render_get_px(int px_x, int px_y)
{
    return get_px_4x_ss(px_x, px_y);
}

/* Render a segment of the set. */
static void render_segment(const segment_t & segment)
{
    for (int y = segment.y_bgn; y < segment.y_end; y++)
    {
        for (int x = segment.x_bgn; x < segment.x_end; x++)
        {
            render_buffer[y][x] = render_get_px(x, y);
        }
    }
}

/* Function for initializing a renderer. */
void render_init(int n_threads)
{
    /* Initialize render buffer. */
    for (int y = 0; y < scr_height; y++)
    {
        render_buffer.push_back(vector<rgb_t>());
        for (int x = 0; x < scr_width; x++)
        {
            render_buffer.at(y).push_back(rgb_t{ 0, 0, 0, 0 });
        }
    }

    /* Initialize the thread pool. */
    thread_pool = mandelbrot_thread_pool_create(n_threads, render_segment);
    if (thread_pool == NULL)
    {
        std::cerr << "Error initializeing threadpool, exiting." << std::endl;
        #ifdef _WIN32
        system("pause");
        #endif  
        exit(1);
    }
    /* Initialze render data. */
    threads = n_threads;
    rendering_complete = false;
    render_initialized = true;
}

/* Static help function for getting the scale. */
static scale_t get_scale(complex<double> upper_left, complex<double> lower_right)
{
    scale_t scale {
        (lower_right.real() - upper_left.real()) / scr_width,
        (lower_right.imag() - upper_left.imag()) / scr_height
    };
    return scale;
}

/* Static help function for getting the offset. */
static offset_t get_offset(complex<double> upper_left, complex<double> lower_right)
{
    offset_t offset{
        upper_left.real() + (lower_right.real() - upper_left.real()) / 2,
        lower_right.imag() + (upper_left.imag() - lower_right.imag()) / 2
    };
    offset.y = -offset.y;
    return offset;
}

void render_mandelbrot(complex<double> upper_left, complex<double> lower_right)
{
    /* Test if renderer is initialized. */
    if (!render_initialized)
    {
        std::cerr << "Renderer not initialized. Could not render." << std::endl;
        return;
    }
    /* Set the scale and the offset and aquire the segments. */
    scale = get_scale(upper_left, lower_right);
    offset = get_offset(upper_left, lower_right);
    supersample_init(scale, offset);
    int segment_size_x = scr_width / 20;
    int segment_size_y = scr_height / 20;
    for (int y = 0; y < scr_height / segment_size_y; y++)
    {
        for (int x = 0; x < scr_width / segment_size_x; x++)
        {
            segment_t segment = {
                segment_size_x * x,
                segment_size_y * y,
                segment_size_x * (1 + x),
                segment_size_y * (1 + y)};
            mandelbrot_thread_pool_add_job(thread_pool, segment);
        }
    }
}


void render_draw_to_SDL_Renderer(SDL_Renderer* renderer)
{
    for (int y = 0; y < scr_height; y++)
    {
        for (int x = 0; x < scr_width; x++)
        {
            SDL_SetRenderDrawColor(
                renderer,
                render_buffer[y][x].red,
                render_buffer[y][x].green,
                render_buffer[y][x].blue,
                render_buffer[y][x].alpha);
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }
}

/* Temporarly, this needs a fix! */
bool render_test_complete()
{
    return !mandelbrot_thread_pool_get_active_workers(thread_pool) ? true : false;
}

/* Function for setting a pixel on an SDL_Surface. */
static void set_pixel_on_surface(SDL_Surface* surface, int x, int y, rgb_t pixel)
{
    ((rgb_t*)surface->pixels)[y * scr_width + x] = pixel;
}

/* Function for creating a bmp and storing it in execution directory. */
int render_create_bmp(char* file_name)
{
    /* Create a surface and unlock the pixels*/
    std::cout << "Creating BMP." << std::endl;
    SDL_Surface* surface = SDL_CreateRGBSurface(0, scr_width, scr_height, 32, 0, 0, 0, 0);
    SDL_LockSurface(surface);
    for (int y = 0; y < scr_height; y++)
    {
        for (int x = 0; x < scr_width; x++)
        {
            set_pixel_on_surface(surface, x, y, render_buffer[y][x]);
        }
    }
    SDL_UnlockSurface(surface);
    SDL_SaveBMP(surface, file_name);
    std::cout << "BMP created." << std::endl;
    return 0;
}
