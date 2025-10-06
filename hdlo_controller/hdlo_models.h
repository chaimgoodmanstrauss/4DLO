///////////////////////////
// HDLO Models

// define the data:

#ifndef HDLO_MODELS_H
#define HDLO_MODELS_H
#include <array>
#include "models.h"

extern std::array<std::array<int, 6>, 120> flowoctahedrondata;
extern std::array<std::array<int, 6>, 120> flowoctahedroncompound1data;
extern std::array<std::array<int, 6>, 120> octachainshiftedbydata;
extern std::array<std::array<int, 6>, 120> cycledata;
extern std::array<std::array<int, 6>, 120> cyclestemplatedata;
extern std::array<std::array<int, 6>, 120> cyclesdata;
// array of pointers
const int nummodels = 6;
extern colormodel* ourcolormodels[6];
extern String modelNames[6]; // Model names matching ourcolormodels order

void initializemodels();

// Sequence initialization - will be auto-generated
// Requires modelsequence to be registered first
class modelsequence; // Forward declaration
void initializeSequences(modelsequence* seq);

#endif // HDLO_MODELS_H