#include "low_contrast.h"

static int clamp(double v) {
    if (v < 0.0)   return 0;
    if (v > 255.0) return 255;
    return (int)(v + 0.5);
}

/*
    This function compresses pixel values toward the midpoint (128).
    Essentially moves all colors closer to gray, reducing contrast.
*/
Image applyLowContrast(Image input, double factor) {
    if (factor < 0.0) factor = 0.0;
    if (factor > 1.0) factor = 1.0;

    // Use 128 as the neutral midpoint for compression.
    const double mid = 128.0;

    Image output = createImage(input.height, input.width);

    for (int i = 0; i < input.height; i++) {
        for (int j = 0; j < input.width; j++) {
            int r = clamp(mid + (input.map[i][j].r - mid) * factor);
            int g = clamp(mid + (input.map[i][j].g - mid) * factor);
            int b = clamp(mid + (input.map[i][j].b - mid) * factor);
            output.map[i][j].r = (unsigned char)r;
            output.map[i][j].g = (unsigned char)g;
            output.map[i][j].b = (unsigned char)b;
            output.map[i][j].i = (unsigned char)((r + g + b) / 3);
        }
    }

    return output;
}
