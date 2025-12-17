////////////////////////////////////
//
//   globalids.h
//
// Global integer ID registry for all named entities
// Replaces string-based lookups throughout the system
//
// ID Ranges:
//   1000-1999: Color Functions
//   2000-2999: Palettes  
//   3000-3999: Models
//   4000-4999: Named Permutations
//
#ifndef GLOBALIDS_H
#define GLOBALIDS_H

#include <Arduino.h>

////////////////////////////////////
// COLOR FUNCTION IDs (1000-1999)
//
constexpr int dark = 1000;
constexpr int breathing = 1001;
constexpr int fire2012 = 1002;
constexpr int audio = 1003;
constexpr int audio2 = 1004;
constexpr int vumeter = 1005;
constexpr int particles = 1006;
constexpr int cylon = 1007;
constexpr int audiocylon = 1008;
constexpr int beatdetect = 1009;
constexpr int vocals = 1010;
constexpr int fftfire = 1011;
constexpr int fftspectrum = 1012;
constexpr int perlin = 1013;
constexpr int simplecolor = 1014;
constexpr int dualblobs = 1015;
constexpr int fftballs = 1016;

////////////////////////////////////
// PALETTE IDs (2000-2999)
//
constexpr int fire = 2000;
constexpr int bluefire = 2001;
constexpr int greenfire = 2002;
constexpr int ocean = 2003;
constexpr int forest = 2004;
constexpr int sunset = 2005;
constexpr int rainbow = 2006;
constexpr int lava = 2007;
constexpr int ice = 2008;
constexpr int audiogreen = 2009;
constexpr int audiospectrum = 2010;
constexpr int white = 2011;
constexpr int red = 2012;
constexpr int green = 2013;
constexpr int blue = 2014;
constexpr int cyan = 2015;
constexpr int yellow = 2016;
constexpr int magenta = 2017;
constexpr int heat = 2018;
constexpr int party = 2019;
constexpr int cloud = 2020;
constexpr int forest_builtin = 2021;
constexpr int rainbow_builtin = 2022;
constexpr int rainbowstripe = 2023;
constexpr int plasma_pal = 2024;  // Palette named plasma (color func removed)

////////////////////////////////////
// MODEL IDs (3000-3999)
//
constexpr int strandsbytype = 3000;
constexpr int strandsegmentlights = 3001;
constexpr int quarters = 3002;
constexpr int eighths = 3003;
constexpr int strandsbyindex = 3004;
constexpr int hypercube = 3005;
constexpr int rotatedhypercube = 3006;
constexpr int hypercubes = 3007;
constexpr int twentyfourcell = 3008;
constexpr int tester = 3009;
constexpr int cycles = 3010;
constexpr int allcycles = 3011;
constexpr int graycodes = 3012;
constexpr int sixpaths = 3013;
constexpr int octahedron = 3014;
constexpr int cube = 3015;
constexpr int verts = 3016;

////////////////////////////////////
// NAMED PERMUTATION IDs (4000-4999)
//
// Octahedron cell permutations
constexpr int oneTozzzp = 4000;
constexpr int oneTozzzm = 4001;
constexpr int oneTozzpz = 4002;
constexpr int oneTozzmz = 4003;
constexpr int oneTozpzz = 4004;
constexpr int oneTozmzz = 4005;
constexpr int oneTommmm = 4006;
constexpr int oneTopppp = 4007;
constexpr int oneTommmp = 4008;
constexpr int oneTopppm = 4009;
constexpr int oneTommpm = 4010;
constexpr int oneToppmp = 4011;
constexpr int oneTommpp = 4012;
constexpr int oneToppmm = 4013;
constexpr int oneTompmm = 4014;
constexpr int oneTopmpp = 4015;
constexpr int oneTompmp = 4016;
constexpr int oneTopmpm = 4017;
constexpr int oneTomppm = 4018;
constexpr int oneTopmmp = 4019;
constexpr int oneTomppp = 4020;
constexpr int oneTopmmm = 4021;
constexpr int oneTomzzz = 4022;
constexpr int oneTopzzz = 4023;

// Cube cell permutations  
constexpr int i1Tozzpp = 4024;
constexpr int i1Tozzmm = 4025;
constexpr int i1Tozzpm = 4026;
constexpr int i1Tozzmp = 4027;
constexpr int i1Tompzz = 4028;
constexpr int i1Topmzz = 4029;
constexpr int i1Tozmmz = 4030;
constexpr int i1Tozppz = 4031;
constexpr int i1Tozmzp = 4032;
constexpr int i1Tozpzm = 4033;
constexpr int i1Tozmzm = 4034;
constexpr int i1Tozpzp = 4035;
constexpr int i1Tozmpz = 4036;
constexpr int i1Tozpmz = 4037;
constexpr int i1Tomzmz = 4038;
constexpr int i1Topzpz = 4039;
constexpr int i1Tomzzp = 4040;
constexpr int i1Topzzm = 4041;
constexpr int i1Tomzzm = 4042;
constexpr int i1Topzzp = 4043;
constexpr int i1Tomzpz = 4044;
constexpr int i1Topzmz = 4045;
constexpr int i1Tommzz = 4046;
constexpr int i1Toppzz = 4047;

// Other permutations
constexpr int rightW = 4048;
constexpr int rot4 = 4049;
constexpr int rot3 = 4050;
constexpr int onetoi = 4051;
constexpr int simpletest = 4052;
constexpr int simpletest2 = 4053;
constexpr int rot = 4049;  // Alias for rot4

////////////////////////////////////
// ID TYPE HELPERS
//
inline bool isColorFunctionId(int id) { return id >= 1000 && id < 2000; }
inline bool isPaletteId(int id) { return id >= 2000 && id < 3000; }
inline bool isModelId(int id) { return id >= 3000 && id < 4000; }
inline bool isPermutationId(int id) { return id >= 4000 && id < 5000; }

////////////////////////////////////
// PERMUTATION ARRAYS (integer versions)
//
inline const int octacellperms[24] = {
    oneTozzzp, oneTozzzm, oneTozzpz, oneTozzmz, oneTozpzz, oneTozmzz,
    oneTommmm, oneTopppp, oneTommmp, oneTopppm, oneTommpm, oneToppmp,
    oneTommpp, oneToppmm, oneTompmm, oneTopmpp, oneTompmp, oneTopmpm,
    oneTomppm, oneTopmmp, oneTomppp, oneTopmmm, oneTomzzz, oneTopzzz
};

inline const int cubecellperms[24] = {
    i1Tozzpp, i1Tozzmm, i1Tozzpm, i1Tozzmp, i1Tompzz, i1Topmzz,
    i1Tozmmz, i1Tozppz, i1Tozmzp, i1Tozpzm, i1Tozmzm, i1Tozpzp,
    i1Tozmpz, i1Tozpmz, i1Tomzmz, i1Topzpz, i1Tomzzp, i1Topzzm,
    i1Tomzzm, i1Topzzp, i1Tomzpz, i1Topzmz, i1Tommzz, i1Toppzz
};

#endif // GLOBALIDS_H
