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

const int numcolorfunctions = 100;// for now!

#include <FastLED.h>
#include <Arduino.h>
#include "models.h"

const int MAXBRIGHTNESS = 160;


// color functions

// NOTE: For whatever twisted reason, CRGB is actually Green Red Blue.

inline CRGB rainbow(float position) {
  return //CRGB(0,0,0);
  CHSV(((int)(position * 255)), 255, MAXBRIGHTNESS);
}

inline CRGB bluetored(float position) {
  return CRGB(position * MAXBRIGHTNESS, 0, (1 - position) * MAXBRIGHTNESS);
}

inline CRGB cylon (float position){
  return CHSV((millis()%1000)/1000.0*255,255,MAXBRIGHTNESS*max(0.0,cos(3.141*position)));
}

inline CRGB staticblue(float position) {
  return CRGB(0, 0, MAXBRIGHTNESS);
}

inline CRGB staticred(float position) {
  return CRGB(0, MAXBRIGHTNESS,0);
}

inline CRGB pulsingblue(float position) {
  return CRGB(0, 0, (sin(millis()/1000.)+1)/2*MAXBRIGHTNESS);
}

inline CRGB pulsingwhite(float position) {
  float val = ((sin(position*6.28)+1)*(sin(millis()/1000.)+1)/5+.2)*MAXBRIGHTNESS;
  return CRGB(val, val, .6*val);
}

inline CRGB constantlyDark(float position) {
  return CRGB(0, 0, 0);
}

// Color function array

extern ColorFunction colorFunctionArray[numcolorfunctions];
extern String colorFunctionNames[numcolorfunctions];

#endif  // COLORFUNCTIONS_H