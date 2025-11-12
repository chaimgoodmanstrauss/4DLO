////////////////////////////////////
//
//   sequences.cpp
//
// Minimal dual palette implementation
//

#include "namedpermutations.h"
#include "modelsequence.h"
#include "hdlo_models.h"

// Global sequence object
modelsequence mainSequence;

// FFT threshold for fftfire function - filters noise within the fire effect
const float fftthreshold = 0.008; 

// Palette switching threshold - when to switch from background to audio palette
const float AUDIO_PALETTE_SWITCH_THRESHOLD = 0.02; 

// How long (seconds) to wait after audio drops below threshold before fading back to background
const float AUDIO_TIMEOUT_SECONDS = 2.0;

void initializeSequences() {
    Serial.println("\n=== Initializing sequences ===");
    
    
    SequenceBuilder seq(&mainSequence);
    
    // Main sequence with dual palettes
    mainSequence.beginRegistry("Main Show", 3000.0, true);
    
    // Audio palette - activated by sound
    seq.setaudiopalette({
        "dark",
        {"fftfire", "ocean", 30.0, 0.0, fftthreshold},
        {"fftfire", "cloud", 30.0, 0.0, fftthreshold},
        {"fftfire", "rainbow", 30.0, 0.0, fftthreshold},
        {"fftfire", "ice", 30.0, 0.0, fftthreshold},
        {"fftfire", "sunset", 30.0, 0.0, fftthreshold},
        {"fftfire", "forest", 30.0, 0.0, fftthreshold}
    });
    
    // Background palette - when quiet
    seq.setbackgroundpalette({
        "dark",
        {"noiseperlin", "ocean", 7.0, 20.0},
        {"noiseperlin", "ocean", 7.0, 20.0},
        {"noiseperlin", "ocean", 7.0, 20.0},
        {"noiseperlin", "ocean", 7.0, 20.0},
        {"noiseperlin", "ocean", 7.0, 20.0},
        {"noiseperlin", "ocean", 7.0, 20.0}
    });
    
    seq.addstep("test", 10.0, FADE, 0.4, true);
    seq.addstep("test", "simpletest", 10.0, FADE, 2.0, true);   
    
    mainSequence.endRegistry();
    
    // Reset and start
    mainSequence.reset();
    
}

void updateSequence() {
    mainSequence.update();
}

CRGB getSequenceColor(int edgeindex, float position) {
    return mainSequence.getColor(edgeindex, position);
}
