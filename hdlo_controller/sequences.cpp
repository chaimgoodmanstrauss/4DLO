///////////////////////////
// Sequence Definitions
// Hand-coded choreography for LED sequences

// All times are in SECONDS and converted to milliseconds internally

// NOTE: With lazy updates, only the stateful functions actually used 
// in each sequence will be updated. This saves CPU time!

#include "sequences.h"
#include "modelsequence.h"

void initializeSequences(modelsequence* seq) {
  // Clear any existing steps
  seq->clearRegistry();

  // Sequence: Audio Reactive - 60 seconds
  // OPTIMIZATION: audio and vumeter only update during this sequence
  seq->startNewSequence("Audio Reactive", 60, true);
  seq->addStepByName("flowoctahedron", {"dark", "audio", "audio", "audio"}, 10, FADE, 2);
  seq->addStepByName("cycle", {"dark", "vumeter", "vumeter", "rainbow"}, 10, FADE, 2);
  seq->addStepByName("octachain", {"dark", "audio", "vumeter", "rainbow"}, 10, FADE, 2);
  seq->addStepByName("cycles", {"dark", "vumeter", "audio", "fire2012"}, 10, WIPE, 2);
  seq->addStepByName("twentyfourcell", {"dark", "audio", "fire2012", "audio"}, 10, FADE, 2);
  seq->addStepByName("hypercube", {"dark", "vumeter", "vumeter", "fire2012_blue"}, 10, FADE, 2);

  // Sequence: Fire Showcase - 60 seconds
  // OPTIMIZATION: Only fire effects update, audio effects don't
  seq->startNewSequence("Fire Showcase", 60, true);
  seq->addStepByName("flowoctahedron", {"dark", "fire2012", "fire2012", "fire2012"}, 10, FADE, 2);
  seq->addStepByName("cycle", {"dark", "fire2012_blue", "fire2012_blue", "rainbow"}, 10, FADE, 2);
  seq->addStepByName("cycles", {"dark", "fire2012_green", "rainbow", "fire2012_green"}, 10, FADE, 2);
  seq->addStepByName("octachain", {"dark", "fire2012", "fire2012_blue", "fire2012_green"}, 10, WIPE, 2);
  seq->addStepByName("twentyfourcell", {"dark", "fire2012", "fire2012", "fire2012"}, 10, FADE, 2);
  seq->addStepByName("hypercube", {"dark", "fire2012_blue", "fire2012_blue", "fire2012_blue"}, 10, FADE, 2);

  seq->startNewSequence("twenny-fer cell", 400, true);
  seq->addStepByName("twentyfourcell", { "pulsingblue","rainbow","rainbow","rainbow"}, 400, FADE, 1);
  seq->addStepByName("altcycles", {"rainbow","cylon","staticred", "rainbow",}, 20, FADE, 4);
  seq->addStepByName("cycles", {"dark","rainbow","bluetored","rainbow","pulsingwhite"}, 20, FADE, 4);

  // Sequence 0: Default Flow - 45 seconds
  // OPTIMIZATION: NO stateful functions used, so NONE update! 100% CPU savings!
  seq->startNewSequence("Default Flow", 45, true);
  seq->addStepByName("flowoctahedron", {"dark", "rainbow", "bluetored", "staticblue"}, 10, FADE, 1);
  seq->addStepByName("flow_octa_merged", {"dark", "bluetored", "rainbow", "staticblue"}, 10, WIPE, 1.5);
  seq->addStepByName("octachain", {"dark", "staticblue", "bluetored", "rainbow"}, 5, INSTANT, 0);
  seq->addStepByName("cycle", {"dark", "rainbow", "staticblue", "bluetored"}, 10, FADE, 1);
  seq->addStepByName("cycle_merged", {"dark", "bluetored", "staticblue", "rainbow"}, 10, FADE, 2);

  // Sequence 1: Fast Pulse - 30 seconds
  // OPTIMIZATION: Only uses simple functions, no stateful updates
  seq->startNewSequence("Fast Pulse", 30, true);
  seq->addStepByName("flowoctahedron", {"dark", "rainbow", "rainbow", "rainbow"}, 3, FADE, 0.5);
  seq->addStepByName("flow_octa_merged", {"dark", "bluetored", "bluetored", "bluetored"}, 3, FADE, 0.5);
  seq->addStepByName("cube_hyper_merged", {"dark", "staticblue", "staticblue", "staticblue"}, 3, FADE, 0.5);
  seq->addStepByName("cycle", {"dark", "rainbow", "bluetored", "staticblue"}, 3, WIPE, 0.8);
  seq->addStepByName("cycle_merged", {"dark", "staticblue", "bluetored", "rainbow"}, 3, WIPE, 0.8);
  seq->addStepByName("cycles", {"dark", "bluetored", "rainbow", "staticblue"}, 3, FADE, 0.5);
  
  // Sequence 2: Slow Meditative - 90 seconds
  // OPTIMIZATION: Only simple functions, no stateful updates
  seq->startNewSequence("Slow Meditative", 90, true);
  seq->addStepByName("flowoctahedron", {"dark", "rainbow", "bluetored", "rainbow"}, 12, FADE, 3);
  seq->addStepByName("cycles", {"dark", "bluetored", "rainbow", "bluetored"}, 12, FADE, 3);
  seq->addStepByName("flow_rotated_reflected", {"dark", "rainbow", "staticblue", "rainbow"}, 12, FADE, 3);
  seq->addStepByName("cycle_merged", {"dark", "staticblue", "rainbow", "staticblue"}, 12, FADE, 3);
  
  // Sequence 3: Dynamic Mix - 60 seconds
  // OPTIMIZATION: Only simple functions
  seq->startNewSequence("Dynamic Mix", 60, true);
  seq->addStepByName("flow_octa_merged", {"dark", "rainbow", "bluetored", "staticblue"}, 5, INSTANT, 0);
  seq->addStepByName("cycle", {"dark", "staticblue", "rainbow", "bluetored"}, 4, WIPE, 1);
  seq->addStepByName("flowoctahedron", {"dark", "bluetored", "staticblue", "rainbow"}, 6, FADE, 1.5);
  seq->addStepByName("allcycles_inverted", {"dark", "rainbow", "rainbow", "bluetored"}, 7, FADE, 2);
  seq->addStepByName("cycle_merged", {"dark", "staticblue", "staticblue", "staticblue"}, 5, WIPE, 1.2);
  seq->addStepByName("cube_hyper_merged", {"dark", "bluetored", "bluetored", "rainbow"}, 8, INSTANT, 0);
  seq->addStepByName("flow_rotated_reflected", {"dark", "rainbow", "staticblue", "bluetored"}, 4, FADE, 0.8);
  
  // Sequence 4: All Base Models Showcase - 60 seconds
  seq->startNewSequence("Base Models", 60, true);
  seq->addStepByName("flowoctahedron", {"dark", "rainbow", "bluetored", "staticblue"}, 6, FADE, 1);
  seq->addStepByName("octachain", {"dark", "bluetored", "rainbow", "staticblue"}, 6, FADE, 1);
  seq->addStepByName("cycle", {"dark", "staticblue", "bluetored", "rainbow"}, 6, FADE, 1);
  seq->addStepByName("cycles", {"dark", "rainbow", "staticblue", "bluetored"}, 6, FADE, 1);
  seq->addStepByName("allcycles", {"dark", "bluetored", "rainbow", "staticblue"}, 6, FADE, 1);
  seq->addStepByName("cube", {"dark", "staticblue", "rainbow", "bluetored"}, 6, FADE, 1);
  seq->addStepByName("hypercube", {"dark", "rainbow", "bluetored", "staticblue"}, 6, FADE, 1);
  seq->addStepByName("hypercubes", {"dark", "bluetored", "staticblue", "rainbow"}, 6, FADE, 1);
  seq->addStepByName("twentyfourcell", {"dark", "staticblue", "bluetored", "rainbow"}, 6, FADE, 1);
  seq->addStepByName("graycodes", {"dark", "rainbow", "staticblue", "bluetored"}, 6, FADE, 1);
  
  // Sequence 5: Geometric Progressions - 48 seconds
  seq->startNewSequence("Geometric Progressions", 48, true);
  seq->addStepByName("cube", {"dark", "rainbow", "rainbow", "rainbow"}, 8, FADE, 2);
  seq->addStepByName("hypercube", {"dark", "bluetored", "bluetored", "bluetored"}, 8, FADE, 2);
  seq->addStepByName("twentyfourcell", {"dark", "staticblue", "staticblue", "staticblue"}, 8, FADE, 2);
  seq->addStepByName("octachain", {"dark", "rainbow", "bluetored", "staticblue"}, 8, FADE, 2);
  seq->addStepByName("graycodes", {"dark", "staticblue", "rainbow", "bluetored"}, 8, FADE, 2);
  seq->addStepByName("allcycles", {"dark", "bluetored", "staticblue", "rainbow"}, 8, FADE, 2);
  
  // Sequence 6: Fire and Ice - 50 seconds
  // OPTIMIZATION: Only fire effects update
  seq->startNewSequence("Fire and Ice", 50, true);
  seq->addStepByName("flowoctahedron", {"dark", "fire2012", "fire2012", "fire2012"}, 8, FADE, 2);
  seq->addStepByName("octachain", {"dark", "fire2012_blue", "fire2012_blue", "staticblue"}, 8, FADE, 2);
  seq->addStepByName("cycle", {"dark", "fire2012", "fire2012_blue", "fire2012_green"}, 8, WIPE, 1.5);
  seq->addStepByName("cycles", {"dark", "staticblue", "fire2012", "fire2012"}, 8, FADE, 2);
  seq->addStepByName("twentyfourcell", {"dark", "fire2012_green", "fire2012_green", "rainbow"}, 8, FADE, 2);
  seq->addStepByName("hypercube", {"dark", "fire2012_blue", "rainbow", "fire2012_blue"}, 10, FADE, 2);
  
  // Sequence 7: Audio + Fire Party - 45 seconds
  // OPTIMIZATION: Both audio and fire effects update
  seq->startNewSequence("Audio + Fire Party", 45, true);
  seq->addStepByName("flowoctahedron", {"dark", "audio", "fire2012", "fire2012"}, 8, FADE, 2);
  seq->addStepByName("cycle", {"dark", "fire2012", "audio", "fire2012_blue"}, 8, WIPE, 1.5);
  seq->addStepByName("octachain", {"dark", "vumeter", "fire2012_green", "audio"}, 8, FADE, 2);
  seq->addStepByName("cycles", {"dark", "fire2012_blue", "vumeter", "fire2012"}, 8, FADE, 2);
  seq->addStepByName("twentyfourcell", {"dark", "audio", "audio", "fire2012"}, 8, FADE, 2);
  seq->addStepByName("hypercube", {"dark", "fire2012", "fire2012", "audio"}, 5, FADE, 1);
}
