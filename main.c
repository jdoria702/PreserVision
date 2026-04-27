#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "gaussian.h"
#include <stdio.h>

int main(int argc, const char * argv[]) {
       // Usage: ./main gaussian input_path output_path kernel_size sigma
       if (strcmp(argv[1], "gaussian") == 0) {
              if (argc != 6) {
                     fprintf(stderr, "Usage: %s input_image.ppm output_image.ppm kernel_size sigma\n", argv[0]);
                     return 1;
              }
              Image inputImage = readImage((char *)argv[2]);
              Matrix inputMatrix = image2Matrix(inputImage);
              Matrix blurred = gaussianBlur(inputMatrix, atoi(argv[4]), atof(argv[5]));
              Image outputImage = matrix2Image(blurred, 0, 1.0);
              writeImage(outputImage, (char *)argv[3]);
              deleteImage(inputImage);
              deleteImage(outputImage);
              deleteMatrix(inputMatrix);
              deleteMatrix(blurred);
       }

       printf("Program ends ... ");
       return 0;
}