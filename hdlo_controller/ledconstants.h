//////////////////////////////
//
//  ledconstants.h
//
// Here we specify the details of how the strands are arranged. 
//

//  led #(1) on strand #strandnumber,  corresponds to 
// strandtable[strandnumber*ledsperstrip], 
// and led #(n) corresponds to strandtable[strandnumber*ledsperstrip+(n-1)]
          
// The maximum number of segments per strand. 
// This may increase as we incorporate vertices.



const int numberofpins = 28;// this will be derived from the array
  byte pinList[numberofpins] = {3,9,6,2
  ,1, 10, 4, 24,
   28, 27, 12, 11,
    29, 25, 30, 34,
     35, 31, 32, 33, 37, 38, 16, 17, 39, 40, 15, 13
  }; // this is derived from the array
  const int ledsperstrip = 580; // the max number of leds per strip
const int maxedgesperstrand = 11;


/// Here is where we work out how the pins correspond to specific edges in the 24-cell: 
const int stranddata[numberofpins][maxedgesperstrand][10] = 
{// Strand 0 in quarter 1
{{0, 103, 1}, {133, 39, 1}, {0, 116, 1}, {192, 71, -1}, {0, 100, 1}, {103, 29, -1}, {0, 103, 1}, {83, 62, 1}},
// Strand 1 in quarter 1
{{0, 111, 1}, {133, 57, -1}, {0, 108, 1}, {192, 80, 1}, {0, 101, 1}, {103, 90, 1}, {0, 111, 1}, {83, 23, 1}},
// Strand 2 in quarter 1
{{0, 103, 1}, {103, 14, -1}, {0, 96, 1}, {192, 46, 1}, {0, 116, 1}, {282, 85, 1}},
// Strand 3 in quarter 1
{{0, 111, 1}, {103, 7, -1}, {0, 96, 1}, {192, 18, -1}, {0, 108, 1}, {282, 48, 1}},
// Strand 4 in quarter 1
{{0, 103, 1}, {72, 95, -1}, {0, 119, 1}, {72, 41, -1}, {0, 111, 1}, {83, 7-4, 1}, {0, 103, 1}, {103, 79, 1}, {0, 98, 1}, {103, 55, -1}, {0, 111, 1}},
// Strand 5 in quarter 1
{{0, 116, 1}, {192, 9, -1}, {0, 98, 1}, {192, 0, 1}, {0, 108, 1}},
// Strand 6 in quarter 1
{{72, 64, -1}, {0, 116, 1}, {282, 24, 1}, {0, 108, 1}, {72, 33, -1}},
// Strand 0 in quarter 2
{{0, 105, 1}, {133, 75, 1}, {0, 114, 1}, {192, 94, -1}, {0, 100, 1}, {103, 43, -1}, {0, 105, 1}, {83, 21, 1}},
// Strand 1 in quarter 2
{{0, 113, 1}, {133, 26, -1}, {0, 106, 1}, {192, 65, 1}, {0, 101, 1}, {103, 35, 1}, {0, 113, 1}, {83, 49, 1}},
// Strand 2 in quarter 2
{{0, 105, 1}, {103, 54, -1}, {0, 98, 1}, {192, 77, 1}, {0, 114, 1}, {282, 60, 1}},
// Strand 3 in quarter 2
{{0, 113, 1}, {103, 19, -1}, {0, 98, 1}, {192, 44, -1}, {0, 106, 1}, {282, 84, 1}},
// Strand 4 in quarter 2
{{0, 105, 1}, {72, 91, -1}, {0, 119, 1}, {72, 82, -1}, {0, 113, 1}, {83, 59, 1}, {0, 105, 1}, {103, 5, 1}, {0, 97, 1}, {103, 1, -1}, {0, 113, 1}},
// Strand 5 in quarter 2
{{0, 114, 1}, {192, 12, -1}, {0, 97, 1}, {192, 8, 1}, {0, 106, 1}},
// Strand 6 in quarter 2
{{72, 28, -1}, {0, 114, 1}, {282, 38, 1}, {0, 106, 1}, {72, 69, -1}},
// Strand 0 in quarter 3
{{0, 109, 1}, {133, 58, 1}, {0, 110, 1}, {192, 89, -1}, {0, 100, 1}, {103, 83, -1}, {0, 109, 1}, {83, 51, 1}},
// Strand 1 in quarter 3
{{0, 117, 1}, {133, 36, -1}, {0, 102, 1}, {192, 30, 1}, {0, 101, 1}, {103, 68, 1}, {0, 117, 1}, {83, 86, 1}},
// Strand 2 in quarter 3
{{0, 109, 1}, {103, 17, -1}, {0, 97, 1}, {192, 52, 1}, {0, 110, 1}, {282, 20, 1}},
// Strand 3 in quarter 3
{{0, 117, 1}, {103, 45, -1}, {0, 97, 1}, {192, 76, -1}, {0, 102, 1}, {282, 61, 1}},
// Strand 4 in quarter 3
{{0, 109, 1}, {72, 34, -1}, {0, 119, 1}, {72, 67, -1}, {0, 117, 1}, {83, 27, 1}, {0, 109, 1}, {103, 3, 1}, {0, 99, 1}, {103, 10, -1}, {0, 117, 1}},
// Strand 5 in quarter 3
{{0, 110, 1}, {192, 4, -1}, {0, 99, 1}, {192, 13, 1}, {0, 102, 1}},
// Strand 6 in quarter 3
{{72, 42, -1}, {0, 110, 1}, {282, 73, 1}, {0, 102, 1}, {72, 92, -1}},
// Strand 0 in quarter 4
{{0, 107, 1}, {133, 25, 1}, {0, 112, 1}, {192, 32, -1}, {0, 100, 1}, {103, 66, -1}, {0, 107, 1}, {83, 87, 1}},
// Strand 1 in quarter 4
{{0, 115, 1}, {133, 72, -1}, {0, 104, 1}, {192, 40, 1}, {0, 101, 1}, {103, 93, 1}, {0, 115, 1}, {83, 63, 1}},
// Strand 2 in quarter 4
{{0, 107, 1}, {103, 47, -1}, {0, 99, 1}, {192, 16, 1}, {0, 112, 1}, {282, 50, 1}},
// Strand 3 in quarter 4
{{0, 115, 1}, {103, 78, -1}, {0, 99, 1}, {192, 53, -1}, {0, 104, 1}, {282, 22, 1}},
// Strand 4 in quarter 4
{{0, 107, 1}, {72, 70, -1}, {0, 119, 1}, {72, 31, -1}, {0, 115, 1}, {83, 37, 1}, {0, 107, 1}, {103, 11, 1}, {0, 96, 1}, {103, 15, -1}, {0, 115, 1}},
// Strand 5 in quarter 4
{{0, 112, 1}, {192, 2, -1}, {0, 96, 1}, {192, 6, 1}, {0, 104, 1}},
// Strand 6 in quarter 4
{{72, 81, -1}, {0, 112, 1}, {282, 56, 1}, {0, 104, 1}, {72, 88, -1}}

};

// in edgesetup, strandtable is created from this information
// strandtable is used to convert between LED coords and a position on an edge


  const int numberofleds = numberofpins*ledsperstrip;

// this holds all of the light information at each frame; this holds three bytes per index
  CRGB rgbarray[numberofpins * ledsperstrip];

// Next is the octo  controller, and dedicated memory for it.
// DMAMEM displayMemory is a fast dedicated chunk of ram. 
// drawingMemory is set aside for the next frame.

// These are arrays of ints, so the compiler will align it to 32 bit memory; 
// An int is four bytes and a color is three -- so we adjust the memory size accordingly.

  DMAMEM int displayMemory[ledsperstrip * numberofpins * 3 / 4];
  int drawingMemory[ledsperstrip * numberofpins * 3 / 4];
  
  OctoWS2811 octocontroller(ledsperstrip, displayMemory, drawingMemory, WS2811_GRB| WS2811_800kHz, numberofpins, pinList);


// Now set up a pointer that will point to a  CTeensy4Controller
// This pcontroller allows the Fast LED library to make use of the teensy.
// We create the actual pcontroller that this points to in the setup loop. 
// We use templates set the color space (RGB) and the speed (800 khz, the standard for WS2811's),
CTeensy4Controller<GRB, WS2811_800kHz> *teensycontroller;
