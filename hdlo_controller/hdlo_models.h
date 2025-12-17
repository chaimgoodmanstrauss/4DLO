///////////////////////////
// HDLO Models

// define the data:

#ifndef HDLO_MODELS_H
#define HDLO_MODELS_H
#include <array>
#include "models.h"
#include "globalids.h"

extern std::array<std::array<int, 6>, 120> strandsbytypedata;
extern std::array<std::array<int, 6>, 120> strandsegmentlightsdata;
extern std::array<std::array<int, 6>, 120> quartersdata;
extern std::array<std::array<int, 6>, 120> eighthsdata;
extern std::array<std::array<int, 6>, 120> strandsbyindexdata;
extern std::array<std::array<int, 6>, 120> hypercubedata;
extern std::array<std::array<int, 6>, 120> rotatedhypercubedata;
extern std::array<std::array<int, 6>, 120> hypercubesdata;
extern std::array<std::array<int, 6>, 120> twentyfourcelldata;
extern std::array<std::array<int, 6>, 120> testerdata;
extern std::array<std::array<int, 6>, 120> cyclesdata;
extern std::array<std::array<int, 6>, 120> allcyclesdata;
extern std::array<std::array<int, 6>, 120> graycodesdata;
extern std::array<std::array<int, 6>, 120> sixpathsdata;
extern std::array<std::array<int, 6>, 120> octahedrondata;
extern std::array<std::array<int, 6>, 120> cubedata;
extern std::array<std::array<int, 6>, 120> vertsdata;
// array of pointers
const int nummodels = 17;
extern colormodel* ourcolormodels[17];

void initializemodels();
class modelsequence; // Forward declaration;
void initializeSequences(modelsequence* seq);
#endif // HDLO_MODELS_H
