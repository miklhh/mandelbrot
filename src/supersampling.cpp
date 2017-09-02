#include <complex>
#include <iostream>

#include "supersampling.h"
#include "color.h"

extern const int scr_width;
extern const int scr_height;
extern uint32_t max_iterations;

/* Get the average of four numbers. */
static uint8_t get_average(uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4)
{
    return (uint32_t(c1) + uint32_t(c2) + uint32_t(c3) + uint32_t(c4)) / 4;
}

/* Get a pixel without supersampling. */
rgb_t get_px_1x_ss(int px_x, int px_y, scale_t scale, offset_t offset)
{
    using std::complex;
    double x = double((px_x - scr_width / 2) * scale.x + offset.x);
    double y = double((px_y - scr_height / 2) * scale.y + offset.y);
    complex<double> c(x, y);
    complex<double> z(0, 0);


    complex<double> c1(x, y);
    complex<double> z1(0, 0);
    uint32_t iterations = 0;
    for (iterations; iterations < max_iterations; iterations++)
    {
        z1 = z1 * z1 + c1;
        if (abs(z1) > 2.0) { break; }
    }
    return get_color(iterations, z1);
}

/* Get a px with 4 times supersampling. */
rgb_t get_px_4x_ss(int px_x, int px_y, scale_t scale, offset_t offset)
{
    using std::complex;
    double x = double((px_x - scr_width / 2) * scale.x + offset.x);
    double y = double((px_y - scr_height / 2) * scale.y + offset.y);
    complex<double> c(x, y);
    complex<double> z(0, 0);

    /* Calculate the color for the four 'underlaying pixels'. 
     * Supersamling layout: |1|2|
     *                      |3|4| 
     */
    
    /* 'Pixel': 1. */
    complex<double> c1(x - scale.x / 4, y - scale.y / 4);
    complex<double> z1(0, 0);
    uint32_t iterations1 = 0;
    for (iterations1; iterations1 < max_iterations; iterations1++)
    {
        z1 = z1 * z1 + c1;
        if (abs(z1) > 2.0) { break; }
    }
    rgb_t color1 = get_color(iterations1, z1);

    /* 'Pixel': 2. */
    complex<double> c2(c.real() - scale.x / 4, c.imag() + scale.y / 4);
    complex<double> z2(0, 0);
    uint32_t iterations2 = 0;
    for (iterations2; iterations2 < max_iterations; iterations2++)
    {
        z2 = z2 * z2 + c2;
        if (abs(z2) > 2.0) { break; }
    }
    rgb_t color2 = get_color(iterations2, z2);

    /* 'Pixel': 3. */
    complex<double> c3(c.real() + scale.x / 4, c.imag() - scale.y / 4);
    complex<double> z3(0, 0);
    uint32_t iterations3 = 0;
    for (iterations3; iterations3 < max_iterations; iterations3++)
    {
        z3 = z3 * z3 + c3;
        if (abs(z3) > 2.0) { break; }
    }
    rgb_t color3 = get_color(iterations3, z3);

    /* 'Pixel': 4. */
    complex<double> c4(c.real() + scale.x / 4, c.imag() + scale.y / 4);
    complex<double> z4(0, 0);
    uint32_t iterations4 = 0;
    for (iterations4; iterations4 < max_iterations; iterations4++)
    {
        z4 = z4 * z4 + c4;
        if (abs(z4) > 2.0) { break; }
    }
    rgb_t color4 = get_color(iterations4, z4);

    /* Calculate the avrage. */
    rgb_t color;
    color.alpha = get_average(color1.alpha, color2.alpha, color3.alpha, color4.alpha);
    color.blue = get_average(color1.blue, color2.blue, color3.blue, color4.blue);
    color.green = get_average(color1.green, color2.green, color3.green, color4.green);
    color.red = get_average(color1.red, color2.red, color3.red, color4.red);

    return color;
}
