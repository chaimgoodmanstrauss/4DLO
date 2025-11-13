////////////////////////////////////////////
//
// colorfunctions.cpp
//
// Implementation file for color functions
// Static member initialization only
//
#include "colorfunctions.h"

// Initialize static member
unsigned long StatefulColorFunction::globalFrameNumber = 0;

/////////////////////////////////////////
// COLOR FUNCTION REGISTRATION
//

void registerAllColorFunctions() {
    auto& factory = ColorFunctionFactory::getInstance();
    
    // Simple functions
    factory.registerFunction("dark", []() {
        return new DarkColorFunction();
    });
    
    factory.registerFunction("rainbow", []() {
        return new RainbowColorFunction();
    });
    
    factory.registerFunction("breathing", []() {
        return new BreathingColorFunction();
    });
    
    // Complex stateful functions
    factory.registerFunction("fire2012", []() {
        return new Fire2012ColorFunction();
    });
    
    factory.registerFunction("audio", []() {
        return new AudioReactiveColorFunction();
    });
    
    factory.registerFunction("audio2", []() {
        return new AudioReactiveColorFunction2();
    });
    
    factory.registerFunction("vumeter", []() {
        return new VUMeterColorFunction();
    });
    
    factory.registerFunction("plasma", []() {
        return new PlasmaColorFunction();
    });
    
    factory.registerFunction("particles", []() {
        return new ParticleColorFunction();
    });
    
    factory.registerFunction("cylon", []() {
        return new CylonEffect();
    });
    
    factory.registerFunction("audiocylon", []() {
        return new AudioCylonEffect();
    });
    
    factory.registerFunction("beatdetect", []() {
        return new BeatDetector();
    });
    
    factory.registerFunction("vocals", []() {
        return new VocalHighlighter();
    });
    
    factory.registerFunction("fftfire", []() {
        return new FFTFireColorFunction();
    });
    
    factory.registerFunction("perlin", []() {
        return new PerlinColorFunction();
    });
    
    factory.registerFunction("simplecolor", []() {
        return new SimpleColorViewer();
    });
    
    Serial.println("Registered 16 color functions");
}
