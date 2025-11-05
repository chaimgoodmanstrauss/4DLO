///////////////////////////
// Example Sequences with Inline Palette Specification and Sequence-Level Audio
//
// NEW FEATURES:
// 1. Use {"function", "palette"} to specify palettes inline
// 2. Audio configuration is now per-sequence, not per-step
// 3. Automatic fallback to SD card when microphone is silent
//

#include "sequences.h"
#include "modelsequence.h"

void initializeSequences(modelsequence* seq) {
  
seq->clearRegistry();
seq->startNewSequence("Live Audio Only", 4000, true,
                       AudioSourceConfig(AUDIO_MICROPHONE));

 /*
  seq->addStep("test", {
    "breathing",
    "fftfire",
    "audiocylon",
    "plasma","plasma","plasma",
    "cylon"
  }, 100, FADE, 2);*/
   seq->addStep("test", {
    "breathing",
    
    "rainbow",
    "rainbow",
    {"noiseperlin", "ocean"},
    {"noiseperlin", "lava"},
    {"noiseperlin", "plasma"},
    {"noiseperlin", "forest"},


  }, 100, FADE, 2);

  
/*
    seq->addStep("test", {
    "breathing",
    {"plasma","rainbow"},
    {"plasma","rainbow"},
    {"fftfire","fire"},
    {"fftfire","ocean"},
   {"fftfire","rainbow"},
    {"audio2","rainbow"},
    {"vumeter","rainbow"},
  }, 100, FADE, 2);*/
/*
  seq->addStep("test", {
    "breathing",
    {"plasma", "ocean"},
    {"cylon", "fire"},
    {"plasma", "rainbow"},
    {"plasma", "forest"},
    {"plasma", "lava"},
    {"plasma", "sunset"}
  }, 100, FADE, 2);
  */
  

/*
   seq->addStep("test", {
    "breathing",
    {"audiocylon", "fire"},
    {"audiocylon", "ocean"},
    {"audiocylon", "rainbow"},
    {"audiocylon", "forest"},
    {"audiocylon", "lava"},
    {"audiocylon", "sunset"}
  }, 100, FADE, 2);
*/

}
  