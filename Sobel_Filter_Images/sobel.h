#ifndef SOBEL_H
#define SOBEL_H

#include "netpbm.h"
#include "convolution.h"

Matrix sobelGx(Matrix input);
Matrix sobelGy(Matrix input);
Matrix sobelEdgeDetection(Matrix input);

#endif
