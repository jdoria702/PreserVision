#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "gaussian.h"
#include "canny.h"
#include "Laws_Texture_Images/laws_texture.h"
#include "Texture_Segmentation_Images/texture_segmentation.h"
#include <stdio.h>

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
       } 
       
       // Usage: ./main laws_texture input_path output_path patch_size
       // Will create 25 images for each of the 25 Laws' texture energy maps
       else if (strcmp(argv[1], "laws_texture") == 0) {
              if (argc != 5) {
                     fprintf(stderr, "Usage: %s laws_texture input_image.ppm output_image_prefix patchSize\n", argv[0]);
                     return 1;
              }
              Image inputImage = readImage((char *)argv[2]);
              Matrix inputMatrix = image2Matrix(inputImage);
              createLawsTextureEnergyMap(inputMatrix, (char *)argv[3], atoi(argv[4]));
              deleteImage(inputImage);
              deleteMatrix(inputMatrix);
       }

       // Usage ./main texture_segmentation input_image.ppm label_csv output_image.ppm patch_size
       // Need to have run laws_texture first to generate the label_csv file (which contains patch_id and cluster label for each patch)
       else if (strcmp(argv[1], "texture_segmentation") == 0) {
              if (argc != 6) {
                     fprintf(stderr, "Usage: %s texture_segmentation input_image.ppm label_csv output_image.ppm patchSize\n", argv[0]);
                     return 1;
              }
              Image inputImage = readImage((char *)argv[2]);
              segmentTexture(inputImage, (char *)argv[3], atoi(argv[5]));
              writeImage(inputImage, (char *)argv[4]);
              deleteImage(inputImage);
       }
       
       else {
              fprintf(stderr, "Unknown filter: %s\n", argv[1]);
              return 1;
       }

       printf("Program ends ... ");
       return 0;
}