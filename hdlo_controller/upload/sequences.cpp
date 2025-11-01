///////////////////////////
// Sequence Definitions
// Hand-coded choreography for LED sequences
//
// All times are in SECONDS (converted internally)
//

#include "sequences.h"
#include "modelsequence.h"

void initializeSequences(modelsequence* seq) {
  
  seq->clearRegistry();

  //=================================================================
  // AUDIO SOURCE SWITCHING EXAMPLES
  //=================================================================
  
  // Example 1: Microphone-only sequence (default behavior)
  // Simply omit the audio parameter or use AudioSourceConfig()
  seq->startNewSequence("Microphone Demo", 30, true);
  seq->addStep("flowoctahedron", {"dark", "audio", "audio", "audio"}, 10, FADE, 2);
  seq->addStep("cycle", {"dark", "vumeter", "vumeter", "rainbow"}, 10, FADE, 2);
  seq->addStep("octachain", {"dark", "audio", "vumeter", "rainbow"}, 10, FADE, 2);
  
  //=================================================================
  // PALETTE SWITCHING EXAMPLES
  //=================================================================
  
  // Example: Palette Progression - Fire effect cycles through different palettes
  // NOTE: Palette switching happens between steps using switchPalette()
  seq->startNewSequence("Palette Progression", 60, true);
  
  // Step 1: Start with classic fire palette
  switchPalette("fire2012", "fire");
  seq->addStep("flowoctahedron", {"dark", "fire2012", "fire2012", "fire2012"}, 10, FADE, 2);
  
  // Step 2: Switch to lava palette (more yellow/orange)
  switchPalette("fire2012", "lava");
  seq->addStep("cycle", {"dark", "fire2012", "fire2012", "rainbow"}, 10, FADE, 2);
  
  // Step 3: Switch to ocean palette (blue fire effect)
  switchPalette("fire2012", "ocean");
  seq->addStep("octachain", {"dark", "fire2012", "fire2012", "staticblue"}, 10, FADE, 2);
  
  // Step 4: Switch to forest palette (green fire effect)
  switchPalette("fire2012", "forest");
  seq->addStep("cycles", {"dark", "fire2012", "rainbow", "fire2012"}, 10, FADE, 2);
  
  // Step 5: Switch to ice palette (cool colors)
  switchPalette("fire2012", "ice");
  seq->addStep("twentyfourcell", {"dark", "fire2012", "fire2012", "particles"}, 10, FADE, 2);
  
  // Step 6: Switch to party palette (multicolor)
  switchPalette("fire2012", "party");
  seq->addStep("hypercube", {"dark", "fire2012", "plasma", "fire2012"}, 10, FADE, 2);
  
  // Example: Multi-Function Palette Coordination
  // All palettized functions use the same palette progression
  seq->startNewSequence("Coordinated Palettes", 60, true);
  
  // Step 1: All functions use "rainbow"
  switchPalette("fire2012", "rainbow");
  switchPalette("audio", "rainbow");
  switchPalette("plasma", "rainbow");
  switchPalette("particles", "rainbow");
  seq->addStep("flowoctahedron", {"dark", "fire2012", "plasma", "audio"}, 10, FADE, 2,
               AudioSourceConfig(AUDIO_MICROPHONE));
  
  // Step 2: All functions use "lava"
  switchPalette("fire2012", "lava");
  switchPalette("audio", "lava");
  switchPalette("plasma", "lava");
  switchPalette("particles", "lava");
  seq->addStep("cycle", {"dark", "audio", "particles", "plasma"}, 10, FADE, 2);
  
  // Step 3: All functions use "ocean"
  switchPalette("fire2012", "ocean");
  switchPalette("audio", "ocean");
  switchPalette("plasma", "ocean");
  switchPalette("particles", "ocean");
  seq->addStep("octachain", {"dark", "plasma", "fire2012", "audio"}, 10, FADE, 2);
  
  // Step 4: All functions use "sunset"
  switchPalette("fire2012", "sunset");
  switchPalette("audio", "sunset");
  switchPalette("plasma", "sunset");
  switchPalette("particles", "sunset");
  seq->addStep("cycles", {"dark", "particles", "audio", "fire2012"}, 10, FADE, 2);
  
  // Step 5: All functions use "forest"
  switchPalette("fire2012", "forest");
  switchPalette("audio", "forest");
  switchPalette("plasma", "forest");
  switchPalette("particles", "forest");
  seq->addStep("twentyfourcell", {"dark", "fire2012", "plasma", "particles"}, 10, FADE, 2);
  
  // Step 6: All functions use "ice"
  switchPalette("fire2012", "ice");
  switchPalette("audio", "ice");
  switchPalette("plasma", "ice");
  switchPalette("particles", "ice");
  seq->addStep("hypercube", {"dark", "audio", "fire2012", "plasma"}, 10, FADE, 2);
  
  // Example: Mixed Palettes - Different functions use different palettes
  seq->startNewSequence("Mixed Palettes", 40, true);
  
  // Fire uses fire palette, plasma uses ocean, particles use rainbow
  switchPalette("fire2012", "fire");
  switchPalette("plasma", "ocean");
  switchPalette("particles", "rainbow");
  seq->addStep("flowoctahedron", {"dark", "fire2012", "plasma", "particles"}, 10, FADE, 2);
  
  // Switch to contrasting palettes
  switchPalette("fire2012", "ice");        // Cool
  switchPalette("plasma", "lava");         // Hot
  switchPalette("particles", "forest");    // Natural
  seq->addStep("cycle", {"dark", "particles", "fire2012", "plasma"}, 10, FADE, 2);
  
  // All different custom palettes
  switchPalette("fire2012", "sunset");
  switchPalette("plasma", "party");
  switchPalette("particles", "heat");
  seq->addStep("octachain", {"dark", "plasma", "particles", "fire2012"}, 10, FADE, 2);
  
  // Back to harmonious palettes
  switchPalette("fire2012", "ocean");
  switchPalette("plasma", "ocean");
  switchPalette("particles", "ocean");
  seq->addStep("cycles", {"dark", "fire2012", "plasma", "particles"}, 10, FADE, 2);
  
  // Example: Audio + Palette Switching
  seq->startNewSequence("Audio Palette Journey", 50, true);
  
  // Energetic rainbow start
  switchPalette("audio", "rainbow");
  seq->addStep("flowoctahedron", {"dark", "audio", "audio", "audio"}, 10, FADE, 2,
               AudioSourceConfig(AUDIO_MICROPHONE));
  
  // Shift to hot colors
  switchPalette("audio", "lava");
  switchPalette("fire2012", "fire");
  seq->addStep("cycle", {"dark", "audio", "audio", "fire2012"}, 10, FADE, 2);
  
  // Cool down to ocean
  switchPalette("audio", "ocean");
  switchPalette("plasma", "ice");
  seq->addStep("octachain", {"dark", "audio", "audio", "plasma"}, 10, FADE, 2);
  
  // Nature vibes
  switchPalette("audio", "forest");
  switchPalette("freqbands", "forest");
  seq->addStep("cycles", {"dark", "freqbands", "audio", "particles"}, 10, FADE, 2);
  
  // Party mode!
  switchPalette("audio", "party");
  switchPalette("beatdetect", "party");
  seq->addStep("twentyfourcell", {"dark", "beatdetect", "audio", "fire2012"}, 10, FADE, 2);
  
  // Example 2: Explicit microphone switching
  // Use AudioSourceConfig(AUDIO_MICROPHONE) to explicitly set microphone
  seq->startNewSequence("Explicit Microphone", 20, true);
  seq->addStep("flowoctahedron", {"dark", "audio", "rainbow", "audio"}, 10, FADE, 2, 
               AudioSourceConfig(AUDIO_MICROPHONE));
  seq->addStep("cycle", {"dark", "vumeter", "fire2012", "vumeter"}, 10, FADE, 2,
               AudioSourceConfig(AUDIO_MICROPHONE));
  
  // Example 3: SD Card playback at normal speed
  // AudioSourceConfig(AUDIO_SD_CARD, "filename.wav", playbackRate, looping)
  seq->startNewSequence("SD Card Demo", 60, true);
  seq->addStep("flowoctahedron", {"dark", "audio", "audio", "fire2012"}, 20, FADE, 2,
               AudioSourceConfig(AUDIO_SD_CARD, "music.wav", 1.0, true)); // Normal speed, looping
  seq->addStep("cycle", {"dark", "vumeter", "rainbow", "audio"}, 20, FADE, 2,
               AudioSourceConfig(AUDIO_KEEP_CURRENT)); // Keep playing the same file
  seq->addStep("octachain", {"dark", "audio", "fire2012_blue", "vumeter"}, 20, FADE, 2,
               AudioSourceConfig(AUDIO_KEEP_CURRENT)); // Still playing
  
  // Example 4: SD Card with variable playback speed (if using AudioPlaySdResmp)
  seq->startNewSequence("Variable Speed SD", 40, true);
  seq->addStep("flowoctahedron", {"dark", "audio", "plasma", "audio"}, 10, FADE, 1,
               AudioSourceConfig(AUDIO_SD_CARD, "beat.wav", 0.75, true)); // 75% speed (slower)
  seq->addStep("cycle", {"dark", "vumeter", "particles", "vumeter"}, 10, FADE, 1,
               AudioSourceConfig(AUDIO_SD_CARD, "beat.wav", 1.0, true)); // 100% speed (normal)
  seq->addStep("octachain", {"dark", "audio", "fire2012", "audio"}, 10, FADE, 1,
               AudioSourceConfig(AUDIO_SD_CARD, "beat.wav", 1.5, true)); // 150% speed (faster)
  seq->addStep("cycles", {"dark", "vumeter", "rainbow", "vumeter"}, 10, FADE, 1,
               AudioSourceConfig(AUDIO_SD_CARD, "beat.wav", 2.0, true)); // 200% speed (double)
  
  // Example 5: Switching between microphone and SD card
  seq->startNewSequence("Source Switcher", 60, true);
  // Start with microphone
  seq->addStep("flowoctahedron", {"dark", "audio", "audio", "rainbow"}, 10, FADE, 2,
               AudioSourceConfig(AUDIO_MICROPHONE));
  // Switch to SD card
  seq->addStep("cycle", {"dark", "vumeter", "fire2012", "vumeter"}, 10, FADE, 2,
               AudioSourceConfig(AUDIO_SD_CARD, "drums.wav", 1.0, true));
  // Keep SD card playing
  seq->addStep("octachain", {"dark", "audio", "plasma", "audio"}, 10, FADE, 2,
               AudioSourceConfig(AUDIO_KEEP_CURRENT));
  // Switch back to microphone
  seq->addStep("cycles", {"dark", "vumeter", "particles", "vumeter"}, 10, FADE, 2,
               AudioSourceConfig(AUDIO_MICROPHONE));
  // Different SD file
  seq->addStep("twentyfourcell", {"dark", "audio", "fire2012_blue", "audio"}, 10, FADE, 2,
               AudioSourceConfig(AUDIO_SD_CARD, "ambient.wav", 1.0, false)); // No looping
  // Back to mic for finale
  seq->addStep("hypercube", {"dark", "vumeter", "rainbow", "audio"}, 10, FADE, 2,
               AudioSourceConfig(AUDIO_MICROPHONE));
  
  // Example: ULTIMATE COMBO - Audio Source + Palette Switching Together!
  seq->startNewSequence("Ultimate Audio+Palette", 60, true);
  
  // Start: Microphone + Rainbow
  switchPalette("audio", "rainbow");
  switchPalette("particles", "rainbow");
  seq->addStep("flowoctahedron", {"dark", "audio", "audio", "particles"}, 10, FADE, 2,
               AudioSourceConfig(AUDIO_MICROPHONE));
  
  // SD Card + Lava palette (hot sound, hot colors)
  switchPalette("audio", "lava");
  switchPalette("fire2012", "fire");
  seq->addStep("cycle", {"dark", "audio", "fire2012", "audio"}, 10, FADE, 2,
               AudioSourceConfig(AUDIO_SD_CARD, "drums.wav", 1.0, true));
  
  // Keep SD + Ocean palette (keep sound, cool colors)
  switchPalette("audio", "ocean");
  switchPalette("plasma", "ice");
  seq->addStep("octachain", {"dark", "audio", "plasma", "audio"}, 10, FADE, 2,
               AudioSourceConfig(AUDIO_KEEP_CURRENT));
  
  // Microphone + Forest palette (live sound, natural colors)
  switchPalette("audio", "forest");
  switchPalette("freqbands", "forest");
  seq->addStep("cycles", {"dark", "freqbands", "audio", "particles"}, 10, FADE, 2,
               AudioSourceConfig(AUDIO_MICROPHONE));
  
  // SD Card + Party palette (beat + party colors!)
  switchPalette("audio", "party");
  switchPalette("beatdetect", "party");
  seq->addStep("twentyfourcell", {"dark", "beatdetect", "audio", "fire2012"}, 10, FADE, 2,
               AudioSourceConfig(AUDIO_SD_CARD, "beat.wav", 1.5, true)); // 150% speed!
  
  // Finale: Microphone + Sunset palette
  switchPalette("audio", "sunset");
  switchPalette("plasma", "sunset");
  seq->addStep("hypercube", {"dark", "audio", "plasma", "audio"}, 10, FADE, 2,
               AudioSourceConfig(AUDIO_MICROPHONE));
  
  // Example 6: Non-looping SD card (plays once then stops)
  seq->startNewSequence("One-Shot Playback", 30, true);
  seq->addStep("flowoctahedron", {"dark", "audio", "audio", "fire2012"}, 30, FADE, 2,
               AudioSourceConfig(AUDIO_SD_CARD, "intro.wav", 1.0, false)); // No loop - plays once

  //=================================================================
  // ORIGINAL SEQUENCES (UPDATED)
  //=================================================================

  // Sequence: Audio Reactive - 60 seconds (using microphone)
  seq->startNewSequence("Audio Reactive", 60, true);
  seq->addStep("flowoctahedron", {"dark", "audio", "audio", "audio"}, 10, FADE, 2,
               AudioSourceConfig(AUDIO_MICROPHONE));
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
