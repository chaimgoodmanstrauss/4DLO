////////////////////////////////////////////
//
// colorfunctions.cpp
//
// Color function registration with integer IDs
// Rainbow and Plasma functions removed
//
#include "colorfunctions.h"
#include "globalids.h"

// Initialize static member
unsigned long StatefulColorFunction::globalFrameNumber = 0;

/////////////////////////////////////////
// COLOR FUNCTION REGISTRATION
//

void registerAllColorFunctions() {
    auto& factory = ColorFunctionFactory::getInstance();
    
    // Simple functions
    factory.registerFunction(dark, []() {
        return new DarkColorFunction();
    });
    
    // Rainbow REMOVED
    
    factory.registerFunction(breathing, []() {
        return new BreathingColorFunction();
    });
    
    // Complex stateful functions
    factory.registerFunction(fire2012, []() {
        return new Fire2012ColorFunction();
    });
    
    factory.registerFunction(audio, []() {
        return new AudioReactiveColorFunction();
    });
    
    factory.registerFunction(audio2, []() {
        return new AudioReactiveColorFunction2();
    });
    
    factory.registerFunction(vumeter, []() {
        return new VUMeterColorFunction();
    });
    
    // Plasma REMOVED
    
    factory.registerFunction(particles, []() {
        return new ParticleColorFunction();
    });
    
    factory.registerFunction(cylon, []() {
        return new CylonEffect();
    });
    
    factory.registerFunction(audiocylon, []() {
        return new AudioCylonEffect();
    });
    
    factory.registerFunction(beatdetect, []() {
        return new BeatDetector();
    });
    
    factory.registerFunction(vocals, []() {
        return new VocalHighlighter();
    });
    
    factory.registerFunction(fftfire, []() {
        return new FFTFireColorFunction();
    });
    
    factory.registerFunction(fftspectrum, []() {
        return new FFTSpectrumColorFunction();
    });
    
    factory.registerFunction(perlin, []() {
        return new PerlinColorFunction();
    });
    
    factory.registerFunction(simplecolor, []() {
        return new SimpleColorViewer();
    });
    
    factory.registerFunction(dualblobs, []() {
        return new DualBlobsColorFunction();
    });
    
    factory.registerFunction(fftballs, []() {
        return new FFTBouncingBallsColorFunction();
    });
    
    Serial.println("Registered 17 color functions (rainbow/plasma removed)");
}
