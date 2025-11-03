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
    "dark",
    "rainbow",
    "rainbow",
    "rainbow",
    "rainbow",
    "rainbow",
    "rainbow",
    "rainbow",
    "rainbow",
  }, 100, FADE, 2);
*/


  seq->addStep("test", {
    "dark",
    {"audio", "fire"},
    {"audio", "ocean"},
    {"audio", "rainbow"},
    {"audio", "fire"},
    {"audio", "fire"},
    {"vumeter", "lava"},
    {"beatdetect", "sunset"}
  }, 10, FADE, 2);
  

  seq->addStep("test", {
    "dark",
    {"audio", "rainbow"},
    {"freqbands", "rainbow"},
    {"plasma", "rainbow"},
    {"audio", "fire"},
    {"audio", "fire"},
    {"vumeter", "lava"},
  }, 10, FADE, 2);

  
  seq->addStep("test", {
    "dark",
    {"plasma", "rainbow"},
    {"plasma", "ocean"},
    {"plasma", "forest"},
    {"plasma", "sunset"},
    {"plasma", "lava"},
    {"plasma", "ice"},
    {"plasma", "rainbow"},
  }, 10, FADE, 2);

}
  