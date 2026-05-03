#ifndef TEXTURE_SEGMENTATION_H
#define TEXTURE_SEGMENTATION_H

#include "netpbm.h"

void segmentTexture(Image image, char *energyMap, char *labels, int numClusters);

#endif