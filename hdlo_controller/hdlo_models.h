///////////////////////////
// HDLO Models

// define the data:

#ifndef HDLO_MODELS_H
#define HDLO_MODELS_H
#include <array>
#include "models.h"

extern std::array<std::array<int, 6>, 120> hypercubedata;
extern std::array<std::array<int, 6>, 120> rotatedhypercubedata;
extern std::array<std::array<int, 6>, 120> hypercubesdata;
extern std::array<std::array<int, 6>, 120> twentyfourcelldata;
extern std::array<std::array<int, 6>, 120> testdata;
extern std::array<std::array<int, 6>, 120> cyclesdata;
extern std::array<std::array<int, 6>, 120> allcyclesdata;
extern std::array<std::array<int, 6>, 120> graycodesdata;
extern std::array<std::array<int, 6>, 120> sixpathsdata;
extern std::array<std::array<int, 6>, 120> octahedrondata;
extern std::array<std::array<int, 6>, 120> cubedata;
// array of pointers
const int nummodels = 11;
extern colormodel* ourcolormodels[11];

extern String modelNames[11]; // Model names matching ourcolormodels order
void initializemodels();
class modelsequence; // Forward declaration;
void initializeSequences(modelsequence* seq);
#endif // HDLO_MODELS_H
