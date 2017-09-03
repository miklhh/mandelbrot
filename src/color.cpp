#include <stdint.h>
#include <math.h>
#include <complex>
#include <iostream>

#include "color.h"
#include "types.h"

const double pi = 3.1415926535;

/* Shift the range of a value to [ranger_lower, range_higher]. Useful forexample
 * when you want to shift the angle to the standard angle [0, 2*pi]. If the 
 * conversion can't be done, the function will return after 1000 iterations. */
static double shift_range(double value, double range_lower, double range_higher)
{
    const int max_shifts = 1000;
    const double shift_value = range_higher - range_lower;
    for (int i = 0; i < max_shifts; i++)
    {
        if (value < range_lower) { value += shift_value; }
        else if (value > range_higher) { value -= shift_value; }
        else return value;
    }
    return value;
}

/* Conversion from wikipedia: 
 * https://en.wikipedia.org/wiki/HSL_and_HSV#Converting_to_RGB 
 */
static rgb_t hsl_to_rgb(hsl_t hsl)
{
    

    /* Make sure that the hue is in it's correct spectrum. */
    hsl.hue = shift_range(hsl.hue, 0, 2*pi);
    double hue_prime = hsl.hue / (pi / 3);
    double chroma = (double(1) - abs(2 * hsl.light - 1)) * hsl.saturation;
    double x = chroma * (1 - abs(fmod(hue_prime, 2) - 1));

    double r1 = 0;
    double g1 = 0;
    double b1 = 0;
         if (0 <= hue_prime && hue_prime <= 1) { r1 = chroma;  g1 = x;       b1 = 0;      }
    else if (1 <= hue_prime && hue_prime <= 2) { r1 = x;       g1 = chroma;  b1 = 0;      }
    else if (2 <= hue_prime && hue_prime <= 3) { r1 = 0;       g1 = chroma;  b1 = x;      }
    else if (3 <= hue_prime && hue_prime <= 4) { r1 = 0;       g1 = x;       b1 = chroma; }
    else if (4 <= hue_prime && hue_prime <= 5) { r1 = x;       g1 = 0;       b1 = chroma; }
    else if (5 <= hue_prime && hue_prime <= 6) { r1 = chroma;  g1 = 0;       b1 = x;      }

    double m = hsl.light - chroma / 2;
    uint8_t r = uint8_t((r1 + m) * 255);
    uint8_t g = uint8_t((g1 + m) * 255);
    uint8_t b = uint8_t((b1 + m) * 255);
    rgb_t color = { r, g, b, 0 };
    return color;
}

/* Get the convergence value if iterations is the number of iterations requiered
 * to escape the 'escape-radius' and zn is the final value of the series when it
 * has surpasst the 'escape-radius. Based on wikipedias 'smooth-coloring'.
 * functions is supposed to return a value [0, max_iterations]. */
static double get_convergence_value(
    int iterations, 
    std::complex<double> zn, 
    std::complex<double> c)
{
    const double escape_radius = 2.0;

    /* Enchance the divirging point to get a 'smoother' value. */
    for (int i = 0; i < 5; i++)
    {
        zn = zn * zn + c;
        iterations++;
    }
    return double(iterations) - log2( log( abs(zn) ) / log(escape_radius) );
}

/* Function sets the color to draw. */
rgb_t get_color(int iterations, std::complex<double> zn, std::complex<double> c)
{
    rgb_t color{ 0, 0, 0, 0 };

    /* If it converges. */
    extern uint32_t max_iterations;
    if (iterations == max_iterations)
    {
        return color;
    }
    else
    {
        /*
         * Coloring algorithm by Paride:
         * http://www.paridebroggi.com/2015/05/fractal-continuous-coloring.html 
         */
        /*
        color.red = uint8_t(sin(0.016 * color_value(iterations, zn) + 4) * 100 + 155);
        color.green = uint8_t(sin(0.013 * color_value(iterations, zn) + 2) * 100 + 155);
        color.blue = uint8_t(sin(0.010 * color_value(iterations, zn) + (3.1415/2)) * 100 + 155);
        return color;
        */
        double convergence_value = get_convergence_value(iterations, zn, c);
        rgb_t color = hsl_to_rgb(
        {
            //amplitude_h * sin(frequency_h * convergence_value + phase_h) + offset_h,
            log(10 * convergence_value),
            //amplitude_s * sin(frequency_s * convergence_value + phase_s) + offset_s,
            1,
            0.7
        });
        //std::cout << convergence_value / 5000 * (2 * pi) << std::endl;
        return color;
    }
}