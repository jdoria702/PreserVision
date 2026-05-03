#ifndef IRREGULAR_SHAPE_DETECTOR_H
#define IRREGULAR_SHAPE_DETECTOR_H

#include "netpbm.h"

Image detectCoralBlobs(Image input, int minArea, int maxArea);

#endif