////////////////////////////////////
//
//   sequences.cpp
//
// Multiple named sequences with independent durations and enable states
//
// ARCHITECTURE:
// - Each sequence is built by its own function (buildMainSequence, etc.)
// - Registry system manages switching between sequences
// - Each sequence has: name, duration (in seconds), enabled state
// - Only enabled sequences are active in rotation
//
// SEQUENCES:
// 1. "Main Show"  - 12 steps, 120 sec, enabled by default
// 2. "Night Mode" - 6 steps, 90 sec, disabled by default  
// 3. "Party Mode" - 6 steps, 60 sec, disabled by default
//
// To enable/disable sequences, modify the 'enabled' parameter in beginRegistry()
// or use runtime controls (to be implemented)
//

#include "modelsequence.h"
#include "hdlo_models.h"

// Global sequence object
modelsequence mainSequence;

/////////////////////////////////////////
// SEQUENCE BUILDERS
/////////////////////////////////////////

const float fftthreshold = .008;//needed for the fftfire, tuned to the physical microphone

void buildMainSequence(SequenceBuilder& seq) {
    mainSequence.beginRegistry("Main Show", 3000, true);

//restful:
seq.add("test_merged", {
        "dark",  
        {"noiseperlin", "forest", 3.0, 5.0}, 
        {"plasma", "ocean_builtin",-1},
        {"noiseperlin", "rainbow", 1.0, 20.0},
        {"noiseperlin", "ocean_builtin", 5.0, 20.0},
        {"plasma", "sunset", 1.0, 10.0},
        {"noiseperlin", "heat", 5.0, 5.0},
        }, 20);



/*
//cylon variations
seq.add("test", {
        "dark",  
       {"fftfire", "fire",30,0,.005}, 
       {"cylon", "heat", 1.0, 0, 1.0},         // speed=1, bounce, phase=1
       {"cylon", "ocean_builtin", 0.5, 0, 2.0},       // speed=0.5, bounce, phase=2 (faster palette change)
       {"cylon", "rainbow", 1.5, 1, 1.0},      // speed=1.5, forward loop, phase=1
       {"cylon", "ocean_builtin", 1.0, 0, 0.5}, // speed=1, bounce, phase=0.5 (slower palette)
       {"cylon", "forest", 2.0, -1, 3.0},      // speed=2, backward loop, phase=3 (fast palette)
       }, 20);
*/


/*
//noise perlin variations:
seq.add("test", {
        "dark",
        {"noiseperlin", "forest", 25.0, 30.0}, 
        {"noiseperlin", "rainbow", 25.0, 5.0},
        {"noiseperlin", "ocean_builtin", 15.0, 10.0},
        {"noiseperlin", "sunset", 25.0, 10.0},
        {"noiseperlin", "cloud", 15.0, 10.0},  
         {"noiseperlin", "heat", 25.0, 30.0},
              
    }, 20);
*/

/*
    seq.add("test", {
        "breathing",   
        "fftfire", 
        {"fftfire", "heat", 30.0, 0,fftthreshold},
        {"fftfire", "rainbow", 30.0, 0,fftthreshold},
        {"fftfire", "ice", 30.0, 0,fftthreshold},
        {"fftfire", "sunset", 30.0, 0,fftthreshold},
        {"fftfire", "forest", 30.0, 0,fftthreshold},
        {"fftfire", "ocean", 30.0, 0,fftthreshold},
    }, 20);*/

    
    mainSequence.endRegistry();
}

void buildNightSequence(SequenceBuilder& seq) {
    mainSequence.beginRegistry("Night Mode", 90, false);
    
    //===========================================
    // NIGHT SEQUENCE: 6 steps, calm and dim
    //===========================================
    
    // Step 1: Deep ocean calm
    seq.add("test", {
        {"breathing", "", 0.4},                    // 0: Very slow breathing
        {"plasma", "ocean", 0.5},                  // 1: Very slow ocean plasma
        {"noiseperlin", "ice", 8.0, 25.0},         // 2: Gentle ice noise
        {"plasma", "cloud", 0.6},                  // 3: Soft cloud plasma
        {"noiseperlin", "ocean", 10.0, 30.0},      // 4: Ocean noise
        {"simplecolor", "blue"},                   // 5: Dim blue
        {"plasma", "forest", 0.4}                  // 6: Forest plasma
    }, 15);
    
    // Step 2: Starfield ambient
    seq.add("test", {
        {"breathing", "", 0.35},                   // 0: Ultra slow breathing
        {"noiseperlin", "cloud", 6.0, 20.0},       // 1: Cloud drift
        {"particles", "ice"},                      // 2: Sparse particles
        {"plasma", "ice", 0.45},                   // 3: Ice plasma
        {"noiseperlin", "forest", 9.0, 28.0},      // 4: Forest whisper
        {"simplecolor", "green"},                  // 5: Dim green
        {"plasma", "ocean", 0.5}                   // 6: Ocean plasma
    }, 15);
    
    // Step 3: Twilight glow
    seq.add("test", {
        {"breathing", "", 0.5},                    // 0: Slow breathing
        {"plasma", "lava", 0.6},                   // 1: Gentle lava glow
        {"noiseperlin", "magma", 12.0, 32.0},      // 2: Warm magma noise
        {"fire2012", "lava", 65.0, 80.0},          // 3: Gentle fire
        {"plasma", "cloud", 0.55},                 // 4: Cloud plasma
        {"noiseperlin", "ice", 10.0, 30.0},        // 5: Cool ice noise
        {"simplecolor", "red"}                     // 6: Dim red
    }, 15);
    
    // Step 4: Moonlight shimmer
    seq.add("test", {
        {"breathing", "", 0.4},                    // 0: Very slow breathing
        {"plasma", "ice", 0.5},                    // 1: Ice shimmer
        {"noiseperlin", "cloud", 8.0, 26.0},       // 2: Cloud drift
        {"plasma", "ocean", 0.6},                  // 3: Ocean waves
        {"particles", "cloud"},                    // 4: Floating particles
        {"noiseperlin", "ice", 11.0, 33.0},        // 5: Ice patterns
        {"plasma", "forest", 0.45}                 // 6: Forest glow
    }, 15);
    
    // Step 5: Deep rest
    seq.add("test", {
        {"breathing", "", 0.3},                    // 0: Ultra slow breathing
        {"noiseperlin", "ocean", 7.0, 22.0},       // 1: Deep ocean
        {"plasma", "ice", 0.4},                    // 2: Slow ice
        {"simplecolor", "blue"},                   // 3: Calm blue
        {"noiseperlin", "forest", 8.0, 24.0},      // 4: Forest night
        {"plasma", "cloud", 0.5},                  // 5: Cloud drift
        {"noiseperlin", "cloud", 9.0, 27.0}        // 6: Soft clouds
    }, 15);
    
    // Step 6: Dawn approach
    seq.add("test", {
        {"breathing", "", 0.6},                    // 0: Slow breathing
        {"plasma", "ocean", 0.7},                  // 1: Ocean awakening
        {"noiseperlin", "ice", 12.0, 35.0},        // 2: Ice patterns
        {"fire2012", "lava", 60.0, 90.0},          // 3: Gentle warming
        {"plasma", "forest", 0.65},                // 4: Forest light
        {"noiseperlin", "magma", 14.0, 38.0},      // 5: Warm glow
        {"plasma", "cloud", 0.7}                   // 6: Morning clouds
    }, 15);
    
    mainSequence.endRegistry();
}

void buildPartySequence(SequenceBuilder& seq) {
    mainSequence.beginRegistry("Party Mode", 60, false);
    
    //===========================================
    // PARTY SEQUENCE: 6 steps, high energy audio reactive
    //===========================================
    
    // Step 1: Audio explosion
    seq.add("test", {
        {"breathing", "", 2.5},                    // 0: Fast breathing
        {"beatdetect", "rainbow"},                 // 1: Beat rainbow
        {"audiocylon", "fire", 0.04},              // 2: Fast audio cylon
        {"fftfire", "magma"},                      // 3: FFT fire
        {"vumeter", "party"},                      // 4: VU party
        {"audio", "lava"},                         // 5: Audio lava
        {"plasma", "rainbow", 2.5}                 // 6: Fast rainbow plasma
    }, 10);
    
    // Step 2: Beat driven
    seq.add("test", {
        {"breathing", "", 3.0},                    // 0: Very fast breathing
        {"beatdetect", "fire"},                    // 1: Beat fire
        {"vumeter", "rainbow"},                    // 2: VU rainbow
        {"audiocylon", "magma", 0.045},            // 3: Fast audiocylon
        {"fftfire", "lava"},                       // 4: FFT lava
        {"audio2", "party"},                       // 5: Audio2 party
        {"rainbow", "", 8.0}                       // 6: Ultra fast rainbow
    }, 10);
    
    // Step 3: Visual chaos
    seq.add("test", {
        {"breathing", "", 2.8},                    // 0: Fast breathing
        {"rainbow", "", 7.0},                      // 1: Fast rainbow
        {"fire2012", "fire", 35.0, 160.0},         // 2: Intense fire
        {"plasma", "party", 3.0},                  // 3: Fast party plasma
        {"cylon", "rainbow", 0.05, 1, 4.0},        // 4: Fast cylon, forward loop, rapid palette cycle
        {"beatdetect", "magma"},                   // 5: Beat magma
        {"audiocylon", "lava", 0.05}               // 6: Fast audiocylon lava
    }, 10);
    
    // Step 4: Audio peak
    seq.add("test", {
        {"breathing", "", 3.5},                    // 0: Ultra fast breathing
        {"fftfire", "fire"},                       // 1: FFT fire
        {"beatdetect", "party"},                   // 2: Beat party
        {"audio", "rainbow"},                      // 3: Audio rainbow
        {"vumeter", "lava"},                       // 4: VU lava
        {"audiocylon", "ocean", 0.05},             // 5: Fast audiocylon ocean
        {"vocals", "magma"}                        // 6: Vocals magma
    }, 10);
    
    // Step 5: Strobe energy
    seq.add("test", {
        {"breathing", "", 4.0},                    // 0: Extreme fast breathing
        {"beatdetect", "rainbow"},                 // 1: Beat rainbow
        {"plasma", "fire", 3.5},                   // 2: Ultra fast fire plasma
        {"audiocylon", "party", 0.06},             // 3: Ultra fast audiocylon
        {"rainbow", "", 9.0},                      // 4: Maximum rainbow speed
        {"fftfire", "lava"},                       // 5: FFT lava
        {"vumeter", "fire"}                        // 6: VU fire
    }, 10);
    
    // Step 6: Grand finale
    seq.add("test", {
        {"breathing", "", 3.2},                    // 0: Very fast breathing
        {"audio2", "rainbow"},                     // 1: Audio2 rainbow
        {"fire2012", "magma", 30.0, 170.0},        // 2: Maximum fire
        {"beatdetect", "lava"},                    // 3: Beat lava
        {"plasma", "rainbow", 3.0},                // 4: Fast rainbow plasma
        {"audiocylon", "fire", 0.055},             // 5: Fast audiocylon fire
        {"fftfire", "party"}                       // 6: FFT party
    }, 10);
    
    mainSequence.endRegistry();
}

/////////////////////////////////////////
// INITIALIZATION
/////////////////////////////////////////

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
    
    // Build all sequences
    SequenceBuilder seq(&mainSequence);
    
    buildMainSequence(seq);
    buildNightSequence(seq);
    buildPartySequence(seq);
    
    // Start with Main Show (first registry entry)
    mainSequence.reset();
    
    Serial.println("Sequences initialized!");
    Serial.print("Total registry entries: ");
    Serial.println(mainSequence.getNumRegistryEntries());
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
