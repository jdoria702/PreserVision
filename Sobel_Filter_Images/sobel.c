#include "sobel.h"
#include "netpbm.h"
#include <math.h>

/*
    The Sobel filters used are negatives of the ones from class. 
    There is no meaningful difference other than the fact that the sign / direction of the gradient is now reversed.
    When we calculate the final edge magnitude, it's squared anyways, so sign disappears.
*/
Matrix sobelGx(Matrix input) {
    double kData[3][3] = {
        {1, 0, -1},
        {2, 0, -2},
        {1, 0, -1}
    };
    Matrix kernel = createMatrixFromArray(&kData[0][0], 3, 3);
    Matrix result = convolve(input, kernel);
    deleteMatrix(kernel);
    return result;
}

Matrix sobelGy(Matrix input) {
    double kData[3][3] = {
        {1, 2, 1},
        { 0,  0,  0},
        { -1,  -2,  -1}
    };
    Matrix kernel = createMatrixFromArray(&kData[0][0], 3, 3);
    Matrix result = convolve(input, kernel);
    deleteMatrix(kernel);
    return result;
}

Matrix sobelEdgeDetection(Matrix input) {
    Matrix gx = sobelGx(input);
    Matrix gy = sobelGy(input);

    Matrix mag = createMatrix(input.height, input.width);
    for (int y = 0; y < input.height; y++) {
        for (int x = 0; x < input.width; x++) {
            mag.map[y][x] = sqrt(gx.map[y][x] * gx.map[y][x] +
                                 gy.map[y][x] * gy.map[y][x]);
        }
    }

    deleteMatrix(gx);
    deleteMatrix(gy);
    return mag;
}
