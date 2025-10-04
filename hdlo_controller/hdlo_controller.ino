#include "teensy4controller.h"
#include "ledconstants.h"
#include "edgesetup.h"
#include "colorfunctions.h"
#include "models.h"
#include "hdlo_models.h"

// #include "trillcontroller.h" // If we would like to add this.


int count = 0;

// Animation control variables
int currentModelIndex = 0;
int currentPermutation = 0;
const int numPermutations = numcolorfunctions - 1; // Don't count the dark function
unsigned long lastChangeTime = 0;
const unsigned long changeInterval = 5000; // Change every 5 seconds


void setup() {
  
  // Prepare serial output, if we wish to use it for debugging or monitoring data.
  Serial.begin(9600);
  Serial.println("Yo the HDLO LED contoller ");
  Serial.println("We are using "+String(numberofleds)+" on "+String(numberofpins)+" pins.");

  octocontroller.begin(); //initialize the octocontroller 
  // create our teensycontroller 
  teensycontroller = new CTeensy4Controller<RGB, WS2811_800kHz>(&octocontroller);

  // Initialize FastLED
  FastLED.addLeds(teensycontroller, rgbarray, numberofpins * ledsperstrip);
  // set various parameters
  FastLED.setBrightness(120);
  
  bool animateinitializationq = true;

  /////////////
  initedgedata();
  initializemodels();
  
  // Override colorfunction 0 to be constantly dark
  colorFunctionArray[0] = constantlyDark;
  
  // Initialize all models with color functions
  for(int modelIdx = 0; modelIdx < nummodels; modelIdx++) {
    for(int funcIdx = 0; funcIdx < numcolorfunctions; funcIdx++) {
      ourcolormodels[modelIdx]->setColorFunction(funcIdx, colorFunctionNames[funcIdx], colorFunctionArray[funcIdx]);
    }
  }
  
  Serial.println("Setup complete. Starting animation loop.");
}


void loop() {
  unsigned long currentTime = millis();
  
  // Check if it's time to change the permutation or model
  if(currentTime - lastChangeTime >= changeInterval) {
    currentPermutation++;
    if(currentPermutation >= numPermutations) {
      currentPermutation = 0;
      currentModelIndex++;
      if(currentModelIndex >= nummodels) {
        currentModelIndex = 0;
      }
      Serial.println("Switching to model index: " + String(currentModelIndex));
    }
    
    // Permute the color functions (rotate non-zero functions)
    // Keep function 0 as dark, rotate functions 1, 2, 3
    for(int modelIdx = 0; modelIdx < nummodels; modelIdx++) {
      for(int funcIdx = 1; funcIdx < numcolorfunctions; funcIdx++) {
        int newFuncIdx = ((funcIdx - 1 + currentPermutation) % numPermutations) + 1;
        ourcolormodels[modelIdx]->setColorFunction(funcIdx, 
                                                    colorFunctionNames[newFuncIdx], 
                                                    colorFunctionArray[newFuncIdx]);
      }
    }
    
    lastChangeTime = currentTime;
    Serial.println("Permutation: " + String(currentPermutation));
  }
  
  // Light up the LEDs using the strand table
  for(int i = 0; i < numberofleds; i++) {
    int edgeType = strandtable[i][1];
    int positionInt = strandtable[i][0];
    
    // Skip if this LED is not mapped to an edge
    if(edgeType == 0 && positionInt == 0) {
      rgbarray[i] = CRGB(0, 0, 0);
      continue;
    }
    
    // Convert position from integer (0-10000) to float (0.0-1.0)
    float position = positionInt / 10000.0;
    
    // Get the color from the current model
    rgbarray[i] = ourcolormodels[currentModelIndex]->getcolorfunction(edgeType, position);
  }
  
  // Update the LED display
  FastLED.show();
  
  // Small delay to avoid overwhelming the controller
  delay(10);
}