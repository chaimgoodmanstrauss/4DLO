///////////////////////////
// Sequence Definitions
// Hand-coded choreography for LED sequences

#include "sequences.h"
#include "modelsequence.h"
#include "hdlo_models.h"
#include "colorfunctions.h"

void initializeSequences(modelsequence* seq) {
  // Clear any existing steps
  seq->clearRegistry();
  
  // Sequence 0: Default Flow - 45 seconds
  seq->startNewSequence("Default Flow", 45000, true);
  seq->addStepByName("flowoctahedron", {"dark", "rainbow", "bluetored", "staticblue"}, 8000, FADE, 1000);
  seq->addStepByName("flowoctahedroncompound1", {"dark", "bluetored", "rainbow", "staticblue"}, 6000, WIPE, 1500);
  seq->addStepByName("octachainshiftedby", {"dark", "staticblue", "bluetored", "rainbow"}, 7000, INSTANT, 0);
  seq->addStepByName("cycle", {"dark", "rainbow", "staticblue", "bluetored"}, 5000, FADE, 1000);
  seq->addStepByName("cycles", {"dark", "bluetored", "staticblue", "rainbow"}, 10000, FADE, 2000);
  
  // Sequence 1: Fast Pulse - 30 seconds
  seq->startNewSequence("Fast Pulse", 30000, true);
  seq->addStepByName("flowoctahedron", {"dark", "rainbow", "rainbow", "rainbow"}, 3000, FADE, 500);
  seq->addStepByName("flowoctahedroncompound1", {"dark", "bluetored", "bluetored", "bluetored"}, 3000, FADE, 500);
  seq->addStepByName("octachainshiftedby", {"dark", "staticblue", "staticblue", "staticblue"}, 3000, FADE, 500);
  seq->addStepByName("cycle", {"dark", "rainbow", "bluetored", "staticblue"}, 3000, WIPE, 800);
  seq->addStepByName("cyclestemplate", {"dark", "staticblue", "bluetored", "rainbow"}, 3000, WIPE, 800);
  seq->addStepByName("cycles", {"dark", "bluetored", "rainbow", "staticblue"}, 3000, FADE, 500);
  
  // Sequence 2: Slow Meditative - 90 seconds
  seq->startNewSequence("Slow Meditative", 90000, true);
  seq->addStepByName(modelNames[0], {"dark", "rainbow", "bluetored", "rainbow"}, 12000, FADE, 3000);
  seq->addStepByName(modelNames[5], {"dark", "bluetored", "rainbow", "bluetored"}, 12000, FADE, 3000);
  seq->addStepByName(modelNames[2], {"dark", "rainbow", "staticblue", "rainbow"}, 12000, FADE, 3000);
  seq->addStepByName(modelNames[4], {"dark", "staticblue", "rainbow", "staticblue"}, 12000, FADE, 3000);
  
  // Sequence 3: Dynamic Mix - 60 seconds
  seq->startNewSequence("Dynamic Mix", 60000, true);
  seq->addStepByName(modelNames[1], {colorFunctionNames[0],colorFunctionNames[1],colorFunctionNames[2],colorFunctionNames[3]}, 5000, INSTANT, 0);
  seq->addStepByName(modelNames[3], {"dark", "staticblue", "rainbow", "bluetored"}, 4000, WIPE, 1000);
  seq->addStepByName(modelNames[0], {"dark", "bluetored", "staticblue", "rainbow"}, 6000, FADE, 1500);
  seq->addStepByName(modelNames[5], {"dark", "rainbow", "rainbow", "bluetored"}, 7000, FADE, 2000);
  seq->addStepByName(modelNames[2], {"dark", "staticblue", "staticblue", "staticblue"}, 5000, WIPE, 1200);
  seq->addStepByName(modelNames[4], {"dark", "bluetored", "bluetored", "rainbow"}, 8000, INSTANT, 0);
  seq->addStepByName(modelNames[1], {"dark", "rainbow", "staticblue", "bluetored"}, 4000, FADE, 800);
}