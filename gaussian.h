#ifndef GAUSSIAN_H
#define GAUSSIAN_H

#include "netpbm.h"

Matrix convolve(Matrix input, Matrix kernel);
Matrix createGaussianKernel(int size, double sigma);
Matrix gaussianBlur(Matrix input, int size, double sigma);

#endif // GAUSSIAN_H