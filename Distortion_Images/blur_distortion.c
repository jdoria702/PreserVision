#include "blur_distortion.h"

/*
    This function applies a box blur (uniform blur) to the image.
    Essentially, for every pixel, we're replacing it with the average of its neighbors.
*/
Image applyBlurDistortion(Image input, int kernel_size) {
    if (kernel_size < 1 || kernel_size % 2 == 0) {
        fprintf(stderr, "blur_distortion: kernel_size must be a positive odd integer.\n");
        return createImage(input.height, input.width);
    }
    
    int half = kernel_size / 2;
    Image output = createImage(input.height, input.width);

    for (int i = 0; i < input.height; i++) {
        for (int j = 0; j < input.width; j++) {
            double sumR = 0.0, sumG = 0.0, sumB = 0.0;
            int count = 0;

            for (int di = -half; di <= half; di++) {
                for (int dj = -half; dj <= half; dj++) {
                    int ni = i + di;
                    int nj = j + dj;
                    // Clamp to image boundaries
                    if (ni < 0) ni = 0;
                    if (ni >= input.height) ni = input.height - 1;
                    if (nj < 0) nj = 0;
                    if (nj >= input.width) nj = input.width - 1;
                    sumR += input.map[ni][nj].r;
                    sumG += input.map[ni][nj].g;
                    sumB += input.map[ni][nj].b;
                    count++;
                }
            }

            unsigned char r = (unsigned char)(sumR / count + 0.5);
            unsigned char g = (unsigned char)(sumG / count + 0.5);
            unsigned char b = (unsigned char)(sumB / count + 0.5);
            output.map[i][j].r = r;
            output.map[i][j].g = g;
            output.map[i][j].b = b;
            output.map[i][j].i = (unsigned char)((r + g + b) / 3);
        }
    }

    return output;
}
