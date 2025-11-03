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



const int numberofpins = 1;// this will be derived from the array
  byte pinList[numberofpins] = {24}; // this is derived from the array
  const int ledsperstrip = 260; // the max number of leds per strip
const int maxedgesperstrand = 20;


/// Here is where we work out how the pins correspond to specific edges in the 24-cell: 
const int stranddata[numberofpins][maxedgesperstrand][3] = 
{ //a cycle is 67,95,39,64,92,36, all positive direction
  
    {//strand #1
    {40,0,1}, //{the length of the segment mapped to edge 2, pointed forwards
    {4,7,1},
    {40,1,1}, 
    {4,7,1},
    {40,2,1},
    {4,7,1},
    {40,3,1},
    {4,7,1},
    {40,4,1},
    {4,7,1},
    {40,5,1},
    },

   // {{200,1,1}},
    
/*
  { //strand #2// the equator of flow octahedron//21,51,87,62
    {72,21, 1},
    {72,51,1},
    {72,87,1},
    {72,62,1}},

{ //strand #3// parts of the hypercube//59,37,38,56
    {72,59, 1}, 
    {72,37,1},
    {72,75,-1},
    {72,85,-1}},*/
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
  
  OctoWS2811 octocontroller(ledsperstrip, displayMemory, drawingMemory, WS2811_GRB | WS2811_800kHz, numberofpins, pinList);


// Now set up a pointer that will point to a  CTeensy4Controller
// This pcontroller allows the Fast LED library to make use of the teensy.
// We create the actual pcontroller that this points to in the setup loop. 
// We use templates set the color space (RGB) and the speed (800 khz, the standard for WS2811's),
CTeensy4Controller<GRB, WS2811_800kHz> *teensycontroller;
