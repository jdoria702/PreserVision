#ifndef TEXTURE_SEGMENTATION_H
#define TEXTURE_SEGMENTATION_H

#include "netpbm.h"

void segmentTexture(Image image, char *labels, int patchSize);

#endif