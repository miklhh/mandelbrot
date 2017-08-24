#include <stdint.h>
#include <thread>
#include <vector>
#include <atomic>

#include "render.h"
#include "supersampling.h"


using std::vector;
using std::thread;
using std::complex;
using std::atomic;

/* Render data. */
extern const int scr_width;
extern const int scr_height;
static vector<vector<atomic<color_t>>> render_buffer;
static uint8_t threads;
static vector<thread> render_threads;
static bool rendering_complete;
static scale_t scale;
static offset_t offset;


/* Render a segment of the set. */
static void render_segment(int x_bgn, int y_bgn, int x_end, int y_end)
{
    for (int px_y = y_bgn; px_y < y_end; px_y++)
    {
        for (int px_x = x_bgn; px_x < x_end; px_x++)
        {
            color_t px_color = get_px_4x_ss(px_x, px_y, scale, offset);
            render_buffer[px_y][px_x].store(px_color);
        }
    }
}


void render_init(uint8_t n_threads)
{
    /* Initialze render data. */
    threads = n_threads;
    rendering_complete = false;

    /* Initialze the render buffer. */
    for (int y = 0; y < scr_height; y++)
    {
        vector<atomic<color_t>> tmp_vector;
        for (int x = 0; x < scr_width; x++)
        {
            color_t px_color = color_t{ 0, 0, 0, 0 };
            atomic<color_t> atomic_px_color(px_color);
            tmp_vector.push_back(atomic_px_color);
        }
        render_buffer.push_back(tmp_vector);
    }
}


void render_mandelbrot(complex<double> upper_left, complex<double> lower_right)
{

}


void render_draw(SDL_Renderer* renderer)
{

}


bool render_test_complete()
{
    return rendering_complete;
}
