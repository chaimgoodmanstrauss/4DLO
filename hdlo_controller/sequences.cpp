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
const float fftspectrumsensitivity = 20000;//increase the intensity when it is shown
const float fftspectrumthreshold = .01;// but raise the gate to show it. 

// Palette switching threshold - when to switch from background to audio palette
const float AUDIO_PALETTE_SWITCH_THRESHOLD = 0.025; //somehow none of these seem to be on the same scale. 

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
        {"fftspectrum", "sunset", fftspectrumsensitivity, fftspectrumthreshold},
        {"fftspectrum", "party", fftspectrumsensitivity, fftspectrumthreshold},
        {"fftspectrum", "rainbow", fftspectrumsensitivity, fftspectrumthreshold},
        {"fftfire", "heat", 30.0, 0.0, fftthreshold},
        {"fftfire", "rainbow", 30.0, 0.0, fftthreshold},
        {"fftfire", "rainbow", 30.0, 0.0, fftthreshold}
    });
    
    // Background palette - when quiet
    seq.setbackgroundpalette({
        "dark",
    /*    {"perlin", "cloud", 2.0, 30.0,0},
        {"perlin", "cloud", 4.0, 40.0,.1},
        {"perlin", "cloud", 5.0, 20.0,.4},
        {"perlin", "cloud", 7.0, 20.0,1},
        {"perlin", "cloud", 15.0, 20.0,1},
        {"perlin", "cloud", 7.0, 20.0}*/
        {"perlin", "cloud",  2.0,3,.03},
        {"perlin", "heat",   2,3,.1},
        {"perlin", "ocean",  2,100,.2},
        {"perlin", "sunset",  10,3,0},
        {"perlin", "forest",  10,3,.5},
        {"perlin", "rainbow",  10,3,.5}
    });
    
    seq.addstep("test", 10.0, FADE, 0.4);
    seq.addstep("test", "simpletest", 10.0, FADE, 2.0);   
    
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
