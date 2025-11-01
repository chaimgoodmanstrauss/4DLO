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

  //=================================================================
  // EXAMPLE 1: Microphone with automatic fallback to SD card
  //=================================================================
  
  // Audio will switch to "ambient.wav" (looping) at 0.8x speed after 3 seconds of silence
  seq->startNewSequence("Reactive with Fallback", 60, true, 
                       AudioSourceConfig("ambient.wav", 0.8, 3000));
  
  seq->addStep("flowoctahedron", {
    "dark",
    {"audio", "fire"},              // audio-reactive with fire palette
    {"vumeter", "fire"},            
    {"beatdetect", "fire"}
  }, 10, FADE, 2);
  
  seq->addStep("cycle", {
    "dark",
    {"audio", "ocean"},
    {"freqbands", "ocean"},
    {"plasma", "ocean"}
  }, 10, FADE, 2);
  
  seq->addStep("octachain", {
    "dark",
    {"audio", "rainbow"},
    {"vumeter", "rainbow"},
    {"particles", "rainbow"}
  }, 10, FADE, 2);
  
  //=================================================================
  // EXAMPLE 2: SD Card playback for entire sequence  
  //=================================================================
  
  // SD card files loop by default
  seq->startNewSequence("Music Sync", 45, true,
                       AudioSourceConfig(AUDIO_SD_CARD, "track1.wav", 1.0));
  
  seq->addStep("flowoctahedron", {
    "dark",
    {"audio", "sunset"},
    {"beatdetect", "sunset"},
    {"freqbands", "sunset"}
  }, 15, FADE, 2);
  
  seq->addStep("hypercube", {
    "dark",
    {"audio", "party"},
    {"vumeter", "party"},
    {"plasma", "party"}
  }, 15, FADE, 2);
  
  seq->addStep("twentyfourcell", {
    "dark",
    {"audio", "ice"},
    {"particles", "ice"},
    {"fire2012", "ice"}
  }, 15, FADE, 2);
  
  //=================================================================
  // EXAMPLE 3: Pure microphone (no fallback)
  //=================================================================
  
  seq->startNewSequence("Live Audio Only", 40, true,
                       AudioSourceConfig(AUDIO_MICROPHONE));
  
  seq->addStep("flowoctahedron", {
    "dark",
    {"audio", "fire"},
    {"vumeter", "lava"},
    {"beatdetect", "sunset"}
  }, 10, FADE, 2);
  
  seq->addStep("cycle", {
    "dark",
    {"audio", "rainbow"},
    {"freqbands", "rainbow"},
    {"plasma", "rainbow"}
  }, 10, FADE, 2);
  
  //=================================================================
  // EXAMPLE 4: Non-audio sequence (visual only)
  //=================================================================
  
  seq->startNewSequence("Pure Visuals", 60, true);  // No audio config = AUDIO_KEEP_CURRENT
  
  seq->addStep("flowoctahedron", {
    "dark",
    {"fire2012", "fire"},
    {"fire2012", "ocean"},
    {"fire2012", "forest"}
  }, 10, FADE, 2);
  
  seq->addStep("cycle", {
    "dark",
    {"plasma", "sunset"},
    {"particles", "party"},
    {"rainbow"}
  }, 10, FADE, 2);
  
  //=================================================================
  // EXAMPLE 5: Complex fallback configuration
  //=================================================================
  
  // Create custom audio config with specific parameters
  AudioSourceConfig complexAudio;
  complexAudio.type = AUDIO_MICROPHONE;
  complexAudio.enableFallback = true;
  complexAudio.fallbackFile = "nature_sounds.wav";
  complexAudio.fallbackRate = 0.5;        // Half speed for ambient effect
  complexAudio.silenceThreshold = 0.02;   // Adjust sensitivity
  complexAudio.silenceTimeout = 8000;     // 8 seconds before switching
  
  seq->startNewSequence("Ambient Fallback", 90, true, complexAudio);
  
  seq->addStep("flowoctahedron", {
    "dark",
    {"audio", "forest"},
    {"plasma", "forest"},
    {"particles", "forest"}
  }, 30, FADE, 3);
  
  seq->addStep("cycles", {
    "dark",
    {"audio", "ocean"},
    {"fire2012", "ocean"},
    {"vumeter", "ocean"}
  }, 30, FADE, 3);
  
  seq->addStep("hypercube", {
    "dark",
    {"audio", "sunset"},
    {"plasma", "sunset"},
    {"beatdetect", "sunset"}
  }, 30, FADE, 3);
  
  //=================================================================
  // EXAMPLE 6: Backward compatibility - old syntax still works
  //=================================================================
  
  seq->startNewSequence("Classic Style", 30, true);
  
  // Original string array syntax (no inline palettes)
  seq->addStep("flowoctahedron", {"dark", "fire2012", "plasma", "rainbow"}, 10, FADE, 2);
  seq->addStep("cycle", {"dark", "audio", "particles", "staticblue"}, 10, FADE, 2);
  seq->addStep("octachain", {"dark", "vumeter", "fire2012", "plasma"}, 10, FADE, 2);
}

//=================================================================
// NOTES:
//=================================================================
// 
// Audio Configuration Options:
// 
// 1. Simple microphone with fallback:
//    AudioSourceConfig("filename.wav", playbackRate, silenceTimeoutMs)
//    - Fallback file always loops when activated
//
// 2. SD card playback:
//    AudioSourceConfig(AUDIO_SD_CARD, "filename.wav", playbackRate)
//    - Loops by default (pass false as 4th param to disable looping)
//
// 3. Pure microphone (no fallback):
//    AudioSourceConfig(AUDIO_MICROPHONE)
//
// 4. Keep current (no change):
//    AudioSourceConfig() or omit parameter
//
// The auto-fallback feature smoothly transitions to SD card playback
// (looping) when the microphone input is silent, and switches back 
// when audio is detected again.
//
