////////////////////////////////////
//
//   sequences.cpp
//
// 12-step sequence with organized function groups
// Structure: 7 functions per step (breathing + 6 others)
//
// Steps 1-3:   Noise functions + palettes
// Steps 4-6:   Audio reactive functions
// Steps 7-9:   Misc static functions
// Steps 10-12: Function pairs with parameter variations
//

#include "modelsequence.h"
#include "hdlo_models.h"

// Global sequence object
modelsequence mainSequence;

void initializeSequences() {
    Serial.println("Initializing sequences...");
    
    colormodel* testModel = colormodel::findModelByName("test");
    
    if(!testModel) {
        Serial.println("ERROR: test model not found!");
        colormodel::printRegistry();
        return;
    }
    
    Serial.print("Found model: ");
    Serial.println(testModel->getModelName());
    
    SequenceBuilder seq(&mainSequence);
    
    /////////////////////////////////////////
    // MAIN SEQUENCE: 12 steps, 7 functions each
    // Duration: 120 seconds (2 minutes)
    /////////////////////////////////////////
    mainSequence.beginRegistry("Main Show", 120, true);
    
    //===========================================
    // STEPS 1-3: NOISE FUNCTIONS + PALETTES
    //===========================================
    
    // Step 1: Noise showcase - slow breathing
    seq.add("test", {
        {"breathing", "", 0.5},                    // 0: Slow breathing
        {"noiseperlin", "ocean", 12.0, 35.0},      // 1: Gentle ocean noise
        {"noiseperlin", "forest", 18.0, 45.0},     // 2: Forest noise
        {"plasma", "ice", 0.6},                    // 3: Slow ice plasma
        {"plasma", "lava", 1.2},                   // 4: Medium lava plasma
        {"noiseperlin", "cloud", 10.0, 30.0},      // 5: Soft cloud noise
        {"plasma", "rainbow", 0.8}                 // 6: Rainbow plasma
    }, 10);
    
    // Step 2: Noise variations - medium breathing
    seq.add("test", {
        {"breathing", "", 1.0},                    // 0: Medium breathing
        {"plasma", "fire", 1.5},                   // 1: Fire plasma
        {"noiseperlin", "magma", 22.0, 55.0},      // 2: Magma noise
        {"plasma", "ocean", 1.0},                  // 3: Ocean plasma
        {"noiseperlin", "party", 25.0, 60.0},      // 4: Party noise
        {"plasma", "forest", 0.9},                 // 5: Forest plasma
        {"noiseperlin", "ice", 15.0, 40.0}         // 6: Ice noise
    }, 10);
    
    // Step 3: Noise intensity - fast breathing
    seq.add("test", {
        {"breathing", "", 2.0},                    // 0: Fast breathing
        {"noiseperlin", "rainbow", 30.0, 70.0},    // 1: Fast rainbow noise
        {"plasma", "magma", 2.0},                  // 2: Fast magma plasma
        {"noiseperlin", "lava", 28.0, 65.0},       // 3: Lava noise
        {"plasma", "cloud", 1.8},                  // 4: Fast cloud plasma
        {"noiseperlin", "fire", 35.0, 75.0},       // 5: Fire noise
        {"plasma", "party", 2.5}                   // 6: Fast party plasma
    }, 10);
    
    //===========================================
    // STEPS 4-6: AUDIO REACTIVE FUNCTIONS
    //===========================================
    
    // Step 4: Audio basics - slow breathing
    seq.add("test", {
        {"breathing", "", 0.6},                    // 0: Slow breathing
        {"audio", "rainbow"},                      // 1: Basic audio rainbow
        {"audiocylon", "fire", 0.02},              // 2: Audio cylon fire
        {"vumeter", "ocean"},                      // 3: VU meter ocean
        {"beatdetect", "lava"},                    // 4: Beat detection lava
        {"audio2", "forest"},                      // 5: Audio2 forest
        {"vocals", "ice"}                          // 6: Vocals ice
    }, 10);
    
    // Step 5: Audio variety - medium breathing
    seq.add("test", {
        {"breathing", "", 1.2},                    // 0: Medium breathing
        {"fftfire", "fire"},                       // 1: FFT fire
        {"vumeter", "magma"},                      // 2: VU meter magma
        {"beatdetect", "rainbow"},                 // 3: Beat detect rainbow
        {"audiocylon", "ocean", 0.03},             // 4: Audio cylon ocean
        {"audio", "party"},                        // 5: Audio party
        {"vocals", "cloud"}                        // 6: Vocals cloud
    }, 10);
    
    // Step 6: Audio intensity - fast breathing
    seq.add("test", {
        {"breathing", "", 2.5},                    // 0: Fast breathing
        {"beatdetect", "fire"},                    // 1: Beat detect fire
        {"audio2", "lava"},                        // 2: Audio2 lava
        {"fftfire", "magma"},                      // 3: FFT fire magma
        {"audiocylon", "rainbow", 0.04},           // 4: Fast audio cylon
        {"vumeter", "ice"},                        // 5: VU meter ice
        {"audio", "ocean"}                         // 6: Audio ocean
    }, 10);
    
    //===========================================
    // STEPS 7-9: MISC STATIC FUNCTIONS
    //===========================================
    
    // Step 7: Static mix - very slow breathing
    seq.add("test", {
        {"breathing", "", 0.3},                    // 0: Very slow breathing
        {"rainbow", "", 2.0},                      // 1: Slow rainbow
        {"cylon", "ocean", 0.015},                 // 2: Slow cylon ocean
        {"fire2012", "fire", 55.0, 120.0},         // 3: Fire effect
        {"particles", "forest"},                   // 4: Particles forest
        {"simplecolor", "blue"},                   // 5: Simple blue
        {"rainbow", "", 3.5}                       // 6: Medium rainbow
    }, 10);
    
    // Step 8: Static variety - medium-slow breathing
    seq.add("test", {
        {"breathing", "", 0.8},                    // 0: Medium-slow breathing
        {"fire2012", "lava", 50.0, 130.0},         // 1: Fire lava
        {"cylon", "rainbow", 0.025},               // 2: Cylon rainbow
        {"particles", "ice"},                      // 3: Particles ice
        {"rainbow", "", 4.0},                      // 4: Medium-fast rainbow
        {"simplecolor", "red"},                    // 5: Simple red
        {"fire2012", "magma", 45.0, 140.0}         // 6: Fire magma
    }, 10);
    
    // Step 9: Static energy - medium-fast breathing
    seq.add("test", {
        {"breathing", "", 1.8},                    // 0: Medium-fast breathing
        {"cylon", "fire", 0.035},                  // 1: Medium cylon fire
        {"rainbow", "", 6.0},                      // 2: Fast rainbow
        {"particles", "party"},                    // 3: Particles party
        {"fire2012", "fire", 40.0, 150.0},         // 4: Intense fire
        {"cylon", "ocean", 0.04},                  // 5: Fast cylon ocean
        {"simplecolor", "green"}                   // 6: Simple green
    }, 10);
    
    //===========================================
    // STEPS 10-12: FUNCTION PAIRS W/ PARAMETER VARIATIONS
    //===========================================
    
    // Step 10: Fire2012 parameter variations - slow breathing
    seq.add("test", {
        {"breathing", "", 0.7},                    // 0: Slow breathing
        {"fire2012", "fire", 60.0, 100.0},         // 1: Balanced fire
        {"fire2012", "lava", 45.0, 130.0},         // 2: Cooler, more sparking
        {"fire2012", "magma", 70.0, 80.0},         // 3: Warmer, less sparking
        {"plasma", "ocean", 1.0},                  // 4: Ocean plasma
        {"rainbow", "", 3.0},                      // 5: Medium rainbow
        {"cylon", "ice", 0.02}                     // 6: Slow cylon ice
    }, 10);
    
    // Step 11: Noiseperlin parameter variations - medium breathing
    seq.add("test", {
        {"breathing", "", 1.3},                    // 0: Medium breathing
        {"noiseperlin", "rainbow", 15.0, 40.0},    // 1: Moderate speed/scale
        {"noiseperlin", "fire", 25.0, 60.0},       // 2: Faster, larger scale
        {"noiseperlin", "ocean", 10.0, 30.0},      // 3: Slower, smaller scale
        {"audio", "lava"},                         // 4: Audio lava
        {"particles", "forest"},                   // 5: Particles forest
        {"audiocylon", "party", 0.025}             // 6: Audio cylon party
    }, 10);
    
    // Step 12: Plasma parameter variations - fast breathing
    seq.add("test", {
        {"breathing", "", 2.2},                    // 0: Fast breathing
        {"plasma", "fire", 1.5},                   // 1: Medium speed
        {"plasma", "ocean", 0.8},                  // 2: Slow speed
        {"plasma", "rainbow", 2.5},                // 3: Fast speed
        {"beatdetect", "lava"},                    // 4: Beat detect lava
        {"vumeter", "ice"},                        // 5: VU meter ice
        {"fftfire", "magma"}                       // 6: FFT fire magma
    }, 10);
    
    mainSequence.endRegistry();
    
    // Start the sequence
    mainSequence.reset();
    
    Serial.println("Sequences initialized!");
    mainSequence.printSequenceInfo();
}

// Call this from loop()
void updateSequence() {
    mainSequence.update();
}

// Get color for rendering
CRGB getSequenceColor(int edgeindex, float position) {
    return mainSequence.getColor(edgeindex, position);
}
