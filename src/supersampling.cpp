#include <complex>
#include <iostream>
#include <cmath>

#include "supersampling.h"
#include "color.h"

extern const int scr_width;
extern const int scr_height;
static scale_t scale;
static offset_t offset;
extern uint32_t max_iterations;

/* Test if the series has escaped the escape-radius. Function implemented
 * without absolut value, since testing shows that this runs alot faster.
 * - Math:
 * - Let: z = x+iy
 * - |z| > escape_radius                      <==>
 * - |x+iy| > escape_radius                   <==>
 * - |x| + |y| > escape_radius                <==>
 * - sqrt(x^2) + sqrt(y^2) > escape_radius    <==>
 * - x^2 + y^2 > escape_radius^2
 * This way of doing the calculation (without having to do the square-root
 * via the absolut value) is alot faster on many computers and
 * mathimaticlly equvalent to doing the absolut value test. */
static bool test_escape(const std::complex<double> & zn, double escape_radius)
{
    auto square = [](double x) -> double { return x * x; };
    return square(zn.real()) + square(zn.imag()) > square(escape_radius);
}

/* Get the average of four numbers. */
static uint8_t get_average(uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4)
{
    return (uint32_t(c1) + uint32_t(c2) + uint32_t(c3) + uint32_t(c4)) / 4;
}

/* Initialize the supersampling. */
void supersample_init(const scale_t & scl, const offset_t & ofst)
{
    scale = scl;
    offset = ofst;
}

/* Get a pixel without supersampling. */
rgb_t get_px_1x_ss(int px_x, int px_y)
{
    using std::complex;
    double x = double((px_x - scr_width / 2) * scale.x + offset.x);
    double y = double((px_y - scr_height / 2) * scale.y + offset.y);
    complex<double> c(x, y);
    complex<double> z(0, 0);
    unsigned int iterations;
    for (iterations = 0; iterations < max_iterations; iterations++)
    {
        z = z * z + c;
        if (test_escape(z, 2.0)) { break; }
    }
    return get_color(iterations, z, c);
}

/* Get a px with 4 times supersampling. */
rgb_t get_px_4x_ss(int px_x, int px_y)
{
    using std::complex;
    double x = double((px_x - scr_width / 2) * scale.x + offset.x);
    double y = double((px_y - scr_height / 2) * scale.y + offset.y);
    complex<double> c(x, y);
    complex<double> z(0, 0);

    /* 'Pixel': 1. */
    complex<double> c1(x - scale.x / 4, y - scale.y / 4);
    complex<double> z1(0, 0);
    unsigned int iterations1;
    for (iterations1 = 0; iterations1 < max_iterations; iterations1++)
    {
        z1 = z1 * z1 + c1;
        if (test_escape(z1, 2.0)) { break; }
    }
    rgb_t color1 = get_color(iterations1, z1, c1);
    /* 'Pixel': 2. */
    complex<double> c2(c.real() - scale.x / 4, c.imag() + scale.y / 4);
    complex<double> z2(0, 0);
    unsigned int iterations2;
    for (iterations2 = 0; iterations2 < max_iterations; iterations2++)
    {
        z2 = z2 * z2 + c2;
        if (test_escape(z2, 2.0)) { break; }
    }
    rgb_t color2 = get_color(iterations2, z2, c2);
    /* 'Pixel': 3. */
    complex<double> c3(c.real() + scale.x / 4, c.imag() - scale.y / 4);
    complex<double> z3(0, 0);
    unsigned int iterations3;
    for (iterations3 = 0; iterations3 < max_iterations; iterations3++)
    {
        z3 = z3 * z3 + c3;
        if (test_escape(z3, 2.0)) { break; }
    }
    rgb_t color3 = get_color(iterations3, z3, c3);
    /* 'Pixel': 4. */
    complex<double> c4(c.real() + scale.x / 4, c.imag() + scale.y / 4);
    complex<double> z4(0, 0);
    unsigned int iterations4;
    for (iterations4 = 0; iterations4 < max_iterations; iterations4++)
    {
        z4 = z4 * z4 + c4;
        if (test_escape(z4, 2.0)) { break; }
    }
    rgb_t color4 = get_color(iterations4, z4, c4);
    /* Calculate the avrage and return the color. */
    rgb_t color;
    color.alpha = get_average(color1.alpha, color2.alpha, color3.alpha, color4.alpha);
    color.blue = get_average(color1.blue, color2.blue, color3.blue, color4.blue);
    color.green = get_average(color1.green, color2.green, color3.green, color4.green);
    color.red = get_average(color1.red, color2.red, color3.red, color4.red);
    return color;
}
