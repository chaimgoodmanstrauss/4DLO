
const int numberofpins = 2;// this will be derived from the array
  byte pinList[numberofpins] = {6,8}; // this is derived from the array
  const int ledsperstrip = 300; // the max number of leds per strip

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
  OctoWS2811 octocontroller(ledsperstrip, displayMemory, drawingMemory, WS2811_RGB | WS2811_800kHz, numberofpins, pinList);


// Now set up a pointer that will point to a  CTeensy4Controller
// This pcontroller allows the Fast LED library to make use of the teensy.
// We create the actual pcontroller that this points to in the setup loop. 
// We use templates set the color space (RGB) and the speed (800 khz, the standard for WS2811's),
CTeensy4Controller<RGB, WS2811_800kHz> *teensycontroller;
