#ifndef LOW_CONTRAST_H
#define LOW_CONTRAST_H

#include "netpbm.h"

// Compresses the dynamic range of each RGB channel toward the midpoint (128).
// factor: contrast scale in [0.0, 1.0]
//   0.0 = flat gray (all pixels become 128), 1.0 = original image unchanged
Image applyLowContrast(Image input, double factor);

#endif
