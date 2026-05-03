#include "irregular_shape_detector.h"
#include <stdlib.h>
#include <stdio.h>

/*
    After working on the Hough Transform to detect corals, we weren't happy with the results. 
    You can review them under the "Hough_Transform_Images" and also read the corresponding .c
    file to find out why the algorithm wasn't ideal for the image we were presented.

    We wanted to test out a new pipeline that we thought would improve our coral detection capabilities.

    Given the coral image:
        1. Use thresholding to find coral pixels.
            - Corals in the image are clearly darker (slightly brown?) than the surroundings.
            - Threshold by color!
            - Output will be a binary image.
        2. Remove any noise.
            - Thresholding may result in noise because as you can see in the image, there are objects that are also dark.
            - We want to try to remove as many of these components as possible.
            - Can try removing small components first to see how this helps.
        3. Group coral pixels into blobs (connect components).
            - Scan the binary mask and group connected pixels into blobs, and ideally each blob corresponds to exactly one coral piece.
        4. Filter the blobs for "bad" components.
            - Can filter by size + shape (because some of the coral pieces are small too, but they are "bloby" and not thin and criss-crossed like the mesh).
        5. Draw the results onto the image.
            - Not drawing circles anymore!
*/

// A "Blob" has a minimum and maximum size, as well as an area:
typedef struct {
    int minX, minY;
    int maxX, maxY;
    int area;
} Blob;

// Define what counts as a coral pixel (generally darker colors = coral pixel):
static int isCoralPixel(Pixel p) {
    int r = p.r;
    int g = p.g;
    int b = p.b;
    int gray = (r + g + b) / 3;

    // Saturation to remove tag:
    int max = (r > g ? (r > b ? r : b) : (g > b ? g : b));
    int min = (r < g ? (r < b ? r : b) : (g < b ? g : b));
    int saturation = max - min;

    if (saturation > 60) {
        return 0;
    }

    // TUNE THESE LATER IF NEEDED:
    return gray < 130 && r >= g && g >= b;
}

// SHRINK (ERODE):
void erode(unsigned char **mask, int height, int width) {
    unsigned char **temp = malloc(height * sizeof(unsigned char *));
    for (int y = 0; y < height; y++) {
        temp[y] = calloc(width, sizeof(unsigned char));
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            int keep = 1;

            for (int dy = -1; dy <= 1 && keep; dy++) {
                for (int dx = -1; dx <= 1 && keep; dx++) {

                    int ny = y + dy;
                    int nx = x + dx;

                    if (ny < 0 || ny >= height || nx < 0 || nx >= width || mask[ny][nx] == 0) {
                        keep = 0;
                    }
                }
            }

            if (keep) {
                temp[y][x] = 255;
            }
        }
    }

    // Copy back:
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            mask[y][x] = temp[y][x];
        }
        free(temp[y]);
    }
    free(temp);
}

// EXPAND (DILATE):
void dilate(unsigned char **mask, int height, int width) {
    unsigned char **temp = malloc(height * sizeof(unsigned char *));
    for (int y = 0; y < height; y++) {
        temp[y] = calloc(width, sizeof(unsigned char));
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            if (mask[y][x] == 255) {
                // Turn all neighbors on:
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {

                        int ny = y + dy;
                        int nx = x + dx;

                        if (ny >= 0 && ny < height && nx >= 0 && nx < width) {
                            temp[ny][nx] = 255;
                        }
                    }
                }
            }
        }
    }

    // Copy back:
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            mask[y][x] = temp[y][x];
        }
        free(temp[y]);
    }
    free(temp);
}

// Algorithm as summarized above:
Image detectCoralBlobs(Image input, int minArea, int maxArea) {
    int height = input.height;
    int width = input.width;

    Image out = createImage(height, width);

    // Copy the original image:
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            out.map[y][x] = input.map[y][x];
        }
    }

    // "mask" is an array of pointers to rows, and each row is an array of unsigned char values.
    // Same goes for "visited".
    unsigned char **mask = malloc(height * sizeof(unsigned char *));
    unsigned char **visited = malloc(height * sizeof(unsigned char *));

    for (int y = 0; y < height; y++) {
        mask[y] = calloc(width, sizeof(unsigned char));
        visited[y] = calloc(width, sizeof(unsigned char));
    }

    // Build the binary coral mask:
    // 255 = likely coral.
    // 0 = probably not coral.
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (isCoralPixel(input.map[y][x])) {
                mask[y][x] = 255;
            }
        }
    }

    // Morphological closing to connect coral:
    int DILATE_ROUNDS = 4;
    int ERODE_ROUNDS = 4;

    for (int i = 0; i < DILATE_ROUNDS; i++) {
        dilate(mask, height, width);
    }

    for (int j = 0; j < ERODE_ROUNDS; j++) {
        erode(mask, height, width);
    }

    // Implement a queue for BFS:
    int *queueX = malloc(height * width * sizeof(int));
    int *queueY = malloc(height * width * sizeof(int));

    int count = 0;

    // Connected components using BFS:
    // Start by scanning the entire image.
    for (int sy = 0; sy < height; sy++) {
        for (int sx = 0; sx < width; sx++) {
            // Ignore if not coral pixel or if already visited:
            if (mask[sy][sx] == 0 || visited[sy][sx]) {
                continue;
            }

            // Once you find a new coral blob, assume it's the beginning of a new coral piece and start building its connected components:
            Blob blob;
            blob.minX = blob.maxX = sx;
            blob.minY = blob.maxY = sy;
            blob.area = 0;

            int front = 0;
            int back = 0;

            // Store the starting coral pixel in queue:
            queueX[back] = sx;
            queueY[back] = sy;
            back++;
            visited[sy][sx] = 1;

            // Continue this process until there are no more connected coral pixels to explore (queue is empty):
            while (front < back) {
                int x = queueX[front];
                int y = queueY[front];
                front++;

                blob.area++;

                if (x < blob.minX) {
                    blob.minX = x;
                }

                if (x > blob.maxX) {
                    blob.maxX = x;
                }

                if (y < blob.minY) {
                    blob.minY = y;
                }

                if (y > blob.maxY) {
                    blob.maxY = y;
                }
                
                // Check neighboring pixels (8-neighbor method):
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        if (dx == 0 && dy == 0) {
                            continue;
                        }

                        int nx = x + dx;
                        int ny = y + dy;

                        if (nx < 0 || nx >= width || ny < 0 || ny >= height) {
                            continue;
                        }

                        if (visited[ny][nx]) {
                            continue;
                        }

                        if (mask[ny][nx] == 0) {
                            continue;
                        }

                        // Update queue if we discover another candidate (add it to queue):
                        visited[ny][nx] = 1;
                        queueX[back] = nx;
                        queueY[back] = ny;
                        back++;
                    }
                }
            }
            // Density filter + aspect filter first to remove mesh:
            // Compute dimensions of the blob:
            int blob_width = blob.maxX - blob.minX + 1;
            int blob_height = blob.maxY - blob.minY + 1;
            int boxArea = blob_width * blob_height;

            float density = (float)blob.area / boxArea;
            float aspect = (float)blob_width / blob_height;

            // Shape filter (density) + aspect filter:
            if (density < 0.5 && (aspect > 3.0 || aspect < 0.5)) {
                continue;
            }

            // Like we said before, we're going to filter the identified blobs and decide whether or not they are noise or truly coral pieces:
            if (blob.area >= minArea && blob.area <= maxArea) {
                count++;

                // If it passes the "filter", we're going to draw a red rectangle for the coral region + a red dot for the center:
                rectangle(out,
                          blob.minY, blob.minX,
                          blob.maxY, blob.maxX,
                          2, 0, 0,
                          255, 0 , 0, NO_CHANGE);
                
                int cx = (blob.minX + blob.maxX) / 2;
                int cy = (blob.minY + blob.maxY) / 2;

                filledEllipse(out,
                              cy, cx,
                              3, 3,
                              255, 0, 0, NO_CHANGE);

                printf("[%d] blob: area=%d box=(%d,%d)-(%d,%d)\n",
                       count,
                       blob.area,
                       blob.minX, blob.minY,
                       blob.maxX, blob.maxY);
            }
        }
    }

    printf("Detected coral blobs: %d\n", count);

    // De-allocate space:
    free(queueX);
    free(queueY);

    for (int y = 0; y < height; y++) {
        free(mask[y]);
        free(visited[y]);
    }
    free(mask);
    free(visited);

    return out;
}