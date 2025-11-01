///////////////////////////
// Sequence Definitions
// Hand-coded choreography for LED sequences
//
// SIMPLIFIED: No pointer management, just string names
// All times are in SECONDS (converted internally)
//

#include "sequences.h"
#include "modelsequence.h"

void initializeSequences(modelsequence* seq) {
  // Clear any existing steps
  seq->clearRegistry();

  // Sequence: Audio Reactive - 60 seconds
  // OPTIMIZATION: audio and vumeter only update during this sequence
  seq->startNewSequence("Audio Reactive", 60, true);
  seq->addStep("flowoctahedron", {"dark", "audio", "audio", "audio"}, 10, FADE, 2);
  seq->addStep("cycle", {"dark", "vumeter", "vumeter", "rainbow"}, 10, FADE, 2);
  seq->addStep("octachain", {"dark", "audio", "vumeter", "rainbow"}, 10, FADE, 2);
  seq->addStep("cycles", {"dark", "vumeter", "audio", "fire2012"}, 10, WIPE, 2);
  seq->addStep("twentyfourcell", {"dark", "audio", "fire2012", "audio"}, 10, FADE, 2);
  seq->addStep("hypercube", {"dark", "vumeter", "vumeter", "fire2012_blue"}, 10, FADE, 2);

  // Sequence: Fire Showcase - 60 seconds
  // OPTIMIZATION: Only fire effects update, audio effects don't
  seq->startNewSequence("Fire Showcase", 60, true);
  seq->addStep("flowoctahedron", {"dark", "fire2012", "fire2012", "fire2012"}, 10, FADE, 2);
  seq->addStep("cycle", {"dark", "fire2012_blue", "fire2012_blue", "rainbow"}, 10, FADE, 2);
  seq->addStep("cycles", {"dark", "fire2012_green", "rainbow", "fire2012_green"}, 10, FADE, 2);
  seq->addStep("octachain", {"dark", "fire2012", "fire2012_blue", "fire2012_green"}, 10, WIPE, 2);
  seq->addStep("twentyfourcell", {"dark", "fire2012", "fire2012", "fire2012"}, 10, FADE, 2);
  seq->addStep("hypercube", {"dark", "fire2012_blue", "fire2012_blue", "fire2012_blue"}, 10, FADE, 2);

  // Sequence: Plasma Party - 60 seconds
  // NEW: Using the new plasma and particle effects
  seq->startNewSequence("Plasma Party", 60, true);
  seq->addStep("flowoctahedron", {"dark", "plasma", "plasma", "plasma"}, 10, FADE, 2);
  seq->addStep("cycle", {"dark", "particles", "plasma", "rainbow"}, 10, FADE, 2);
  seq->addStep("cycles", {"dark", "plasma", "particles", "plasma"}, 10, WIPE, 2);
  seq->addStep("octachain", {"dark", "particles", "particles", "plasma"}, 10, FADE, 2);
  seq->addStep("twentyfourcell", {"dark", "plasma", "rainbow", "particles"}, 10, FADE, 2);
  seq->addStep("hypercube", {"dark", "particles", "plasma", "particles"}, 10, FADE, 2);

  // Sequence: Twenty-Four Cell Feature
  seq->startNewSequence("Twenty-Four Cell", 400, true);
  seq->addStep("twentyfourcell", {"pulsingblue", "rainbow", "rainbow", "rainbow"}, 400, FADE, 1);
  seq->addStep("altcycles", {"rainbow", "cylon", "staticred", "rainbow"}, 20, FADE, 4);
  seq->addStep("cycles", {"dark", "rainbow", "bluetored", "rainbow", "pulsingwhite"}, 20, FADE, 4);

  // Sequence: Default Flow - 45 seconds
  // OPTIMIZATION: NO stateful functions used, so NONE update! 100% CPU savings!
  seq->startNewSequence("Default Flow", 45, true);
  seq->addStep("flowoctahedron", {"dark", "rainbow", "bluetored", "staticblue"}, 10, FADE, 1);
  seq->addStep("flow_octa_merged", {"dark", "bluetored", "rainbow", "staticblue"}, 10, WIPE, 1.5);
  seq->addStep("octachain", {"dark", "staticblue", "bluetored", "rainbow"}, 5, INSTANT, 0);
  seq->addStep("cycle", {"dark", "rainbow", "staticblue", "bluetored"}, 10, FADE, 1);
  seq->addStep("cycle_merged", {"dark", "bluetored", "staticblue", "rainbow"}, 10, FADE, 2);

  // Sequence: Fast Pulse - 30 seconds
  // OPTIMIZATION: Only uses simple functions, no stateful updates
  seq->startNewSequence("Fast Pulse", 30, true);
  seq->addStep("flowoctahedron", {"dark", "rainbow", "rainbow", "rainbow"}, 3, FADE, 0.5);
  seq->addStep("flow_octa_merged", {"dark", "bluetored", "bluetored", "bluetored"}, 3, FADE, 0.5);
  seq->addStep("cube_hyper_merged", {"dark", "staticblue", "staticblue", "staticblue"}, 3, FADE, 0.5);
  seq->addStep("cycle", {"dark", "rainbow", "bluetored", "staticblue"}, 3, WIPE, 0.8);
  seq->addStep("cycle_merged", {"dark", "staticblue", "bluetored", "rainbow"}, 3, WIPE, 0.8);
  seq->addStep("cycles", {"dark", "bluetored", "rainbow", "staticblue"}, 3, FADE, 0.5);
  
  // Sequence: Slow Meditative - 90 seconds
  // OPTIMIZATION: Only simple functions, no stateful updates
  seq->startNewSequence("Slow Meditative", 90, true);
  seq->addStep("flowoctahedron", {"dark", "rainbow", "bluetored", "rainbow"}, 12, FADE, 3);
  seq->addStep("cycles", {"dark", "bluetored", "rainbow", "bluetored"}, 12, FADE, 3);
  seq->addStep("flow_rotated_reflected", {"dark", "rainbow", "staticblue", "rainbow"}, 12, FADE, 3);
  seq->addStep("cycle_merged", {"dark", "staticblue", "rainbow", "staticblue"}, 12, FADE, 3);
  
  // Sequence: Dynamic Mix - 60 seconds
  // OPTIMIZATION: Only simple functions
  seq->startNewSequence("Dynamic Mix", 60, true);
  seq->addStep("flow_octa_merged", {"dark", "rainbow", "bluetored", "staticblue"}, 5, INSTANT, 0);
  seq->addStep("cycle", {"dark", "staticblue", "rainbow", "bluetored"}, 4, WIPE, 1);
  seq->addStep("flowoctahedron", {"dark", "bluetored", "staticblue", "rainbow"}, 6, FADE, 1.5);
  seq->addStep("allcycles_inverted", {"dark", "rainbow", "rainbow", "bluetored"}, 7, FADE, 2);
  seq->addStep("cycle_merged", {"dark", "staticblue", "staticblue", "staticblue"}, 5, WIPE, 1.2);
  seq->addStep("cube_hyper_merged", {"dark", "bluetored", "bluetored", "rainbow"}, 8, INSTANT, 0);
  seq->addStep("flow_rotated_reflected", {"dark", "rainbow", "staticblue", "bluetored"}, 4, FADE, 0.8);
  
  // Sequence: All Base Models Showcase - 60 seconds
  seq->startNewSequence("Base Models", 60, true);
  seq->addStep("flowoctahedron", {"dark", "rainbow", "bluetored", "staticblue"}, 6, FADE, 1);
  seq->addStep("octachain", {"dark", "bluetored", "rainbow", "staticblue"}, 6, FADE, 1);
  seq->addStep("cycle", {"dark", "staticblue", "bluetored", "rainbow"}, 6, FADE, 1);
  seq->addStep("cycles", {"dark", "rainbow", "staticblue", "bluetored"}, 6, FADE, 1);
  seq->addStep("allcycles", {"dark", "bluetored", "rainbow", "staticblue"}, 6, FADE, 1);
  seq->addStep("cube", {"dark", "staticblue", "rainbow", "bluetored"}, 6, FADE, 1);
  seq->addStep("hypercube", {"dark", "rainbow", "bluetored", "staticblue"}, 6, FADE, 1);
  seq->addStep("hypercubes", {"dark", "bluetored", "staticblue", "rainbow"}, 6, FADE, 1);
  seq->addStep("twentyfourcell", {"dark", "staticblue", "bluetored", "rainbow"}, 6, FADE, 1);
  seq->addStep("graycodes", {"dark", "rainbow", "staticblue", "bluetored"}, 6, FADE, 1);
  
  // Sequence: Geometric Progressions - 48 seconds
  seq->startNewSequence("Geometric Progressions", 48, true);
  seq->addStep("cube", {"dark", "rainbow", "rainbow", "rainbow"}, 8, FADE, 2);
  seq->addStep("hypercube", {"dark", "bluetored", "bluetored", "bluetored"}, 8, FADE, 2);
  seq->addStep("twentyfourcell", {"dark", "staticblue", "staticblue", "staticblue"}, 8, FADE, 2);
  seq->addStep("octachain", {"dark", "rainbow", "bluetored", "staticblue"}, 8, FADE, 2);
  seq->addStep("graycodes", {"dark", "staticblue", "rainbow", "bluetored"}, 8, FADE, 2);
  seq->addStep("allcycles", {"dark", "bluetored", "staticblue", "rainbow"}, 8, FADE, 2);
  
  // Sequence: Fire and Ice - 50 seconds
  // OPTIMIZATION: Only fire effects update
  seq->startNewSequence("Fire and Ice", 50, true);
  seq->addStep("flowoctahedron", {"dark", "fire2012", "fire2012", "fire2012"}, 8, FADE, 2);
  seq->addStep("octachain", {"dark", "fire2012_blue", "fire2012_blue", "staticblue"}, 8, FADE, 2);
  seq->addStep("cycle", {"dark", "fire2012", "fire2012_blue", "fire2012_green"}, 8, WIPE, 1.5);
  seq->addStep("cycles", {"dark", "staticblue", "fire2012", "fire2012"}, 8, FADE, 2);
  seq->addStep("twentyfourcell", {"dark", "fire2012_green", "fire2012_green", "rainbow"}, 8, FADE, 2);
  seq->addStep("hypercube", {"dark", "fire2012_blue", "rainbow", "fire2012_blue"}, 10, FADE, 2);
  
  // Sequence: Audio + Fire Party - 45 seconds
  // OPTIMIZATION: Both audio and fire effects update
  seq->startNewSequence("Audio + Fire Party", 45, true);
  seq->addStep("flowoctahedron", {"dark", "audio", "fire2012", "fire2012"}, 8, FADE, 2);
  seq->addStep("cycle", {"dark", "fire2012", "audio", "fire2012_blue"}, 8, WIPE, 1.5);
  seq->addStep("octachain", {"dark", "vumeter", "fire2012_green", "audio"}, 8, FADE, 2);
  seq->addStep("cycles", {"dark", "fire2012_blue", "vumeter", "fire2012"}, 8, FADE, 2);
  seq->addStep("twentyfourcell", {"dark", "audio", "audio", "fire2012"}, 8, FADE, 2);
  seq->addStep("hypercube", {"dark", "fire2012", "fire2012", "audio"}, 5, FADE, 1);
  
  // Sequence: Particle Storm - 40 seconds
  // NEW: Showcasing the particle system
  seq->startNewSequence("Particle Storm", 40, true);
  seq->addStep("flowoctahedron", {"dark", "particles", "particles", "particles"}, 8, FADE, 2);
  seq->addStep("cycle", {"dark", "particles", "rainbow", "particles"}, 8, FADE, 2);
  seq->addStep("cycles", {"dark", "particles", "particles", "plasma"}, 8, WIPE, 1.5);
  seq->addStep("twentyfourcell", {"dark", "particles", "particles", "particles"}, 8, FADE, 2);
  seq->addStep("hypercube", {"dark", "plasma", "particles", "plasma"}, 8, FADE, 2);
}
