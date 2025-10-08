#include "teensy4controller.h"
#include "ledconstants.h"
#include "edgesetup.h"
#include "colorfunctions.h"
#include "models.h"
#include "hdlo_models.h"
#include "modelsequence.h"

int count = 0;

// Create a modelsequence instance
modelsequence mainSequence;

void setup() {
  
  // Prepare serial output, if we wish to use it for debugging or monitoring data.
  Serial.begin(9600);
  Serial.println("Yo the HDLO LED contoller ");
  Serial.println("We are using "+String(numberofleds)+" on "+String(numberofpins)+" pins.");

  octocontroller.begin(); //initialize the octocontroller 
  // create our teensycontroller 
  teensycontroller = new CTeensy4Controller<GRB, WS2811_800kHz>(&octocontroller);

  // Initialize FastLED
  FastLED.addLeds(teensycontroller, rgbarray, numberofpins * ledsperstrip);
  // set various parameters
  FastLED.setBrightness(120);
  
  bool animateinitializationq = true;

  /////////////
  initedgedata();
  
  // Initialize base models (these will auto-register)
  initializemodels();
  
  // Override colorfunction 0 to be constantly dark
  colorFunctionArray[0] = constantlyDark;
  
  // Initialize all models with color functions
  // Note: Now we use the global registry instead of ourcolormodels
  for(int modelIdx = 0; modelIdx < colormodel::getNumRegisteredModels(); modelIdx++) {
    colormodel* model = colormodel::getModelRegistry()[modelIdx];
    for(int funcIdx = 0; funcIdx < numcolorfunctions; funcIdx++) {
      model->setColorFunction(funcIdx, 
                              colorFunctionNames[funcIdx], 
                              colorFunctionArray[funcIdx]);
    }
  }
  
  // Create merged models using string names - SO EASY!
  colormodel::mergeModels("flowoctahedron", "octachain", "flow_octa_merged");
  colormodel::mergeModels("cycle", "cycles", "cycle_merged");
  colormodel::mergeModels("cube", "hypercube", "cube_hyper_merged");
  
  // Create permuted models using string names
  String perms1[] = {"rotate30", "reflect"};
  colormodel::applyEdgePermutationSequence("flowoctahedron", perms1, 2, "flow_rotated_reflected");
  
  String perms2[] = {"invert"};
  colormodel::applyEdgePermutationSequence("allcycles", perms2, 1, "allcycles_inverted");
  
  // Set color functions for all newly created models
  for(int modelIdx = 0; modelIdx < colormodel::getNumRegisteredModels(); modelIdx++) {
    colormodel* model = colormodel::getModelRegistry()[modelIdx];
    for(int funcIdx = 0; funcIdx < numcolorfunctions; funcIdx++) {
      model->setColorFunction(funcIdx, 
                              colorFunctionNames[funcIdx], 
                              colorFunctionArray[funcIdx]);
    }
  }
  
  // Print the registry to see all models
  colormodel::printRegistry();
  
  // Register the global registry with modelsequence
  mainSequence.registerModels(colormodel::getModelRegistry(), 
                              colormodel::getNumRegisteredModels(), 
                              colormodel::getModelNameRegistry());
  mainSequence.registerColorFunctions(colorFunctionArray, numcolorfunctions, colorFunctionNames);
  
  // Initialize sequences - all definitions come from sequences.cpp
  initializeSequences(&mainSequence);
  
  Serial.println("Registry has " + String(mainSequence.getRegistrySize()) + " sequences");
  
  // Start the registry-based cycling
  mainSequence.beginRegistry();
  
  Serial.println("Setup complete. Registry cycling started.");
}


void loop() {
  unsigned long currentTime = millis();
  
  // Update registry - automatically switches sequences based on individual durations
  mainSequence.updateRegistry(currentTime);
  
  // Update the sequence state (handles transitions within the current sequence)
  mainSequence.update(currentTime, colorFunctionArray);


  // Get direct access to the current model
  colormodel* currentModel = mainSequence.getCurrentModel();
  if(currentModel == nullptr) {
    // Handle error case
    Serial.println("Error: No current model available");
    delay(10);
    return;
  }
  
  // Get the edge model data array
  const std::array<std::array<int, 6>, 120>& edgeModelData = currentModel->getEdgeModels();
  

  
  // Light up the LEDs using the sequence
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
    
    int direction = edgeModelData[edgeType][1];      // 1 or -1
    float shiftposition = edgeModelData[edgeType][2]/10000.;    
    float scaleposition = edgeModelData[edgeType][3]/10000.;   
    float shifttime = edgeModelData[edgeType][4]/10000.;      
    float scaletime = edgeModelData[edgeType][5]/10000.;     
    
    position = scaleposition*position+shiftposition+.2*scaletime*(currentTime/1000.)+shifttime;
    
    if(direction < 0) {
      position = 1.0 - position;  // Reverse the position if direction is negative
    }


    // Get the color from the sequence
    rgbarray[i] = mainSequence.getColor(edgeType, position);
  }
  
  // Update the LED display
  FastLED.show();
  
  // Optional: Print status every 2 seconds
  static unsigned long lastPrint = 0;
  if(currentTime - lastPrint > 2000) {
    Serial.println("Sequence: " + mainSequence.getCurrentRegistryName() + 
                   " | Step " + String(mainSequence.getCurrentStep() + 1) + 
                   "/" + String(mainSequence.getTotalSteps()) + 
                   " - " + mainSequence.getCurrentModelName() +
                   (mainSequence.isInTransition() ? " (transitioning)" : ""));
    lastPrint = currentTime;
  }
  
  delay(10);
}