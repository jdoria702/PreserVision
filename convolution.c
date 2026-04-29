#include "convolution.h"

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