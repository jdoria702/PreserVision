#ifndef LAWS_TEXTURE_H
#define LAWS_TEXTURE_H

#include "netpbm.h"
#include "convolution.h"

void createLawsTextureEnergyMap(Matrix input, char *outputPath, int patchSize);
char *make_output_path(const char *base, const char *suffix);

#endif