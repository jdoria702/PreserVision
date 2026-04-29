#include "netpbm.h"
#include "canny.h"
#include <math.h>

Matrix magnitude(Matrix m1, Matrix m2) {
    Matrix magnitudeMatrix = createMatrix(m1.height, m1.width);
    for (int y = 0; y < m1.height; y++) {
        for (int x = 0; x < m1.width; x++) {
            magnitudeMatrix.map[y][x] = sqrt(m1.map[y][x] * m1.map[y][x] + m2.map[y][x] * m2.map[y][x]);
        }
    }
    return magnitudeMatrix;
}

/* Convert two matrices into an orientation matrix */
Matrix orientation(Matrix m1, Matrix m2) {
    Matrix orientationMatrix = createMatrix(m1.height, m1.width);
    for (int y = 0; y < m1.height; y++) {
        for (int x = 0; x < m1.width; x++) {
            double radian = atan2(m2.map[y][x], m1.map[y][x]);
            double degrees = radian * 180 / M_PI;
            if (degrees < 0) {
                degrees += 360;
            }
            orientationMatrix.map[y][x] = degrees;
        }
    }
    return orientationMatrix;
}

Matrix sector(Matrix m) {
    Matrix sectorMatrix = createMatrix(m.height, m.width);
    for (int y = 0; y < m.height; y++) {
        for (int x = 0; x < m.width; x++) {
            double angle = m.map[y][x];

            if ((angle >= 337.5 && angle <= 360) ||
                (angle >= 0 && angle <= 22.5) ||
                (angle >= 157.5 && angle <= 202.5)) {
                sectorMatrix.map[y][x] = 0;
            }
            else if ((angle >= 22.5 && angle <= 67.5) ||
                    (angle >= 202.5 && angle <= 247.5)) {
                sectorMatrix.map[y][x] = 1;
            }
            else if ((angle >= 67.5 && angle <= 112.5) ||
                    (angle >= 247.5 && angle <= 292.5)) {
                sectorMatrix.map[y][x] = 2;
            }
            else if ((angle >= 112.5 && angle <= 157.5) ||
                    (angle >= 292.5 && angle <= 337.5)) {
                sectorMatrix.map[y][x] = 3;
            }
        }
    }
    return sectorMatrix;
}

Matrix nonMaximaSuppression(Matrix m, Matrix s) {
    Matrix nonMaximaSuppressedMatrix = createMatrix(m.height, m.width);
    for (int y = 0; y < m.height; y++) {
        for (int x = 0; x < m.width; x++) {
            // Check value of sector at [y][x]
            double sectorVal = s.map[y][x];
            nonMaximaSuppressedMatrix.map[y][x] = m.map[y][x];
            if (sectorVal == 0) {
                // Check top and bottom neighbor
                if (y > 0 && m.map[y - 1][x] > m.map[y][x]) {
                    nonMaximaSuppressedMatrix.map[y][x] = 0;
                }

                if (y < m.height - 1 && m.map[y + 1][x] > m.map[y][x]) {
                    nonMaximaSuppressedMatrix.map[y][x] = 0;
                }

            } else if (sectorVal == 1) {
                // Check top-left and bottom-right neighbor
                if (y > 0 && x > 0 && m.map[y - 1][x - 1] > m.map[y][x]) {
                    nonMaximaSuppressedMatrix.map[y][x] = 0;
                }

                if (y < m.height - 1 && x < m.width - 1 && m.map[y + 1][x + 1] > m.map[y][x]) {
                    nonMaximaSuppressedMatrix.map[y][x] = 0;
                }

            } else if (sectorVal == 2) {
                // Check left and right neighbor
                if (x > 0 && m.map[y][x - 1] > m.map[y][x]) {
                    nonMaximaSuppressedMatrix.map[y][x] = 0;
                }

                if (x < m.width - 1 && m.map[y][x + 1] > m.map[y][x]) {
                    nonMaximaSuppressedMatrix.map[y][x] = 0;
                }

            } else if (sectorVal == 3) {
                // Check top-right and bottom-left neighbor
                if (y > 0 && x < m.width - 1 && m.map[y - 1][x + 1] > m.map[y][x]) {
                    nonMaximaSuppressedMatrix.map[y][x] = 0;
                }

                if (y < m.height - 1 && x > 0 && m.map[y + 1][x - 1] > m.map[y][x]) {
                    nonMaximaSuppressedMatrix.map[y][x] = 0;
                }

            }
        }
    }
    return nonMaximaSuppressedMatrix;
}

Matrix hysteresisThresholding(Matrix m, int lowThreshold, int highThreshold) {
    Matrix hysteresisThresholdingMatrix = createMatrix(m.height, m.width);

    // STAGE 1
    for (int y = 0; y < m.height; y++) {
        for (int x = 0; x < m.width; x++) {
            if (m.map[y][x] >= highThreshold) {
                hysteresisThresholdingMatrix.map[y][x] = 255; // EDGE
            } else if (m.map[y][x] >= lowThreshold) {
                hysteresisThresholdingMatrix.map[y][x] = 1; // CANDIDATE
            } else {
                hysteresisThresholdingMatrix.map[y][x] = 0; // NO EDGE
            }
        }
    }

    // STAGE 2
    for (int y = 0; y < m.height; y++) {
        for (int x = 0; x < m.width; x++) {
            if (hysteresisThresholdingMatrix.map[y][x] == 1) {
                // Check top
                if (y > 0 && hysteresisThresholdingMatrix.map[y - 1][x] == 255) {
                    hysteresisThresholdingMatrix.map[y][x] = 255;
                }

                // Check top-left
                else if (y > 0 && x > 0 && hysteresisThresholdingMatrix.map[y - 1][x - 1] == 255) {
                    hysteresisThresholdingMatrix.map[y][x] = 255;
                }

                // Check top-right
                else if (y > 0 && x < m.width - 1 && hysteresisThresholdingMatrix.map[y - 1][x + 1] == 255) {
                    hysteresisThresholdingMatrix.map[y][x] = 255;
                }

                // Check bottom
                else if (y < m.height - 1 && hysteresisThresholdingMatrix.map[y + 1][x] == 255) {
                    hysteresisThresholdingMatrix.map[y][x] = 255;
                }

                // Check bottom-left
                else if (y < m.height - 1 && x > 0 && hysteresisThresholdingMatrix.map[y + 1][x - 1] == 255) {
                    hysteresisThresholdingMatrix.map[y][x] = 255;
                }

                // Check bottom-right
                else if (y < m.height - 1 && x < m.width - 1 && hysteresisThresholdingMatrix.map[y + 1][x + 1] == 255) {
                    hysteresisThresholdingMatrix.map[y][x] = 255;
                }

                // Check left
                else if (x > 0 && hysteresisThresholdingMatrix.map[y][x - 1] == 255) {
                    hysteresisThresholdingMatrix.map[y][x] = 255;
                }

                // Check right
                else if (x < m.width - 1 && hysteresisThresholdingMatrix.map[y][x + 1] == 255) {
                    hysteresisThresholdingMatrix.map[y][x] = 255;
                }

                else {
                    hysteresisThresholdingMatrix.map[y][x] = 0;
                }
            }
        }
    }
    return hysteresisThresholdingMatrix;
}

Matrix cannyEdgeDetection(Matrix inputMatrix, int lowThreshold, int highThreshold) {
    double p[2][2] = {{-0.5, -0.5}, {0.5, 0.5}};
    double q[2][2] = {{-0.5, 0.5}, {-0.5, 0.5}};
    Matrix pMatrix = createMatrixFromArray(&p[0][0], 2, 2);
    Matrix qMatrix = createMatrixFromArray(&q[0][0], 2, 2);

    // Convolve input with p and q to get P and Q
    Matrix P = convolve(inputMatrix, pMatrix);
    Matrix Q = convolve(inputMatrix, qMatrix);
    
    Matrix mag = magnitude(P, Q);
    Matrix orient = orientation(P, Q);
    Matrix sect = sector(orient);
    Matrix nonMaxSuppressed = nonMaximaSuppression(mag, sect);
    Matrix hysteresisThresholded = hysteresisThresholding(nonMaxSuppressed, lowThreshold, highThreshold);
    deleteMatrix(mag);
    deleteMatrix(orient);
    deleteMatrix(sect);
    deleteMatrix(nonMaxSuppressed);
    return hysteresisThresholded;
}