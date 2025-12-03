////////////////////////////////////
//
//   sequences.cpp
//

#include "namedpermutations.h"
#include "modelsequence.h"
#include "hdlo_models.h"

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


///////////////////////////////////////
// some scripts 

void octaswirl(SequenceBuilder& seq, String modelname = "octahedron", 
               float steplength = 1.0, float fadelength = 1) {
    for(int i = 0; i < 24; i++) {
        seq.addstep(modelname, octacellperms[i], steplength, FADE, fadelength);
    }
}

/*
void cycleswirl(SequenceBuilder& seq, String modelname = "octahedron", 
               float steplength = 1.0, float fadelength = 1) {
    for(int i = 0; i < 4; i++) {
        seq.addstep(modelname, octacellperms[i], steplength, FADE, fadelength);
    }
}*/

  void cubeswirl(SequenceBuilder& seq,  String modelname ="cube",float steplength=1,float fadelength=.3){
  for(int i: {23,23,0,1,2,3,4,5,6,11,
           //13,14,15,16,17,18,19,20,
           21,22,23}
       // int i = 0;i<24;i++
       ){
    seq.addstep(modelname, cubecellperms[i],steplength,FADE,fadelength);
  }}

  void setfftfirepalette(SequenceBuilder& seq){
    seq.setaudiopalette({
        "dark",{"fftfire", "rainbow", 30.0, 0,fftthreshold},
        {"fftfire", "party", 30.0, 0,fftthreshold},
        {"fftfire", "ocean", 30.0, 0,fftthreshold},
        {"fftfire", "heat", 30.0, 0,fftthreshold},
  });}

//void setrainbowpallets






void initializeSequences() {
    Serial.println("\n=== Initializing sequences ===");
    
    
    SequenceBuilder seq(&mainSequence);
    
    // Main sequence with dual palettes
    mainSequence.beginRegistry("Main Show", 84600.0, true);
    
    seq.setaudiotimeout(2.0); // I think this is currently blanked out and is a const, easily reset.

    seq.setaudiosource(AudioSourceConfig::MICROPHONE);
    
    seq.setaudiotimeout(10.0); // I think this is currently blanked out and is a const, easily reset.

    seq.setaudiopalette({
        // even unactivated edges (type 0) are colored when audio is present:
        {"fftfire", "rainbow", 30.0, 0,fftthreshold},
        //fftballs has parameters: threshhold, velocity, gravity,size, bounce decay
       // {"fftballs", "rainbow", fftthreshold,   .5,        .001,     .01,    0},
        {"fftballs", "rainbow", fftthreshold,   .5,        .001,     .01,    .00001},
        {"fftballs", "rainbow", fftthreshold,   .5,        .001,     .01,    0},
        {"fftspectrum","party",1},
        {"fftfire", "forest", 30.0, 0,fftthreshold}});
    
    
    seq.setbackgroundpalette({
     "dark", 
   // {"dualblobs", "rainbow", 0.015, 1, 0.7, 1, 5},
    {"simplecolor", "rainbow", .3},
    {"simplecolor", "rainbow", .3},
    {"simplecolor", "rainbow", .3},
    {"simplecolor", "ocean", .3},
    {"simplecolor", "forest", .3},
});

 //   cubeswirl(seq,"octahedron",2);

  /*
seq.setbackgroundpalette({
        "dark",
    {"simplecolor", "ocean", 2},
   {"simplecolor", "ocean", 2,.25},
   {"simplecolor", "ocean", 2,.5},
   {"simplecolor", "ocean", 2,.75}});

colormodel::applyEdgePermutation("cycles", "rot4",  "cycles_rot4_1");
colormodel::applyEdgePermutationSequence("cycles", {"rot4","rot4"}, "cycles_rot4_2");
colormodel::applyEdgePermutationSequence("cycles", {"rot4","rot4","rot4"}, "cycles_rot4_3");

seq.setbackgroundpalette({
        "dark",{"simplecolor", "ocean", 2}});
 
seq.addstep("cycles", 3, FADE, 1);

seq.setbackgroundpalette({
        "dark",{"simplecolor", "forest", 2}});

seq.addstep("cycles_rot4_1", 4, FADE, 1);

seq.setbackgroundpalette({
        "dark",{"simplecolor", "party", 2}});

seq.addstep("cycles_rot4_2", 4, FADE, 1);

seq.setbackgroundpalette({
        "dark",{"simplecolor", "heat", 2}});
seq.addstep("cycles_rot4_3", 4, FADE, 1);


seq.setbackgroundpalette({
        "dark",
    {"simplecolor", "ocean", 2},
   {"simplecolor", "ocean", 2,.25},
   {"simplecolor", "ocean", 2,.5},
   {"simplecolor", "ocean", 2,.75}});

   seq.setbackgroundpalette({
        "dark",
    {"simplecolor", "ocean", 2},
   {"simplecolor", "forest", 2,.25},
   {"simplecolor", "party", 2,.5},
   {"simplecolor", "heat", 2,.75}});

seq.addstep("allcycles", 20, FADE, 2);

seq.setbackgroundpalette({
        "dark",
    {"simplecolor", "ocean", 2},
   {"simplecolor", "ocean", 2,.25},
   {"simplecolor", "ocean", 2,.5},
   {"simplecolor", "ocean", 2,.75}});

   seq.addstep("allcycles", 40, FADE, 10);

seq.setaudiopalette({
  {"fftfire", "rainbow", 30.0, 0,fftthreshold},
{"fftfire", "rainbow", 30.0, 0,fftthreshold},
{"fftfire", "rainbow", 30.0, 0,fftthreshold},
 {"fftballs", "rainbow", 0.001, 0.1, 0.002, 0.01, 0.1},
{"fftfire", "rainbow", 30.0, 0,fftthreshold},
{"fftfire", "rainbow", 30.0, 0,fftthreshold},});

seq.setbackgroundpalette({
        "dark",
  {"simplecolor", "rainbow", .3},
   {"simplecolor", "rainbow", .3},
   {"simplecolor", "rainbow", .3},
   {"simplecolor", "rainbow", .3}});


cubeswirl(seq,"cube",1);


seq.setaudiopalette({"dark",
{"fftfire", "heat", 30.0, 0,fftthreshold},
{"fftfire", "ocean", 30.0, 0,fftthreshold},
{"fftfire", "party", 30.0, 0,fftthreshold},
{"fftfire", "rainbow", 30.0, 0,fftthreshold},
{"fftfire", "rainbow", 30.0, 0,fftthreshold},});

seq.setbackgroundpalette({
    "dark",
    {"perlin", "heat",  2,3},
    {"perlin", "ocean",   2,3},
    {"perlin", "forest", 2,3},
});


seq.addstep("hypercubes", 120, FADE, 10);

seq.setbackgroundpalette({
    "dark",
    {"simplecolor", "forest",.9}
});

seq.addstep("twentyfourcell", 60, FADE, 6);
   //octaswirl(seq,"octahedron",4,.3);

seq.setbackgroundpalette({
    "dark",{"perlin", "heat",10,30}
});
*/
   seq.addstep("twentyfourcell", 2000, FADE, 2);

 
    
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
