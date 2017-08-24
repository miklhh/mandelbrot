#include <stdint.h>
#include <math.h>
#include <iostream>

#include "color.h"
#include "types.h"

/* Coloring functions for red, green and blue.*/
uint8_t color_r(uint32_t n, std::complex<double> zn)
{
    //uint32_t return_value = 1 + n - log(log(abs(zn))) / log(2.0);
    //return uint8_t(return_value);
    return 0;
}

uint8_t color_g(uint32_t n, std::complex<double> zn)
{
    //uint32_t return_value = 1 + n - log(log(abs(zn))) / log(2.0);
    //return uint8_t(return_value);
    return 0;

}

uint8_t color_b(uint32_t n, std::complex<double> zn)
{
    uint32_t return_value = 1 + n - log(log(abs(zn))) / log(2.0);
    return uint8_t(return_value);
}


/* Function sets the color to draw. */
color_t get_color(
    uint32_t iterations,
    std::complex<double> zn)
{
    color_t color;
    color.alpha = 0;
    color.red = 0;
    color.green = 0;
    color.blue = 0;

    /* If it converges. */
    extern uint32_t max_iterations;
    if (iterations == max_iterations)
    {
        color.red = 0;
        color.green = 0;
        color.blue = 0;
    }
    else
    {
        color.red = color_r(iterations, zn);
        color.green = color_g(iterations, zn);
        color.blue = color_b(iterations, zn);
    }
    return color;
}