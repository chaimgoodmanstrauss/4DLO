////////////////////////////////////
//
//   sequences.cpp
//
// Minimal dual palette implementation
//

#include "namedpermutations.h"
#include "modelsequence.h"
#include "hdlo_models.h"


void octaswirl(SequenceBuilder& seq, String modelname = "octahedron", 
               float steplength = 1.0, float fadelength = 0.3) {
    for(int i = 0; i < 24; i++) {
        seq.addstep(modelname, octacellperms[i], steplength, FADE, fadelength);
    }
}

  void cubeswirl(SequenceBuilder& seq,  String modelname ="cube",float steplength=1,float fadelength=.3){
  for(int i=0; i<24;i++){
    seq.addstep(modelname, cubecellperms[i],steplength,FADE,fadelength);
  }}



// Global sequence object
modelsequence mainSequence;

// FFT threshold for fftfire function - filters noise within the fire effect
const float fftthreshold = 0.008; 
const float fftspectrumsensitivity = 20000;//increase the intensity when it is shown
const float fftspectrumthreshold = .008;// but raise the gate to show it. 

// Palette switching threshold - when to switch from background to audio palette
const float AUDIO_PALETTE_SWITCH_THRESHOLD = 0.020; //somehow none of these seem to be on the same scale. 

// How long (seconds) to wait after audio drops below threshold before fading back to background
const float AUDIO_TIMEOUT_SECONDS = 2.0;

void initializeSequences() {
    Serial.println("\n=== Initializing sequences ===");
    
    
    SequenceBuilder seq(&mainSequence);
    
    // Main sequence with dual palettes
    mainSequence.beginRegistry("Main Show", 84600.0, true);
    
    seq.setaudiotimeout(2.0); // I think this is currently blanked out and is a const, easily reset.

    //seq.setaudiosource(AudioSourceConfig::LINE_IN);
    // Switch to SD card with looping
    // seq.setaudiosource(AudioSourceConfig::SD_CARD, "music.wav", true);
    // Switch back to microphone
    seq.setaudiosource(AudioSourceConfig::MICROPHONE);
    
    // Audio palette - activated by sound
    seq.setaudiopalette({
        "dark",
        //threshold,  velocity, gravity, size, bounce decay
      /*  {"fftspectrum", "heat"},  
        {"fftspectrum", "heat"},  
        {"fftspectrum", "heat"},  
        {"fftspectrum", "heat"},  
         {"fftspectrum", "heat"},  
        {"fftspectrum", "heat"},*/
        {"fftfire", "rainbow", 30.0, 0,fftthreshold},  

        {"fftspectrum","party",1},
        {"fftballs", "rainbow", 0.001, 0.1, 0.002, 0.01, 0.1},
         {"fftfire", "rainbow", 30.0, 0,fftthreshold},  
        {"fftfire", "sunset", 30.0, 0,fftthreshold},
         {"fftfire", "rainbow", 30.0, 0,fftthreshold},  
     //   {"fftfire", "sunset", 30.0, 0,fftthreshold},
        


    /*    {"fftballs", "rainbow", 0.001, 0.1, 0.002, 0.01, 0.1},
        {"fftballs", "rainbow", 0.002, 0.1, 0.002, 0.01, 0.2},
        {"fftballs", "rainbow", 0.005, 0.1, 0.002, 0.01, 0.3},
        {"fftballs", "rainbow", 0.008, 0.1, 0.002, 0.04, 0.2},
        {"fftballs", "rainbow", 0.01, 0.1, 0.002, 0.025, 0.6},
        {"fftballs", "rainbow", 0.02, 0.1, 0.002, 0.04, 0.8},*/
     
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
        /*{"perlin", "cloud",  2.0,3,.03},
        {"perlin", "heat",   2,3,.1},
        {"perlin", "ocean",  2,100,.2},
        {"perlin", "sunset",  10,3,0},
        {"perlin", "forest",  10,3,.5},
        {"perlin", "rainbow",  10,3,.5}*/
       /* {"dualblobs", "white", 0.05, .5, 0.5},    // Default settings
    {"dualblobs", "heat", 0.1, 1, 0.8},    // Faster, more particles, more sparkle
    {"dualblobs", "forest", 0.2, 3, 0.3},  // Slower, fewer particles, less sparkle
    {"dualblobs", "ocean", 0.02, 1, 1.0},    // Fast, dense, max sparkle
    {"dualblobs", "party", 0.03, 4, 0.2},   // Very slow drift, minimal sparkle
    {"dualblobs", "red", 0.012, 10, 0.6}*/

    // Pure palette colors (randomColors=0)
    {"simplecolor", "rainbow", 2},
    {"simplecolor", "blue", 2},
    {"simplecolor", "red", 2},
   {"perlin", "ocean"},
    {"perlin", "rainbow", 0.02, 1, 1.0,1,5},
    {"perlin", "rainbow", 0.02, 1, 1.0,1,5},
    {"perlin", "rainbow", 0.02, 1, 1.0,1,5},
   {"simplecolor", "forest", 1.5},
   {"simplecolor", "rainbow", 1.5},
   {"simplecolor", "ocean", .4},
    {"perlin", "ocean"},//, 0.02, 1, 1,  0,5},
    
    {"perlin", "rainbow", 0.02, 1, 1.0,1,5},
    {"simplecolor", "heat", 2},
    {"perlin", "cloud", 0.02, 1, 5.0,0,20},
    {"simplecolor", "forest", 4, 1, 1.0,1,20},/*
    {"dualblobs", "white",   0.010, 2, 0.5, 0, 5},  // Default
    {"dualblobs", "ocean",   0.020, 1, 1.0, 0, 3},  // Fast, dense, long trails - PURE OCEAN COLORS
    {"dualblobs", "heat",    0.015, 1, 0.8, 0, 4},  // Quick sparkly fade
    {"dualblobs", "forest",  0.008, 3, 0.3, 0, 8},  // Slow, long, subtle
    
    // Random multi-hue (randomColors=1)
    {"dualblobs", "rainbow", 0.015, 1, 0.7, 1, 5},  // Sample random rainbow colors

*/
    });

    //seq.applyEdgePermutationSequence("octahedron","octacellperms",1,.3);

   // octaswirl(seq);

   // cubeswirl(seq);

    seq.addstep("tester", 10, FADE, 2);
    /*seq.addstep("allcycles", 10.0, FADE, 2);

    seq.addstep("sixpaths", 10.0, FADE, 2);
   
    seq.addstep("hypercubes", 10.0, FADE, 2);
    
    seq.addstep("twentyfourcell", 10, FADE, 2);
    seq.addstep("hypercubes", 10, FADE, 2);
*/


   // seq.addstep("test", "simpletest", 10.0, FADE, 2.0);   
    
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
