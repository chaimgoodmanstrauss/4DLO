///////////////////////////
// HDLO Models

// define the data:

#ifndef HDLO_MODELS_H
#define HDLO_MODELS_H
#include <array>
#include "models.h"

extern std::array<std::array<int, 6>, 120> testdata;
extern std::array<std::array<int, 6>, 120> edgedata;
// array of pointers
const int nummodels = 2;
extern colormodel* ourcolormodels[2];

extern String modelNames[2]; // Model names matching ourcolormodels order
void initializemodels();
class modelsequence; // Forward declaration;
void initializeSequences(modelsequence* seq);
#endif // HDLO_MODELS_H
