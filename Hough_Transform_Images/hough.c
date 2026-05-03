#include "hough.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

// NOTE: The image that we're working with is coral_nursery.ppm.
// If you carefully examine the image, you'll notice the following:
// a) Corals are not all circular.
// b) Corals are of varying sizes.
// c) The mesh contains uniform "squares" with rounded edges, so it's possible to detect them as circles...
// Since our Hough implementation relies on edge detection -> circle detection, this is an imperfect method!

/*
    The Hough Transform is a feature extraction method used to find basic shapes in a picture, like circles, lines, and ellipses.
    It transfers these shapes' representation from the spatial domain to the parameter space.
    It remains effective even when there are distortions in the image, like noise or occlusion.

    The Hough Transform uses an accumulator array (parameter space / Hough space) to represent the available parameter values
    for the shapes that are being detected. For example, the slope (m) and y-intercept (b) of a line could be the parameters for line detection.
    It also calculates the matching curves in the parameter space for each edge point in the image, and this is accomplished
    by finding the curve that intersects the parameter values at the spot by iterating over all possible values of the parameters.
    The votes or intersections for every combination of parameters are recorded by the accumulator array.
    At the end, the program finds peaks in the array that match the parameters of the shape it has identified. 
    These peaks show whether the image contains lines, circles, or other shapes.

    SOURCE: https://www.geeksforgeeks.org/computer-vision/hough-transform-in-computer-vision/
 */

// Hough Algorithm:
// edge image → collect edge pixels → vote for circle centers → keep strong circles → remove duplicates → draw results
// "For each edge pixel, what circles could it belong to?"

// Define circle result:
// A circle is defined by (center_x, center_y, radius).
// Each detect circle stores: center x, center y, radius, vote count.
typedef struct {
    int cx, cy, r, votes;
} Circle;

// Comparison function for sorting circles by votes descending:
static int circleVoteDesc(const void *a, const void *b) {
    return ((Circle *)b)->votes - ((Circle *)a)->votes;
}

Image houghCircleDetection(Matrix edgeMatrix, Image originalImage,
                           int rMin, int rMax, int voteThreshold) {
    int height = edgeMatrix.height;
    int width  = edgeMatrix.width;

    int maxCircles = 100000;
    Circle *raw    = (Circle *)malloc(maxCircles * sizeof(Circle));
    int numRaw     = 0;

    // Pre-collect edge pixel coordinates so the inner loop only visits actual
    // edges instead of scanning every pixel and branching on most of them.
    int  maxEdges = height * width;
    int *edgeX    = (int *)malloc(maxEdges * sizeof(int));
    int *edgeY    = (int *)malloc(maxEdges * sizeof(int));
    int  numEdges = 0;
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            // Collect edge pixels:
            if (edgeMatrix.map[y][x] >= 128) {
                edgeX[numEdges] = x;
                edgeY[numEdges] = y;
                numEdges++;
            }
    printf("Edge pixels  : %d (%.1f%% of image)\n", numEdges,
           100.0 * numEdges / (height * width));
    
    // The idea is that we take one edge pixel at (x, y), and assuming that edge pixel lies on a circle of radius r,
    // then the center of that circle must lie on a circle of radius r around (x, y)!
    // So we want to process angles around the point from 0 degrees to 360 degrees and vote for all possible centers.
    // At the same time, we also want to keep in mind that the circle we're tracking is not always of a set radius r, 
    // hence why we try difference radii too.

    // Precompute a shared 360-entry cos/sin table (reused every radius).
    // Per-radius integer offset tables (da, db) remove all FP from the inner loop:
    double cosT[360], sinT[360];
    for (int t = 0; t < 360; t++) {
        cosT[t] = cos(t * 2.0 * M_PI / 360.0);
        sinT[t] = sin(t * 2.0 * M_PI / 360.0);
    }
    int *da = (int *)malloc(360 * sizeof(int));
    int *db = (int *)malloc(360 * sizeof(int));

    // Process one radius at a time to keep peak memory at O(H*W):
    for (int r = rMin; r <= rMax; r++) {
        int **acc = (int **)malloc(height * sizeof(int *));
        for (int y = 0; y < height; y++)
            acc[y] = (int *)calloc(width, sizeof(int));

        // Arc-length sampling up to 360 steps (1 degree min resolution).
        int numAngles = MIN((int)(2.0 * M_PI * r) + 1, 360);

        // Build integer offset table for this radius — no FP in inner loop:
        for (int t = 0; t < numAngles; t++) {
            int idx = (int)(t * 360.0 / numAngles);   // map to 360-entry table
            da[t] = (int)round(r * cosT[idx]);
            db[t] = (int)round(r * sinT[idx]);
        }

        for (int e = 0; e < numEdges; e++) {
            int x = edgeX[e], y = edgeY[e];
            for (int t = 0; t < numAngles; t++) {
                int a = x - da[t];
                int b = y - db[t];
                if ((unsigned)a < (unsigned)width && (unsigned)b < (unsigned)height)
                    acc[b][a]++;
            }
        }

        // Threshold expressed as a fraction of the full circumference so it
        // means the same arc coverage regardless of radius.
        int effectiveThreshold = (int)(voteThreshold / 100.0 * numAngles);
        if (effectiveThreshold < 1) effectiveThreshold = 1;

        // Per-radius NMS: cap window at 40 px so large-radius runs stay fast.
        // The cross-radius greedy pass below handles any remaining spatial overlap.
        int suppR = MIN(r / 2, 40);
        if (suppR < 4) suppR = 4;
        for (int b = suppR; b < height - suppR; b++) {
            for (int a = suppR; a < width - suppR; a++) {
                if (acc[b][a] < effectiveThreshold) continue;
                int isMax = 1;
                for (int dy = -suppR; dy <= suppR && isMax; dy++) {
                    for (int dx = -suppR; dx <= suppR && isMax; dx++) {
                        if (dy == 0 && dx == 0) continue;
                        int v = acc[b + dy][a + dx];
                        if (v > acc[b][a]) { isMax = 0; break; }
                        // On a tie, the neighbour that comes earlier in raster
                        // order wins, so suppress the later one.
                        if (v == acc[b][a]) {
                            int ny = b + dy, nx = a + dx;
                            if (ny < b || (ny == b && nx < a)) { isMax = 0; break; }
                        }
                    }
                }
                if (isMax && numRaw < maxCircles) {
                    raw[numRaw].cx    = a;
                    raw[numRaw].cy    = b;
                    raw[numRaw].r     = r;
                    raw[numRaw].votes = acc[b][a];
                    numRaw++;
                }
            }
        }

        for (int y = 0; y < height; y++) free(acc[y]);
        free(acc);
    }

    free(edgeX);
    free(edgeY);
    free(da);
    free(db);

    // Cross-radius NMS: greedily keep circles in descending vote order,
    // discarding any that overlap a circle already kept.  
    // Two circles are considered overlapping when their centres are closer than the larger
    // radius of the pair.
    qsort(raw, numRaw, sizeof(Circle), circleVoteDesc);

    Circle *circles = (Circle *)malloc(numRaw * sizeof(Circle));
    int numCircles  = 0;
    for (int i = 0; i < numRaw; i++) {
        int keep = 1;
        for (int j = 0; j < numCircles && keep; j++) {
            int dx   = raw[i].cx - circles[j].cx;
            int dy   = raw[i].cy - circles[j].cy;
            int dist = (int)sqrt((double)(dx * dx + dy * dy));
            int sep  = MAX(circles[j].r, raw[i].r);
            if (dist < sep) keep = 0;
        }
        if (keep) circles[numCircles++] = raw[i];
    }

    // Report.
    printf("=== Hough Circle Detection Results ===\n");
    printf("Image size   : %d x %d pixels\n", width, height);
    printf("Radius range : %d - %d pixels\n", rMin, rMax);
    printf("Threshold    : %d%% of circumference\n", voteThreshold);
    printf("Detected     : %d circle(s)\n\n", numCircles);
    for (int i = 0; i < numCircles; i++) {
        printf("  [%3d]  center = (%4d, %4d)  radius = %3d  votes = %d\n",
               i + 1,
               circles[i].cx, circles[i].cy,
               circles[i].r,
               circles[i].votes);
    }

    // Draw circles on a copy of the original image:
    Image out = createImage(originalImage.height, originalImage.width);
    for (int y = 0; y < originalImage.height; y++)
        for (int x = 0; x < originalImage.width; x++)
            out.map[y][x] = originalImage.map[y][x];

    for (int i = 0; i < numCircles; i++) {
        ellipse(out,
                circles[i].cy, circles[i].cx,
                circles[i].r,  circles[i].r,
                2, 0, 0,
                255, 0, 0, NO_CHANGE);
        filledEllipse(out,
                      circles[i].cy, circles[i].cx,
                      3, 3,
                      255, 0, 0, NO_CHANGE);
    }

    free(raw);
    free(circles);
    return out;
}
