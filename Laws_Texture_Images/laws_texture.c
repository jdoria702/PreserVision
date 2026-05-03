#include "laws_texture.h"
#include "netpbm.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

char *make_output_path(const char *dir, const char *suffix) {
    size_t needed = strlen(dir) + 1 + strlen("LawsTexture") + 1 + strlen(suffix) + strlen(".ppm") + 1;
    char *out = malloc(needed);
    if (!out) return NULL;
    snprintf(out, needed, "%s/%s_%s.ppm", dir, "LawsTexture", suffix);
    return out;
}

void createLawsTextureEnergyMap(Matrix input, char *outputPath, int patchSize) {
    // Define the 5 Laws' texture kernels (L5, E5, S5, W5, R5)
    double L5[5] = {1, 4, 6, 4, 1};
    double E5[5] = {-1, -2, 0, 2, 1};
    double S5[5] = {-1, 0, 2, 0, -1};
    double R5[5] = {1, -4, 6, -4, 1};
    double W5[5] = {-1, 2, 0, -2, 1};

    // Multiply vectors to make 5x5 Law's masks
    double L5L5[5][5], L5E5[5][5], L5S5[5][5], L5R5[5][5], L5W5[5][5];
    double E5L5[5][5], E5E5[5][5], E5S5[5][5], E5R5[5][5], E5W5[5][5];
    double S5L5[5][5], S5E5[5][5], S5S5[5][5], S5R5[5][5], S5W5[5][5];
    double R5L5[5][5], R5E5[5][5], R5S5[5][5], R5R5[5][5], R5W5[5][5];
    double W5L5[5][5], W5E5[5][5], W5S5[5][5], W5R5[5][5], W5W5[5][5];

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            L5L5[i][j] = L5[i] * L5[j];
            L5E5[i][j] = L5[i] * E5[j];
            L5S5[i][j] = L5[i] * S5[j];
            L5R5[i][j] = L5[i] * R5[j];
            L5W5[i][j] = L5[i] * W5[j];

            E5L5[i][j] = E5[i] * L5[j];
            E5E5[i][j] = E5[i] * E5[j];
            E5S5[i][j] = E5[i] * S5[j];
            E5R5[i][j] = E5[i] * R5[j];
            E5W5[i][j] = E5[i] * W5[j];

            S5L5[i][j] = S5[i] * L5[j];
            S5E5[i][j] = S5[i] * E5[j];
            S5S5[i][j] = S5[i] * S5[j];
            S5R5[i][j] = S5[i] * R5[j];
            S5W5[i][j] = S5[i] * W5[j];

            R5L5[i][j] = R5[i] * L5[j];
            R5E5[i][j] = R5[i] * E5[j];
            R5S5[i][j] = R5[i] * S5[j];
            R5R5[i][j] = R5[i] * R5[j];
            R5W5[i][j] = R5[i] * W5[j];

            W5L5[i][j] = W5[i] * L5[j];
            W5E5[i][j] = W5[i] * E5[j];
            W5S5[i][j] = W5[i] * S5[j];
            W5R5[i][j] = W5[i] * R5[j];
            W5W5[i][j] = W5[i] * W5[j];
        }
    }

    // Convert each mask to matrix
    Matrix L5L5Matrix = createMatrix(5, 5);
    Matrix L5E5Matrix = createMatrix(5, 5);
    Matrix L5S5Matrix = createMatrix(5, 5);
    Matrix L5R5Matrix = createMatrix(5, 5);
    Matrix L5W5Matrix = createMatrix(5, 5);
    Matrix E5L5Matrix = createMatrix(5, 5);
    Matrix E5E5Matrix = createMatrix(5, 5);
    Matrix E5S5Matrix = createMatrix(5, 5);
    Matrix E5R5Matrix = createMatrix(5, 5);
    Matrix E5W5Matrix = createMatrix(5, 5);
    Matrix S5L5Matrix = createMatrix(5, 5);
    Matrix S5E5Matrix = createMatrix(5, 5);
    Matrix S5S5Matrix = createMatrix(5, 5);
    Matrix S5R5Matrix = createMatrix(5, 5);
    Matrix S5W5Matrix = createMatrix(5, 5);
    Matrix R5L5Matrix = createMatrix(5, 5);
    Matrix R5E5Matrix = createMatrix(5, 5);
    Matrix R5S5Matrix = createMatrix(5, 5);
    Matrix R5R5Matrix = createMatrix(5, 5);
    Matrix R5W5Matrix = createMatrix(5, 5);
    Matrix W5L5Matrix = createMatrix(5, 5);
    Matrix W5E5Matrix = createMatrix(5, 5);
    Matrix W5S5Matrix = createMatrix(5, 5);
    Matrix W5R5Matrix = createMatrix(5, 5);
    Matrix W5W5Matrix = createMatrix(5, 5);

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            L5L5Matrix.map[i][j] = L5L5[i][j];
            L5E5Matrix.map[i][j] = L5E5[i][j];
            L5S5Matrix.map[i][j] = L5S5[i][j];
            L5R5Matrix.map[i][j] = L5R5[i][j];
            L5W5Matrix.map[i][j] = L5W5[i][j];

            E5L5Matrix.map[i][j] = E5L5[i][j];
            E5E5Matrix.map[i][j] = E5E5[i][j];
            E5S5Matrix.map[i][j] = E5S5[i][j];
            E5R5Matrix.map[i][j] = E5R5[i][j];
            E5W5Matrix.map[i][j] = E5W5[i][j];

            S5L5Matrix.map[i][j] = S5L5[i][j];
            S5E5Matrix.map[i][j] = S5E5[i][j];
            S5S5Matrix.map[i][j] = S5S5[i][j];
            S5R5Matrix.map[i][j] = S5R5[i][j];
            S5W5Matrix.map[i][j] = S5W5[i][j];

            R5L5Matrix.map[i][j] = R5L5[i][j];
            R5E5Matrix.map[i][j] = R5E5[i][j];
            R5S5Matrix.map[i][j] = R5S5[i][j];
            R5R5Matrix.map[i][j] = R5R5[i][j];
            R5W5Matrix.map[i][j] = R5W5[i][j];

            W5L5Matrix.map[i][j] = W5L5[i][j];
            W5E5Matrix.map[i][j] = W5E5[i][j];
            W5S5Matrix.map[i][j] = W5S5[i][j];
            W5R5Matrix.map[i][j] = W5R5[i][j];
            W5W5Matrix.map[i][j] = W5W5[i][j];
        }
    }

    // Convolve input with each mask and save image
    Matrix L5L5Convolved = convolve(input, L5L5Matrix);
    Matrix L5E5Convolved = convolve(input, L5E5Matrix);
    Matrix L5S5Convolved = convolve(input, L5S5Matrix);
    Matrix L5R5Convolved = convolve(input, L5R5Matrix);
    Matrix L5W5Convolved = convolve(input, L5W5Matrix);

    Matrix E5L5Convolved = convolve(input, E5L5Matrix);
    Matrix E5E5Convolved = convolve(input, E5E5Matrix);
    Matrix E5S5Convolved = convolve(input, E5S5Matrix);
    Matrix E5R5Convolved = convolve(input, E5R5Matrix);
    Matrix E5W5Convolved = convolve(input, E5W5Matrix);

    Matrix S5L5Convolved = convolve(input, S5L5Matrix);
    Matrix S5E5Convolved = convolve(input, S5E5Matrix);
    Matrix S5S5Convolved = convolve(input, S5S5Matrix);
    Matrix S5R5Convolved = convolve(input, S5R5Matrix);
    Matrix S5W5Convolved = convolve(input, S5W5Matrix);

    Matrix R5L5Convolved = convolve(input, R5L5Matrix);
    Matrix R5E5Convolved = convolve(input, R5E5Matrix);
    Matrix R5S5Convolved = convolve(input, R5S5Matrix);
    Matrix R5R5Convolved = convolve(input, R5R5Matrix);
    Matrix R5W5Convolved = convolve(input, R5W5Matrix);

    Matrix W5L5Convolved = convolve(input, W5L5Matrix);
    Matrix W5E5Convolved = convolve(input, W5E5Matrix);
    Matrix W5S5Convolved = convolve(input, W5S5Matrix);
    Matrix W5R5Convolved = convolve(input, W5R5Matrix);
    Matrix W5W5Convolved = convolve(input, W5W5Matrix);

    // Divide image into 8x8 patches and compute energy (sum of absolute values) for each patch
    int numPatchesX = L5L5Convolved.width / patchSize;
    int numPatchesY = L5L5Convolved.height / patchSize;
    int rows = numPatchesY * numPatchesX;
    int cols = 25;
    
    // initialize vectors to hold energy values for each patch and each mask
    int **energyValue = (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++) {
        energyValue[i] = (int *)calloc(cols, sizeof(int));
    }

    // Create feature vector for each patch (row) and each mask (column)
    for (int y = 0; y < numPatchesY; y++) {
        for (int x = 0; x < numPatchesX; x++) {
            for (int j = 0; j < patchSize; j++) {
                for (int i = 0; i < patchSize; i++) {
                    int idxX = x * patchSize + i;
                    int idxY = y * patchSize + j;
                    energyValue[y * numPatchesX + x][0] += fabs(L5L5Convolved.map[idxY][idxX]);
                    energyValue[y * numPatchesX + x][1] += fabs(L5E5Convolved.map[idxY][idxX]);
                    energyValue[y * numPatchesX + x][2] += fabs(L5S5Convolved.map[idxY][idxX]);
                    energyValue[y * numPatchesX + x][3] += fabs(L5R5Convolved.map[idxY][idxX]);
                    energyValue[y * numPatchesX + x][4] += fabs(L5W5Convolved.map[idxY][idxX]);
                    energyValue[y * numPatchesX + x][5] += fabs(E5L5Convolved.map[idxY][idxX]);
                    energyValue[y * numPatchesX + x][6] += fabs(E5E5Convolved.map[idxY][idxX]);
                    energyValue[y * numPatchesX + x][7] += fabs(E5S5Convolved.map[idxY][idxX]);
                    energyValue[y * numPatchesX + x][8] += fabs(E5R5Convolved.map[idxY][idxX]);
                    energyValue[y * numPatchesX + x][9] += fabs(E5W5Convolved.map[idxY][idxX]);
                    energyValue[y * numPatchesX + x][10] += fabs(S5L5Convolved.map[idxY][idxX]);
                    energyValue[y * numPatchesX + x][11] += fabs(S5E5Convolved.map[idxY][idxX]);
                    energyValue[y * numPatchesX + x][12] += fabs(S5S5Convolved.map[idxY][idxX]);
                    energyValue[y * numPatchesX + x][13] += fabs(S5R5Convolved.map[idxY][idxX]);
                    energyValue[y * numPatchesX + x][14] += fabs(S5W5Convolved.map[idxY][idxX]);
                    energyValue[y * numPatchesX + x][15] += fabs(R5L5Convolved.map[idxY][idxX]);
                    energyValue[y * numPatchesX + x][16] += fabs(R5E5Convolved.map[idxY][idxX]);
                    energyValue[y * numPatchesX + x][17] += fabs(R5S5Convolved.map[idxY][idxX]);
                    energyValue[y * numPatchesX + x][18] += fabs(R5R5Convolved.map[idxY][idxX]);
                    energyValue[y * numPatchesX + x][19] += fabs(R5W5Convolved.map[idxY][idxX]);
                    energyValue[y * numPatchesX + x][20] += fabs(W5L5Convolved.map[idxY][idxX]);
                    energyValue[y * numPatchesX + x][21] += fabs(W5E5Convolved.map[idxY][idxX]);
                    energyValue[y * numPatchesX + x][22] += fabs(W5S5Convolved.map[idxY][idxX]);
                    energyValue[y * numPatchesX + x][23] += fabs(W5R5Convolved.map[idxY][idxX]);
                    energyValue[y * numPatchesX + x][24] += fabs(W5W5Convolved.map[idxY][idxX]);
                }
            }
        }
    }

    // build CSV path (separate from make_output_path which makes .ppm)
    size_t csvNeeded = strlen(outputPath) + 1 + strlen("LawsTexture") + 1 + strlen("energy_values") + strlen(".csv") + 1;
    char *csvPath = malloc(csvNeeded);
    if (csvPath) {
        snprintf(csvPath, csvNeeded, "%s/%s_%s.csv", outputPath, "LawsTexture", "energy_values");
    }
    FILE *csvFile = csvPath ? fopen(csvPath, "w") : NULL;
    if (csvFile) {
        const char *headers[25] = {
            "L5L5", "L5E5", "L5S5", "L5R5", "L5W5",
            "E5L5", "E5E5", "E5S5", "E5R5", "E5W5",
            "S5L5", "S5E5", "S5S5", "S5R5", "S5W5",
            "R5L5", "R5E5", "R5S5", "R5R5", "R5W5",
            "W5L5", "W5E5", "W5S5", "W5R5", "W5W5"
        };

        // header
        fprintf(csvFile, "patch_id");
        for (int h = 0; h < cols; h++) fprintf(csvFile, ",%s", headers[h]);
        fputc('\n', csvFile);

        // rows: first column = patch id
        for (int r = 0; r < rows; r++) {
            fprintf(csvFile, "%d", r);
            for (int c = 0; c < cols; c++) {
                fprintf(csvFile, ",%d", energyValue[r][c]);
            }
            fputc('\n', csvFile);
        }
        fclose(csvFile);
    }
    free(csvPath);


    // Convert matrices to images
    Image L5L5Image = matrix2Image(L5L5Convolved, 0, 1.0);
    Image L5E5Image = matrix2Image(L5E5Convolved, 0, 1.0);
    Image L5S5Image = matrix2Image(L5S5Convolved, 0, 1.0);
    Image L5R5Image = matrix2Image(L5R5Convolved, 0, 1.0);
    Image L5W5Image = matrix2Image(L5W5Convolved, 0, 1.0);

    Image E5L5Image = matrix2Image(E5L5Convolved, 0, 1.0);
    Image E5E5Image = matrix2Image(E5E5Convolved, 0, 1.0);
    Image E5S5Image = matrix2Image(E5S5Convolved, 0, 1.0);
    Image E5R5Image = matrix2Image(E5R5Convolved, 0, 1.0);
    Image E5W5Image = matrix2Image(E5W5Convolved, 0, 1.0);

    Image S5L5Image = matrix2Image(S5L5Convolved, 0, 1.0);
    Image S5E5Image = matrix2Image(S5E5Convolved, 0, 1.0);
    Image S5S5Image = matrix2Image(S5S5Convolved, 0, 1.0);
    Image S5R5Image = matrix2Image(S5R5Convolved, 0, 1.0);
    Image S5W5Image = matrix2Image(S5W5Convolved, 0, 1.0);

    Image R5L5Image = matrix2Image(R5L5Convolved, 0, 1.0);
    Image R5E5Image = matrix2Image(R5E5Convolved, 0, 1.0);
    Image R5S5Image = matrix2Image(R5S5Convolved, 0, 1.0);
    Image R5R5Image = matrix2Image(R5R5Convolved, 0, 1.0);
    Image R5W5Image = matrix2Image(R5W5Convolved, 0, 1.0);

    Image W5L5Image = matrix2Image(W5L5Convolved, 0, 1.0);
    Image W5E5Image = matrix2Image(W5E5Convolved, 0, 1.0);
    Image W5S5Image = matrix2Image(W5S5Convolved, 0, 1.0);
    Image W5R5Image = matrix2Image(W5R5Convolved, 0, 1.0);
    Image W5W5Image = matrix2Image(W5W5Convolved, 0, 1.0);

    // Save images
    char *p = make_output_path(outputPath, "L5L5");
    writeImage(L5L5Image, p);
    free(p);

    p = make_output_path(outputPath, "L5E5");
    writeImage(L5E5Image, p);
    free(p);

    p = make_output_path(outputPath, "L5S5");
    writeImage(L5S5Image, p);
    free(p);

    p = make_output_path(outputPath, "L5R5");
    writeImage(L5R5Image, p);
    free(p);

    p = make_output_path(outputPath, "L5W5");
    writeImage(L5W5Image, p);
    free(p);

    p = make_output_path(outputPath, "E5L5");
    writeImage(E5L5Image, p);
    free(p);

    p = make_output_path(outputPath, "E5E5");
    writeImage(E5E5Image, p);
    free(p);

    p = make_output_path(outputPath, "E5S5");
    writeImage(E5S5Image, p);
    free(p);

    p = make_output_path(outputPath, "E5R5");
    writeImage(E5R5Image, p);
    free(p);

    p = make_output_path(outputPath, "E5W5");
    writeImage(E5W5Image, p);
    free(p);

    p = make_output_path(outputPath, "S5L5");
    writeImage(S5L5Image, p);
    free(p);

    p = make_output_path(outputPath, "S5E5");
    writeImage(S5E5Image, p);
    free(p);

    p = make_output_path(outputPath, "S5S5");
    writeImage(S5S5Image, p);
    free(p);

    p = make_output_path(outputPath, "S5R5");
    writeImage(S5R5Image, p);
    free(p);

    p = make_output_path(outputPath, "S5W5");
    writeImage(S5W5Image, p);
    free(p);

    p = make_output_path(outputPath, "R5L5");
    writeImage(R5L5Image, p);
    free(p);

    p = make_output_path(outputPath, "R5E5");
    writeImage(R5E5Image, p);
    free(p);

    p = make_output_path(outputPath, "R5S5");
    writeImage(R5S5Image, p);
    free(p);

    p = make_output_path(outputPath, "R5R5");
    writeImage(R5R5Image, p);
    free(p);

    p = make_output_path(outputPath, "R5W5");
    writeImage(R5W5Image, p);
    free(p);

    p = make_output_path(outputPath, "W5L5");
    writeImage(W5L5Image, p);
    free(p);

    p = make_output_path(outputPath, "W5E5");
    writeImage(W5E5Image, p);
    free(p);

    p = make_output_path(outputPath, "W5S5");
    writeImage(W5S5Image, p);
    free(p);

    p = make_output_path(outputPath, "W5R5");
    writeImage(W5R5Image, p);
    free(p);

    p = make_output_path(outputPath, "W5W5");
    writeImage(W5W5Image, p);
    free(p);

    // Clean up
    deleteMatrix(L5L5Matrix);
    deleteMatrix(L5E5Matrix);
    deleteMatrix(L5S5Matrix);
    deleteMatrix(L5R5Matrix);
    deleteMatrix(L5W5Matrix);
    deleteMatrix(E5L5Matrix);
    deleteMatrix(E5E5Matrix);
    deleteMatrix(E5S5Matrix);
    deleteMatrix(E5R5Matrix);
    deleteMatrix(E5W5Matrix);
    deleteMatrix(S5L5Matrix);
    deleteMatrix(S5E5Matrix);
    deleteMatrix(S5S5Matrix);
    deleteMatrix(S5R5Matrix);
    deleteMatrix(S5W5Matrix);
    deleteMatrix(R5L5Matrix);
    deleteMatrix(R5E5Matrix);
    deleteMatrix(R5S5Matrix);
    deleteMatrix(R5R5Matrix);
    deleteMatrix(R5W5Matrix);
    deleteMatrix(W5L5Matrix);
    deleteMatrix(W5E5Matrix);
    deleteMatrix(W5S5Matrix);
    deleteMatrix(W5R5Matrix);
    deleteMatrix(W5W5Matrix);

    deleteMatrix(L5L5Convolved);
    deleteMatrix(L5E5Convolved);
    deleteMatrix(L5S5Convolved);
    deleteMatrix(L5R5Convolved);
    deleteMatrix(L5W5Convolved);
    deleteMatrix(E5L5Convolved);
    deleteMatrix(E5E5Convolved);
    deleteMatrix(E5S5Convolved);
    deleteMatrix(E5R5Convolved);
    deleteMatrix(E5W5Convolved);
    deleteMatrix(S5L5Convolved);
    deleteMatrix(S5E5Convolved);
    deleteMatrix(S5S5Convolved);
    deleteMatrix(S5R5Convolved);
    deleteMatrix(S5W5Convolved);
    deleteMatrix(R5L5Convolved);
    deleteMatrix(R5E5Convolved);
    deleteMatrix(R5S5Convolved);
    deleteMatrix(R5R5Convolved);
    deleteMatrix(R5W5Convolved);
    deleteMatrix(W5L5Convolved);
    deleteMatrix(W5E5Convolved);
    deleteMatrix(W5S5Convolved);
    deleteMatrix(W5R5Convolved);
    deleteMatrix(W5W5Convolved);

    deleteImage(L5L5Image);
    deleteImage(L5E5Image);
    deleteImage(L5S5Image);
    deleteImage(L5R5Image);
    deleteImage(L5W5Image);
    deleteImage(E5L5Image);
    deleteImage(E5E5Image);
    deleteImage(E5S5Image);
    deleteImage(E5R5Image);
    deleteImage(E5W5Image);
    deleteImage(S5L5Image);
    deleteImage(S5E5Image);
    deleteImage(S5S5Image);
    deleteImage(S5R5Image);
    deleteImage(S5W5Image);
    deleteImage(R5L5Image);
    deleteImage(R5E5Image);
    deleteImage(R5S5Image);
    deleteImage(R5R5Image);
    deleteImage(R5W5Image);
    deleteImage(W5L5Image);
    deleteImage(W5E5Image);
    deleteImage(W5S5Image);
    deleteImage(W5R5Image);
    deleteImage(W5W5Image);

    return;

}