////////////////////////////////////////////
//
//   colorfunctions.cpp
//
// Implementation using PaletteRegistry for automatic palette management
//

#include "colorfunctions.h"
#include "paletteregistry.h"

/////////////////////////////////////////
// STATEFUL COLOR FUNCTION STATIC MEMBER
//

// Initialize static frame counter
unsigned long StatefulColorFunction::globalFrameNumber = 0;

/////////////////////////////////////////
// STATEFUL COLOR FUNCTION REGISTRY
//

StatefulColorFunction* statefulColorFunctions[numcolorfunctions] = {nullptr};
int numStatefulColorFunctions = 0;

void registerStatefulColorFunction(int index, StatefulColorFunction* func) {
    if (index >= 0 && index < numcolorfunctions) {
        if (statefulColorFunctions[index] != nullptr) {
            Serial.println("Warning: Overwriting stateful color function at index " + String(index));
            delete statefulColorFunctions[index];  // Clean up old function
        }
        statefulColorFunctions[index] = func;
        numStatefulColorFunctions++;
        Serial.println("Registered stateful color function: " + func->getName() + 
                      " at index " + String(index));
    }
}

// Function to get the current palette name for a given color function
String getCurrentPaletteName(String functionName) {
    int funcIndex = findColorFunctionByName(functionName);
    
    if(funcIndex < 0 || funcIndex >= numcolorfunctions) {
        return "";
    }
    
    if(statefulColorFunctions[funcIndex] == nullptr) {
        // Not a stateful function, no palette to get
        return "";
    }
    
    // Use virtual function - automatically works for all palette-supporting functions
    return statefulColorFunctions[funcIndex]->getPaletteName();
}

/////////////////////////////////////////
// COLOR FUNCTION ARRAYS
//

// Define wrappers for stateful functions to fit ColorFunction signature
CRGB cylonWrapper(float position) {
    return callStatefulColorFunction(2, position);
}

CRGB fire2012Wrapper(float position) {
    return callStatefulColorFunction(3, position);
}

CRGB audioCylonWrapper(float position) {
    return callStatefulColorFunction(4, position);
}

CRGB audioReactiveWrapper(float position) {
    return callStatefulColorFunction(5, position);
}

CRGB vuMeterWrapper(float position) {
    return callStatefulColorFunction(6, position);
}

CRGB plasmaWrapper(float position) {
    return callStatefulColorFunction(7, position);
}

CRGB particlesWrapper(float position) {
    return callStatefulColorFunction(8, position);
}

CRGB beatDetectWrapper(float position) {
    return callStatefulColorFunction(9, position);
}

CRGB vocalsWrapper(float position) {
    return callStatefulColorFunction(10, position);
}

CRGB simpleColorWrapper(float position) {
    return callStatefulColorFunction(12, position);
}

CRGB audioReactive2Wrapper(float position) {
    return callStatefulColorFunction(13, position);
}

CRGB fftFireWrapper(float position) {
    return callStatefulColorFunction(14, position);
}

CRGB noisePerlinWrapper(float position) {
    return callStatefulColorFunction(15, position);
}

// Color function array definitions - COMPACT INDEXING (no gaps)
ColorFunction colorFunctionArray[numcolorfunctions] = {
    constantlyDark,          // 0 - constantly dark
    rainbow,                 // 1 - rainbow gradient
    cylonWrapper,            // 2 - cylon scanning effect
    fire2012Wrapper,         // 3 - fire effect
    audioCylonWrapper,       // 4 - audio-reactive cylon
    audioReactiveWrapper,    // 5 - audio reactive
    vuMeterWrapper,          // 6 - VU meter
    plasmaWrapper,           // 7 - plasma
    particlesWrapper,        // 8 - particles
    beatDetectWrapper,       // 9 - beat detector
    vocalsWrapper,           // 10 - vocal highlighter
    breathingColor,          // 11 - breathing white
    simpleColorWrapper,      // 12 - simple color palette display
    audioReactive2Wrapper,   // 13 - audio reactive 2 (pitch-sensitive)
    fftFireWrapper,          // 14 - FFT fire (frequency-based fire effect)
    noisePerlinWrapper,      // 15 - Perlin noise + palette
    // Slots 16-99 available for future functions
};

String colorFunctionNames[numcolorfunctions] = {
    "dark",          // 0
    "rainbow",       // 1
    "cylon",         // 2
    "fire2012",      // 3
    "audiocylon",    // 4
    "audio",         // 5
    "vumeter",       // 6
    "plasma",        // 7
    "particles",     // 8
    "beatdetect",    // 9
    "vocals",        // 10
    "breathing",     // 11
    "simplecolor",   // 12
    "audio2",        // 13
    "fftfire",       // 14
    "noiseperlin",   // 15
    // Names for remaining slots will be empty strings
};

/////////////////////////////////////////
// HELPER FUNCTIONS
//

int findColorFunctionByName(String name) {
    for (int i = 0; i < numcolorfunctions; i++) {
        if (colorFunctionNames[i].equalsIgnoreCase(name)) {
            return i;
        }
    }
    return -1;  // Not found
}

CRGB getColorFromFunction(int index, float position) {
    if (index < 0 || index >= numcolorfunctions) {
        return CRGB(0, 0, 0);
    }
    
    // Check if this is a stateful function first
    if (statefulColorFunctions[index] != nullptr) {
        // Lazy update happens inside callStatefulColorFunction
        return callStatefulColorFunction(index, position);
    }
    
    // Otherwise use the regular function pointer
    if (colorFunctionArray[index] != nullptr) {
        return colorFunctionArray[index](position);
    }
    
    return CRGB(0, 0, 0);
}

void printActiveStatefulFunctions() {
    Serial.print("Active stateful functions this frame: ");
    bool anyActive = false;
    
    for (int i = 0; i < numcolorfunctions; i++) {
        if (statefulColorFunctions[i] != nullptr && 
            statefulColorFunctions[i]->wasUpdatedThisFrame()) {
            if (anyActive) Serial.print(", ");
            Serial.print(statefulColorFunctions[i]->getName());
            anyActive = true;
        }
    }
    
    if (!anyActive) {
        Serial.print("none");
    }
    
    Serial.println();
}

/////////////////////////////////////////
// AUTO-INITIALIZATION
//
// Using PaletteRegistry for automatic palette management
//

void initializeStatefulColorFunctions() {
    Serial.println("Initializing stateful color functions with PaletteRegistry...");
    
    // Make sure palettes are initialized
    PaletteRegistry::initialize();
    
    // Cylon (index 2) - Classic scanning effect
    CylonEffect* cylon = new CylonEffect(
        "cylon",         // name
        0.03,            // scan speed
        "fire"           // palette name from registry
    );
    registerStatefulColorFunction(2, cylon);
    
    // Fire2012 Standard (index 3) - Classic fire
    Fire2012ColorFunction* fire2012Standard = new Fire2012ColorFunction(
        "fire2012",      // name
        55,              // cooling
        120,             // sparking
        false,           // reverse direction
        "fire"           // palette name from registry
    );
    registerStatefulColorFunction(3, fire2012Standard);
    
    // Audio Cylon (index 4) - Audio-reactive scanning effect
    AudioCylonEffect* audioCylon = new AudioCylonEffect(
        "audiocylon",    // name
        0.02,            // base scan speed
        "fire"           // palette for tail
    );
    registerStatefulColorFunction(4, audioCylon);
    
    // Audio Reactive (index 5)
    AudioReactiveColorFunction* audioReactive = new AudioReactiveColorFunction(
        "audio",         // name
        A0,              // audio pin
        100,             // sensitivity
        0.95,            // decay rate
        "rainbow",       // palette name from registry
        false            // use palette mode (not HSV mode)
    );
    registerStatefulColorFunction(5, audioReactive);
    
    // VU Meter (index 6) - now with palette support
    VUMeterColorFunction* vuMeter = new VUMeterColorFunction(
        "vumeter",       // name
        A0,              // audio pin (legacy, unused)
        100,             // sensitivity
        false,           // use palette mode (not classic)
        "rainbow"        // palette name from registry
    );
    registerStatefulColorFunction(6, vuMeter);
    
    // Plasma (index 7)
    PlasmaColorFunction* plasma = new PlasmaColorFunction(
        "plasma",        // name
        0.02,            // speed1
        0.03,            // speed2
        0.01,            // speed3
        4.0,             // scale1
        3.0,             // scale2
        5.0,             // scale3
        "plasma"         // plasma palette from registry
    );
    registerStatefulColorFunction(7, plasma);
    
    // Particles (index 8)
    ParticleColorFunction* particles = new ParticleColorFunction(
        "particles",     // name
        -0.01,           // gravity
        3,               // emission rate
        "rainbow",       // palette name from registry
        true             // random colors for each particle
    );
    registerStatefulColorFunction(8, particles);
    
    // Beat Detector (index 9)
    BeatDetector* beatDetect = new BeatDetector(
        "beatdetect",    // name
        "lava"           // palette name from registry
    );
    registerStatefulColorFunction(9, beatDetect);
    
    // Vocal Highlighter (index 10) - now with palette support
    VocalHighlighter* vocals = new VocalHighlighter(
        "vocals",        // name
        0.92,            // decay rate
        "ocean"          // palette name from registry
    );
    registerStatefulColorFunction(10, vocals);
    
    // Simple Color Viewer (index 12) - Palette gradient display
    SimpleColorViewer* simpleViewer = new SimpleColorViewer(
        "simplecolor",   // name
        "rainbow"        // palette name from registry
    );
    registerStatefulColorFunction(12, simpleViewer);
    
    // Audio Reactive 2 (index 13) - Pitch-sensitive vocal version
    AudioReactiveColorFunction2* audioReactive2 = new AudioReactiveColorFunction2(
        "audio2",        // name
        A0,              // audio pin (unused)
        100,             // sensitivity
        0.95,            // decay rate
        "rainbow",       // palette name from registry
        false            // use palette mode (not HSV mode)
    );
    registerStatefulColorFunction(13, audioReactive2);
    
    // FFT Fire (index 14) - Frequency-based fire effect
    FFTFireColorFunction* fftFire = new FFTFireColorFunction(
        "fftfire",       // name
        15,              // cooling (lower = brighter idle, slower fade) [was 20]
        60,              // numFFTBins (from INO)
        AUDIO_FFT_THRESHOLD,  // fftThreshold (0.01)
        AUDIO_BRIGHTNESS_MULTIPLIER,  // brightnessScale (4000)
        AUDIO_MAX_BRIGHTNESS,  // maxBrightness (160)
        6,               // hueMultiplier (from INO)
        false,           // useHSVMode (false = palette mode, true = frequency→color)
        "fire"           // palette (for palette mode)
    );
    registerStatefulColorFunction(14, fftFire);
    
    // Noise Perlin (index 15) - Perlin noise + palette
    NoisePerlinColorFunction* noisePerlin = new NoisePerlinColorFunction(
        "noiseperlin",   // name
        20,              // speed
        30,              // scale
        1,               // colorLoop
        "rainbow"        // palette
    );
    registerStatefulColorFunction(15, noisePerlin);
    
    Serial.println("Stateful color functions initialized:");
    Serial.println("  - cylon (index 2) - Scanning eye effect");
    Serial.println("  - fire2012 (index 3) - Uses 'fire' palette");
    Serial.println("  - audiocylon (index 4) - Audio-reactive scanning (speed=volume, color=pitch)");
    Serial.println("  - audio (index 5) - Uses 'rainbow' palette");
    Serial.println("  - vumeter (index 6) - Classic VU meter colors");
    Serial.println("  - plasma (index 7) - Uses 'plasma' palette");
    Serial.println("  - particles (index 8) - Uses 'rainbow' palette");
    Serial.println("  - beatdetect (index 9) - Beat detection flash");
    Serial.println("  - vocals (index 10) - Vocal frequency highlighter");
    Serial.println("  - simplecolor (index 12) - Palette gradient viewer");
    Serial.println("  - audio2 (index 13) - Pitch-sensitive vocal (width=volume, color=pitch)");
    Serial.println("  - fftfire (index 14) - FFT fire (frequency→color, amplitude→brightness)");
    Serial.println("  - noiseperlin (index 15) - Perlin noise + palette");
    Serial.println("NOTE: All audio functions use centralized AudioSystem");
    Serial.println("NOTE: Functions only update when actually used (lazy evaluation)");
    
    // Print available palettes
    PaletteRegistry::printRegistry();
}

/////////////////////////////////////////
// DYNAMIC PALETTE SWITCHING
//
// Functions to change palettes at runtime
//

void switchPalette(String functionName, String paletteName) {
    int funcIndex = findColorFunctionByName(functionName);
    
    if(funcIndex < 0 || funcIndex >= numcolorfunctions) {
        Serial.println("Error: Function '" + functionName + "' not found");
        return;
    }
    
    if(statefulColorFunctions[funcIndex] == nullptr) {
        Serial.println("Error: Function '" + functionName + "' is not a stateful function");
        return;
    }
    
    // Check palette exists
    if(PaletteRegistry::findByName(paletteName) == nullptr) {
        Serial.println("Error: Palette '" + paletteName + "' not found in registry");
        PaletteRegistry::printRegistry();
        return;
    }
    
    // Use virtual function - automatically works for all palette-supporting functions
    statefulColorFunctions[funcIndex]->setPalette(paletteName);
    
    // Verify it worked by checking if the function supports palettes
    if(statefulColorFunctions[funcIndex]->getPaletteName() == paletteName) {
        // Serial.println("Switched " + functionName + " to palette: " + paletteName);
    } else {
        Serial.println("Warning: Function '" + functionName + "' does not support palettes");
    }
}

// Cycle all palettized functions to the next palette
void cycleAllPalettes() {
    static int paletteIndex = 0;
    
    paletteIndex = (paletteIndex + 1) % PaletteRegistry::getCount();
    String paletteName = PaletteRegistry::getNameByIndex(paletteIndex);
    
    Serial.println("Cycling all functions to palette: " + paletteName);
    
    // Update all palettized functions
    switchPalette("cylon", paletteName);
    switchPalette("fire2012", paletteName);
    switchPalette("audiocylon", paletteName);
    switchPalette("audio", paletteName);
    switchPalette("audio2", paletteName);
    switchPalette("plasma", paletteName);
    switchPalette("particles", paletteName);
    switchPalette("beatdetect", paletteName);
    switchPalette("vumeter", paletteName);
    switchPalette("vocals", paletteName);
    switchPalette("simplecolor", paletteName);
    switchPalette("fftfire", paletteName);
    switchPalette("noiseperlin", paletteName);
}

// Randomize all palettes
void randomizeAllPalettes() {
    Serial.println("Randomizing all palettes...");
    
    int numPalettes = PaletteRegistry::getCount();
    
    // Give each function a different random palette
    switchPalette("cylon", PaletteRegistry::getNameByIndex(random(numPalettes)));
    switchPalette("fire2012", PaletteRegistry::getNameByIndex(random(numPalettes)));
    switchPalette("audiocylon", PaletteRegistry::getNameByIndex(random(numPalettes)));
    switchPalette("audio", PaletteRegistry::getNameByIndex(random(numPalettes)));
    switchPalette("audio2", PaletteRegistry::getNameByIndex(random(numPalettes)));
    switchPalette("plasma", PaletteRegistry::getNameByIndex(random(numPalettes)));
    switchPalette("particles", PaletteRegistry::getNameByIndex(random(numPalettes)));
    switchPalette("beatdetect", PaletteRegistry::getNameByIndex(random(numPalettes)));
    switchPalette("vumeter", PaletteRegistry::getNameByIndex(random(numPalettes)));
    switchPalette("vocals", PaletteRegistry::getNameByIndex(random(numPalettes)));
    switchPalette("simplecolor", PaletteRegistry::getNameByIndex(random(numPalettes)));
    switchPalette("fftfire", PaletteRegistry::getNameByIndex(random(numPalettes)));
    switchPalette("noiseperlin", PaletteRegistry::getNameByIndex(random(numPalettes)));
}
