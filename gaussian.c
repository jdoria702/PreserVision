#include "gaussian.h"
#include "netpbm.h"
#include <math.h>

Matrix convolve(Matrix m1, Matrix m2) {
    int paddingX = m2.width / 2;
    int paddingY = m2.height / 2;
    
    Matrix convolvedMatrix = createMatrix(m1.height, m1.width);
    for (int y = 0 + paddingY; y < m1.height - paddingY; y++) {
        for (int x = 0 + paddingX; x < m1.width - paddingX; x++) {
            double sum = 0;
            for (int i = 0; i < m2.height; i++) {
                for (int j = 0; j < m2.width; j++) {
                    sum += m1.map[y - paddingY + i][x - paddingX + j] * m2.map[i][j];
                }
            }
            convolvedMatrix.map[y][x] = sum;
        }
    }
    return convolvedMatrix;
}

Matrix createGaussianKernel(int size, double sigma) {
    // Require odd size (3,5,7,...) and positive sigma
    if (size < 1 || size % 2 == 0 || sigma <= 0.0) {
        return createMatrix(0, 0);
    }

    Matrix kernel = createMatrix(size, size);
    int center = size / 2;
    double sum = 0.0;
    double twoSigmaSq = 2.0 * sigma * sigma;
    double norm = 1.0 / (2.0 * PI * sigma * sigma);

    // Fill kernel with Gaussian weights
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            // Get position relative to center
            int dx = x - center;
            int dy = y - center;
            double val = norm * exp(-(dx * dx + dy * dy) / twoSigmaSq);
            kernel.map[y][x] = val;
            sum += val;
        }
    }

    // Normalize so all entries sum to 1
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            kernel.map[y][x] /= sum;
        }
    }

    return kernel;
}

Matrix gaussianBlur(Matrix input, int size, double sigma) {
    Matrix kernel = createGaussianKernel(size, sigma);
    Matrix out = convolve(input, kernel);
    deleteMatrix(kernel);
    return out;
}