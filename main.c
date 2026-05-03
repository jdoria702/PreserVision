#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "gaussian.h"
#include "canny.h"
#include "hough.h"

int main(int argc, const char * argv[]) {
       // Usage: ./main gaussian input_path output_path kernel_size sigma
       if (strcmp(argv[1], "gaussian") == 0) {
              if (argc != 6) {
                     fprintf(stderr, "Usage: %s gaussian input_image.ppm output_image.ppm kernel_size sigma\n", argv[0]);
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

       // Usage: ./main sobel input_path output_path
       else if (strcmp(argv[1], "sobel") == 0) {
              fprintf(stderr, "Sobel filter not implemented yet.\n");
              return 1;


       // Usage: ./main canny input_path output_path low_threshold high_threshold
       } else if (strcmp(argv[1], "canny") == 0) {
              if (argc != 6) {
                     fprintf(stderr, "Usage: %s canny input_image.ppm output_image.ppm low_threshold high_threshold\n", argv[0]);
                     return 1;
              }
              Image inputImage = readImage((char *)argv[2]);
              Matrix inputMatrix = image2Matrix(inputImage);
              Matrix canny = cannyEdgeDetection(inputMatrix, atoi(argv[4]), atoi(argv[5]));
              Image outputImage = matrix2Image(canny, 0, 1.0);
              writeImage(outputImage, (char *)argv[3]);
              deleteImage(inputImage);
              deleteImage(outputImage);
              deleteMatrix(inputMatrix);
              deleteMatrix(canny);
       // Usage: ./main hough input_path output_path g_size g_sigma canny_low canny_high r_min r_max vote_threshold
       } else if (strcmp(argv[1], "hough") == 0) {
              if (argc != 11) {
                     fprintf(stderr, "Usage: %s hough input.ppm output.ppm g_size g_sigma canny_low canny_high r_min r_max arc_pct\n"
                                     "  arc_pct: minimum %% of circle circumference that must be an edge (0-100)\n", argv[0]);
                     return 1;
              }
              Image inputImage   = readImage((char *)argv[2]);
              Matrix inputMatrix = image2Matrix(inputImage);
              Matrix blurred     = gaussianBlur(inputMatrix, atoi(argv[4]), atof(argv[5]));
              Matrix edges       = cannyEdgeDetection(blurred, atoi(argv[6]), atoi(argv[7]));
              Image outputImage  = houghCircleDetection(edges, inputImage,
                                                        atoi(argv[8]), atoi(argv[9]), atoi(argv[10]));
              writeImage(outputImage, (char *)argv[3]);
              deleteMatrix(inputMatrix);
              deleteMatrix(blurred);
              deleteMatrix(edges);
              deleteImage(inputImage);
              deleteImage(outputImage);

       } else {
              fprintf(stderr, "Unknown filter: %s\n", argv[1]);
              return 1;
       }

       printf("Program ends ... ");
       return 0;
}