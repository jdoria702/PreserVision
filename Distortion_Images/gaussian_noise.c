#include "gaussian_noise.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

/*
    This function adds Gaussian noise to the image.
    That is, for each pixel, add a random value drawn from a normal (Gaussian distribution).
    This is specifically using the Box-Muller transform so that we can convert uniform random numbers to the desired bell curve distribution.
 */
static double randGaussian(void) {
    double u1, u2;
    do { u1 = (double)rand() / RAND_MAX; } while (u1 == 0.0);
    u2 = (double)rand() / RAND_MAX;
    return sqrt(-2.0 * log(u1)) * cos(2.0 * PI * u2);
}

static int clamp(double v) {
    if (v < 0.0)   return 0;
    if (v > 255.0) return 255;
    return (int)(v + 0.5);
}

Image addGaussianNoise(Image input, double sigma) {
    srand((unsigned int)time(NULL));

    Image output = createImage(input.height, input.width);

    for (int i = 0; i < input.height; i++) {
        for (int j = 0; j < input.width; j++) {
            int r = clamp(input.map[i][j].r + sigma * randGaussian());
            int g = clamp(input.map[i][j].g + sigma * randGaussian());
            int b = clamp(input.map[i][j].b + sigma * randGaussian());
            int intensity = (r + g + b) / 3;
            output.map[i][j].r = (unsigned char)r;
            output.map[i][j].g = (unsigned char)g;
            output.map[i][j].b = (unsigned char)b;
            output.map[i][j].i = (unsigned char)intensity;
        }
    }

    return output;
}
