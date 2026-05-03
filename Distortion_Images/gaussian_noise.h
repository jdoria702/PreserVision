#ifndef GAUSSIAN_NOISE_H
#define GAUSSIAN_NOISE_H

#include "netpbm.h"

// Adds Gaussian noise to each RGB channel of the image.
// sigma: standard deviation of the noise (0 = no noise, higher = more noise)
Image addGaussianNoise(Image input, double sigma);

#endif
