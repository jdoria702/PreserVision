#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Laws_Texture_Images/laws_texture.h"
#include "Texture_Segmentation_Images/texture_segmentation.h"
#include <stdio.h>

#include "Gaussian_Filter_Images/gaussian.h"
#include "Canny_Filter_Images/canny.h"
#include "Hough_Transform_Images/hough.h"
#include "irregular_shape_detector.h"
#include "Distortion_Images/gaussian_noise.h"
#include "Distortion_Images/blur_distortion.h"
#include "Distortion_Images/low_contrast.h"
#include "netpbm.h"

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
       
       // Usage: ./main hough input_path output_path g_size g_sigma canny_low canny_high r_min r_max vote_threshold
       else if (strcmp(argv[1], "hough") == 0) {
              if (argc != 11) {
                     fprintf(stderr, "Usage: %s hough input_image.ppm output_image.ppm g_size g_sigma canny_low canny_high r_min r_max arc_pct\n"
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
       }
       
       // Usage: ./main better_coral_detector input_path output_path min_area max_area
       else if (strcmp(argv[1], "better_coral_detector") == 0) {
              if (argc != 6) {
                     fprintf(stderr, "Usage: %s better_coral_detector input_image.ppm output_image.ppm min_area max_area\n", argv[0]);
                     return 1;
              }

              Image inputImage = readImage((char *)argv[2]);
              Image outputImage = detectCoralBlobs(inputImage, atoi(argv[4]), atoi(argv[5]));

              writeImage(outputImage, (char *)argv[3]);

              deleteImage(inputImage);
              deleteImage(outputImage);

       // Usage: ./main gaussian_noise input_path output_path sigma
       } else if (strcmp(argv[1], "gaussian_noise") == 0) {
              if (argc != 5) {
                     fprintf(stderr, "Usage: %s gaussian_noise input_image.ppm output_image.ppm sigma\n", argv[0]);
                     return 1;
              }
              Image inputImage = readImage((char *)argv[2]);
              Image outputImage = addGaussianNoise(inputImage, atof(argv[4]));
              writeImage(outputImage, (char *)argv[3]);
              deleteImage(inputImage);
              deleteImage(outputImage);

       // Usage: ./main blur_distortion input_path output_path kernel_size
       } else if (strcmp(argv[1], "blur_distortion") == 0) {
              if (argc != 5) {
                     fprintf(stderr, "Usage: %s blur_distortion input_image.ppm output_image.ppm kernel_size\n", argv[0]);
                     return 1;
              }
              Image inputImage = readImage((char *)argv[2]);
              Image outputImage = applyBlurDistortion(inputImage, atoi(argv[4]));
              writeImage(outputImage, (char *)argv[3]);
              deleteImage(inputImage);
              deleteImage(outputImage);

       // Usage: ./main low_contrast input_path output_path factor
       // factor in [0.0, 1.0]: 0.0 = flat gray, 1.0 = original
       } else if (strcmp(argv[1], "low_contrast") == 0) {
              if (argc != 5) {
                     fprintf(stderr, "Usage: %s low_contrast input_image.ppm output_image.ppm factor\n", argv[0]);
                     return 1;
              }
              Image inputImage = readImage((char *)argv[2]);
              Image outputImage = applyLowContrast(inputImage, atof(argv[4]));
              writeImage(outputImage, (char *)argv[3]);
              deleteImage(inputImage);
              deleteImage(outputImage);

       } else {
              fprintf(stderr, "Unknown filter: %s\n", argv[1]);
              return 1;
       }

       printf("Program ends ... ");
       return 0;
}