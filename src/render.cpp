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
static vector<vector<color_t>> render_buffer;
static mandelbrot_threadpool* thread_pool;
static uint8_t threads;
static bool rendering_complete;
static scale_t scale;
static offset_t offset;
bool render_initialized = false;



/* Render a single pixel to the */
static color_t render_get_px(int px_x, int px_y)
{
    return get_px_4x_ss(px_x, px_y, scale, offset);
}

/* Render a segment of the set. */
static void render_segment(segment_t segment)
{
    /*
    int px_x_bgn = segment.x_bgn;
    int px_y_bgn = segment.y_bgn;
    int px_x_end = segment.x_end;
    int px_y_end = segment.y_end;
    for (int px_y = px_y_bgn; px_y < px_y_end; px_y++)
    {
        for (int px_x = px_x_bgn; px_x < px_x_end; px_x++)
        {
            color_t color = render_get_px(px_x, px_y);
            render_buffer[px_y][px_x] = color;
        }
    }
    */
    mandelbrot_threadpool_add_job(thread_pool, segment);
}

/* Function for initializing a renderer. */
void render_init(int n_threads)
{
    /* Initialize render buffer. */
    for (int y = 0; y < scr_height; y++)
    {
        render_buffer.push_back(vector<color_t>());
        for (int x = 0; x < scr_width; x++)
        {
            render_buffer.at(y).push_back(color_t{ 0, 0, 0, 0 });
        }
    }

    /* Initialize the thread pool. */
    thread_pool = mandelbrot_threadpool_init(n_threads, render_segment);
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

    /* Set the scale and the offset. */
    scale = get_scale(upper_left, lower_right);
    offset = get_offset(upper_left, lower_right);

    /* Aquire the segments. */
    int segment_size_x = scr_width / 4;
    int segment_size_y = scr_height / 4;
    for (int y = 0; y < scr_height / segment_size_y; y++)
    {
        for (int x = 0; x < scr_width / segment_size_x; x++)
        {
            // DEBUGING!
            std::cout
                << "X start: "
                << segment_size_x * x
                << " X end: "
                << segment_size_x * (x + 1)
                << " Y start: "
                << segment_size_y * y
                << " Y end: "
                << segment_size_y * (y + 1)
                << std::endl;
            segment_t segment = {
                segment_size_x * x,
                segment_size_y * y,
                segment_size_x * (1 + x),
                segment_size_y * (1 + y)
            };
            /* Render. */
            render_segment(segment);
        }
    }

    /* Rendering is now done. */
    rendering_complete = true;
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
                render_buffer[y][x].alpha
            );
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }
}


bool render_test_complete()
{
    return rendering_complete;
}

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
            ((SDL_Color*)surface->pixels)[y * scr_width + x].r = render_buffer[y][x].red;
            ((SDL_Color*)surface->pixels)[y * scr_width + x].g = render_buffer[y][x].green;
            ((SDL_Color*)surface->pixels)[y * scr_width + x].b = render_buffer[y][x].blue;
            ((SDL_Color*)surface->pixels)[y * scr_width + x].a = render_buffer[y][x].alpha;
        }
    }

    SDL_UnlockSurface(surface);
    SDL_SaveBMP(surface, file_name);
    std::cout << "BMP created." << std::endl;
    return 0;
}
