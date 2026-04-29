#ifndef GAUSSIAN_H
#define GAUSSIAN_H

#include "netpbm.h"
#include "convolution.h"


Matrix createGaussianKernel(int size, double sigma);
Matrix gaussianBlur(Matrix input, int size, double sigma);

#endif