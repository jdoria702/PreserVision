#include "texture_segmentation.h"
#include "netpbm.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void segmentTexture(Image image, char *labels, int patchSize) {
    FILE *labelFile = fopen(labels, "r");
    if (!labelFile) {
        fprintf(stderr, "Error opening label file: %s\n", labels);
        return;
    }

    int numPatchesX = image.width / patchSize;
    
    // Skip header line
    char line[256];
    fgets(line, sizeof(line), labelFile);  // skip "patch_id,label"
    
    while (fgets(line, sizeof(line), labelFile)) {
        int patchId, clusterLabel;
        if (sscanf(line, "%d,%d", &patchId, &clusterLabel) == 2) {
            
            // Convert patch_id to (x, y) grid coordinates
            int patchY = patchId / numPatchesX;
            int patchX = patchId % numPatchesX;
            
            // Get top-left corner of patch in pixel space
            int startPixelY = patchY * patchSize;
            int startPixelX = patchX * patchSize;
            
            // Pick a color based on clusterLabel
            int r = (clusterLabel * 50) % 256;
            int g = (clusterLabel * 100) % 256;
            int b = (clusterLabel * 150) % 256;
            
            // Color all pixels in this patch
            for (int py = 0; py < patchSize; py++) {
                for (int px = 0; px < patchSize; px++) {
                    int pixelY = startPixelY + py;
                    int pixelX = startPixelX + px;
                    setPixel(image, pixelY, pixelX, r, g, b, 0);  // 0 for intensity (setPixel will set r,g,b)
                }
            }
        }
    }
    
    fclose(labelFile);
}