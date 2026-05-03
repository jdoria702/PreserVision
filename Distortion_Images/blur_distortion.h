#ifndef BLUR_DISTORTION_H
#define BLUR_DISTORTION_H

#include "netpbm.h"
#include <stdio.h>

// Applies a uniform box blur to each RGB channel.
// kernel_size: side length of the averaging window (must be a positive odd integer)
Image applyBlurDistortion(Image input, int kernel_size);

#endif
