////////////////////////////////////////////
//
//
//   colorfunctions.h
//
// a colorfunction has one responsibility: return a CRGB at a given position
// It is free to make additional use of time or other data, but that's its business.
// 
// In this file, declare the functions; in the cpp file, they are organized
// into an array, colorFunctionArray and colorFunctionNames 
//
#ifndef COLORFUNCTIONS_H
#define COLORFUNCTIONS_H

#include <FastLED.h>
#include <Arduino.h>
#include "models.h"

// color functions
inline CRGB rainbow(float position) {
  return CHSV(position * 255, 255, 255);
}

inline CRGB bluetored(float position) {
  return CRGB(position * 255, 0, (1 - position) * 255);
}

inline CRGB staticblue(float position) {
  return CRGB(0, 0, 255);
}

inline CRGB constantlyDark(float position) {
  return CRGB(0, 0, 0);
}

// Color function array
const int numcolorfunctions = 4;
extern ColorFunction colorFunctionArray[numcolorfunctions];
extern String colorFunctionNames[numcolorfunctions];

#endif // COLORFUNCTIONS_H