// a colorfunction has one responsibility: return a CRGB at a given position
#include "colorfunctions.h"

// Color function array definitions
ColorFunction colorFunctionArray[numcolorfunctions] = {
  constantlyDark,  // 0 - constantly dark
  rainbow,         // 1
  bluetored,       // 2
  staticblue       // 3
};

String colorFunctionNames[numcolorfunctions] = {
  "dark",
  "rainbow",
  "bluetored", 
  "staticblue"
};