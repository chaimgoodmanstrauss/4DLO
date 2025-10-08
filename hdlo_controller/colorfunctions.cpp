////////////////////////////////////////////
//
//
//   colorfunctions.cpp
//
// a colorfunction has one responsibility: return a CRGB at a given position
// It is free to make additional use of time or other data, but that's its business.
// 
// In the header, the functions were declared and defined;
// more complex functions can be split off into their own files, or placed here.  
// Here we define colorFunctionArray and colorFunctionNames 
//

#include "colorfunctions.h"

// Color function array definitions
ColorFunction colorFunctionArray[numcolorfunctions] = {
  constantlyDark,  // 0 - constantly dark
  rainbow,         // 1
  bluetored,       // 2
  staticblue,       // 3
  pulsingblue,
  pulsingwhite,
  staticred,
  cylon
};

String colorFunctionNames[numcolorfunctions] = {
  "dark",
  "rainbow",
  "bluetored", 
  "staticblue",
  "pulsingblue",
  "pulsingwhite",
  "staticred",
  "cylon"
};