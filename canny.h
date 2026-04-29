#ifndef CANNY_H
#define CANNY_H

#include "netpbm.h"
#include "convolution.h"

Matrix magnitude(Matrix m1, Matrix m2);
Matrix orientation(Matrix m1, Matrix m2);
Matrix sector(Matrix m);
Matrix nonMaximaSuppression(Matrix m, Matrix s);
Matrix hysteresisThresholding(Matrix m, int lowThreshold, int highThreshold);
Matrix cannyEdgeDetection(Matrix inputMatrix, int lowThreshold, int highThreshold);


#endif