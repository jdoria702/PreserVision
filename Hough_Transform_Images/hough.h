// hough.h
#ifndef HOUGH_H
#define HOUGH_H

#include "netpbm.h"

// Detect circles in a binary edge image using the Hough Circle Transform.
// edgeMatrix    : output from Canny edge detection (255 = edge, 0 = background)
// originalImage : color image on which detected circles will be drawn
// rMin, rMax    : radius search range in pixels
// voteThreshold : minimum percentage (0-100) of the circle's circumference
//                 that must be covered by edge pixels to report it as a circle
//                 (e.g. 30 means 30% of the arc must be present)
// Returns a copy of originalImage with every detected circle drawn in red.
Image houghCircleDetection(Matrix edgeMatrix, Image originalImage,
                           int rMin, int rMax, int voteThreshold);

#endif
