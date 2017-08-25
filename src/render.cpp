#include <stdint.h>
#include <thread>
#include <vector>
#include <atomic>
#include <iostream>

#include "render.h"
#include "supersampling.h"


using std::vector;
using std::thread;
using std::complex;
using std::atomic;

/* Render data. */
extern const int scr_width;
extern const int scr_height;
static vector<vector<color_t>> render_buffer;
static uint8_t threads;
static bool rendering_complete;
static scale_t scale;
static offset_t offset;
bool render_initialized = false;



/* Simple implementation of threadpool. */
static class thread_pool
{
public:
    /* Constructor. */
    thread_pool(int n_threads)
    {
        m_threads = n_threads;
        for (int i = 0; i < threads; i++) { m_render_threads.push_back(thread()); }
    }

private:
    vector<thread> m_render_threads;
    int m_threads;
};

/* Render a single pixel to the */
static color_t render_get_px(int px_x, int px_y)
{
    return get_px_4x_ss(px_x, px_y, scale, offset);
}

/* Render a segment of the set. */
static void render_segment(int px_x_bgn, int px_y_bgn, int px_x_end, int px_y_end)
{
    for (int px_y = px_y_bgn; px_y < px_y_end; px_y++)
    {
        for (int px_x = px_x_bgn; px_x < px_x_end; px_x++)
        {
            render_buffer[px_y][px_x] = render_get_px(px_x, px_y);
        }
    }
}


void render_init(int n_threads)
{
    /* Initialze render data. */
    threads = n_threads;
    rendering_complete = false;

    for (int y = 0; y < scr_height; y++)
    {
        render_buffer.push_back(vector<color_t>());
        for (int x = 0; x < scr_width; x++)
        {
            render_buffer.at(y).push_back(color_t{ 0, 0, 0, 0 });
        }
    }
    render_initialized = true;
}


void render_mandelbrot(complex<double> upper_left, complex<double> lower_right)
{
    /* Test if renderer is initialized. */
    if (!render_initialized)
    {
        std::cerr << "Renderer not initialized. Could not render." << std::endl;
        return;
    }

    /* Aquire the segments. */
    int segment_size_x = scr_width / 32;
    int segment_size_y = scr_height / 32;
    for (int y = 0; y < scr_height / segment_size_y; y++)
    {
        for (int x = 0; x < scr_width / segment_size_x; x++)
        {
            render_segment(
                segment_size_x * x,
                segment_size_y * y,
                segment_size_x * (x + 1),
                segment_size_y * (y + 1)
            );
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

int render_create_bmp(char * file_name)
{
    
}
