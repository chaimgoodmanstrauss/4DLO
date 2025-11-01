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
    
    // Get palette name based on function type
    if(funcIndex >= 8 && funcIndex <= 10) {  // Fire functions (8-10)
        Fire2012ColorFunction* fireFunc = static_cast<Fire2012ColorFunction*>(statefulColorFunctions[funcIndex]);
        return fireFunc->getPaletteName();
    } else if(funcIndex == 11) {  // Audio reactive
        AudioReactiveColorFunction* audioFunc = static_cast<AudioReactiveColorFunction*>(statefulColorFunctions[funcIndex]);
        return audioFunc->getPaletteName();
    } else if(funcIndex == 13) {  // Plasma
        PlasmaColorFunction* plasmaFunc = static_cast<PlasmaColorFunction*>(statefulColorFunctions[funcIndex]);
        return plasmaFunc->getPaletteName();
    } else if(funcIndex == 14) {  // Particles
        ParticleColorFunction* particleFunc = static_cast<ParticleColorFunction*>(statefulColorFunctions[funcIndex]);
        return particleFunc->getPaletteName();
    } else if(funcIndex == 17) {  // Frequency bands
        FrequencyBandVisualizer* freqFunc = static_cast<FrequencyBandVisualizer*>(statefulColorFunctions[funcIndex]);
        return freqFunc->getPaletteName();
    } else if(funcIndex == 20) {  // Beat detector
        BeatDetector* beatFunc = static_cast<BeatDetector*>(statefulColorFunctions[funcIndex]);
        return beatFunc->getPaletteName();
    } else if(funcIndex == 12) {  // VU Meter
        VUMeterColorFunction* vuFunc = static_cast<VUMeterColorFunction*>(statefulColorFunctions[funcIndex]);
        return vuFunc->getPaletteName();
    } else if(funcIndex == 18) {  // Bass Pulse
        BassPulseFunction* bassFunc = static_cast<BassPulseFunction*>(statefulColorFunctions[funcIndex]);
        return bassFunc->getPaletteName();
    } else if(funcIndex == 19) {  // Spectrum
        SpectrumAnalyzer* specFunc = static_cast<SpectrumAnalyzer*>(statefulColorFunctions[funcIndex]);
        return specFunc->getPaletteName();
    } else if(funcIndex == 21) {  // Vocals
        VocalHighlighter* vocalFunc = static_cast<VocalHighlighter*>(statefulColorFunctions[funcIndex]);
        return vocalFunc->getPaletteName();
    }
    
    // If we add more stateful functions with palettes, add them here
    return "";
}

/////////////////////////////////////////
// COLOR FUNCTION ARRAYS
//

// Define wrappers for stateful functions to fit ColorFunction signature
CRGB fire2012Wrapper(float position) {
    return callStatefulColorFunction(8, position);
}

CRGB fire2012BlueWrapper(float position) {
    return callStatefulColorFunction(9, position);
}

CRGB fire2012GreenWrapper(float position) {
    return callStatefulColorFunction(10, position);
}

CRGB audioReactiveWrapper(float position) {
    return callStatefulColorFunction(11, position);
}

CRGB vuMeterWrapper(float position) {
    return callStatefulColorFunction(12, position);
}

CRGB plasmaWrapper(float position) {
    return callStatefulColorFunction(13, position);
}

CRGB particlesWrapper(float position) {
    return callStatefulColorFunction(14, position);
}

CRGB freqBandsWrapper(float position) {
    return callStatefulColorFunction(17, position);
}

CRGB bassPulseWrapper(float position) {
    return callStatefulColorFunction(18, position);
}

CRGB spectrumWrapper(float position) {
    return callStatefulColorFunction(19, position);
}

CRGB beatDetectWrapper(float position) {
    return callStatefulColorFunction(20, position);
}

CRGB vocalsWrapper(float position) {
    return callStatefulColorFunction(21, position);
}

// Color function array definitions
ColorFunction colorFunctionArray[numcolorfunctions] = {
    constantlyDark,          // 0 - constantly dark
    rainbow,                 // 1
    bluetored,               // 2
    staticblue,              // 3
    pulsingblue,             // 4
    pulsingwhite,            // 5
    staticred,               // 6
    cylon,                   // 7
    fire2012Wrapper,         // 8 - fire effect (red/orange)
    fire2012BlueWrapper,     // 9 - fire effect (blue)
    fire2012GreenWrapper,    // 10 - fire effect (green)
    audioReactiveWrapper,    // 11 - audio reactive
    vuMeterWrapper,          // 12 - VU meter
    plasmaWrapper,           // 13 - plasma
    particlesWrapper,        // 14 - particles
    staticgreen,             // 15
    pulsingred,              // 16
    freqBandsWrapper,        // 17 - frequency band visualizer
    bassPulseWrapper,        // 18 - bass pulse
    spectrumWrapper,         // 19 - spectrum analyzer
    beatDetectWrapper,       // 20 - beat detector
    vocalsWrapper,           // 21 - vocal highlighter
    // Slots 22-99 available for future functions
};

String colorFunctionNames[numcolorfunctions] = {
    "dark",
    "rainbow",
    "bluetored", 
    "staticblue",
    "pulsingblue",
    "pulsingwhite",
    "staticred",
    "cylon",
    "fire2012",
    "fire2012_blue",
    "fire2012_green",
    "audio",
    "vumeter",
    "plasma",
    "particles",
    "staticgreen",
    "pulsingred",
    "freqbands",
    "basspulse",
    "spectrum",
    "beatdetect",
    "vocals",
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
    
    // Fire2012 Standard (index 8) - Classic fire
    Fire2012ColorFunction* fire2012Standard = new Fire2012ColorFunction(
        "fire2012",      // name
        55,              // cooling
        120,             // sparking
        false,           // reverse direction
        "fire"           // palette name from registry
    );
    registerStatefulColorFunction(8, fire2012Standard);
    
    // Fire2012 Blue (index 9) - Blue fire
    Fire2012ColorFunction* fire2012Blue = new Fire2012ColorFunction(
        "fire2012_blue", 
        45,              // cooler for blue effect
        100,             // less sparking
        false, 
        "bluefire"       // blue fire palette from registry
    );
    registerStatefulColorFunction(9, fire2012Blue);
    
    // Fire2012 Green (index 10) - Green fire
    Fire2012ColorFunction* fire2012Green = new Fire2012ColorFunction(
        "fire2012_green",
        50,
        110,
        false,
        "greenfire"      // green fire palette from registry
    );
    registerStatefulColorFunction(10, fire2012Green);
    
    // Audio Reactive (index 11)
    AudioReactiveColorFunction* audioReactive = new AudioReactiveColorFunction(
        "audio",         // name
        A0,              // audio pin
        100,             // sensitivity
        0.95,            // decay rate
        "rainbow",       // palette name from registry
        false            // use palette mode (not HSV mode)
    );
    registerStatefulColorFunction(11, audioReactive);
    
    // VU Meter (index 12) - now with palette support
    VUMeterColorFunction* vuMeter = new VUMeterColorFunction(
        "vumeter",       // name
        A0,              // audio pin (legacy, unused)
        100,             // sensitivity
        false,           // use palette mode (not classic)
        "rainbow"        // palette name from registry
    );
    registerStatefulColorFunction(12, vuMeter);
    
    // Plasma (index 13)
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
    registerStatefulColorFunction(13, plasma);
    
    // Particles (index 14)
    ParticleColorFunction* particles = new ParticleColorFunction(
        "particles",     // name
        -0.01,           // gravity
        3,               // emission rate
        "rainbow",       // palette name from registry
        true             // random colors for each particle
    );
    registerStatefulColorFunction(14, particles);
    
    // Frequency Band Visualizer (index 17)
    FrequencyBandVisualizer* freqBands = new FrequencyBandVisualizer(
        "freqbands",     // name
        "rainbow"        // palette name from registry
    );
    registerStatefulColorFunction(17, freqBands);
    
    // Bass Pulse (index 18) - now with palette support
    BassPulseFunction* bassPulse = new BassPulseFunction(
        "basspulse",     // name
        "fire"           // palette name from registry
    );
    registerStatefulColorFunction(18, bassPulse);
    
    // Spectrum Analyzer (index 19) - now with palette support
    SpectrumAnalyzer* spectrum = new SpectrumAnalyzer(
        "spectrum",      // name
        "rainbow"        // palette name from registry
    );
    registerStatefulColorFunction(19, spectrum);
    
    // Beat Detector (index 20)
    BeatDetector* beatDetect = new BeatDetector(
        "beatdetect",    // name
        "lava"           // palette name from registry
    );
    registerStatefulColorFunction(20, beatDetect);
    
    // Vocal Highlighter (index 21) - now with palette support
    VocalHighlighter* vocals = new VocalHighlighter(
        "vocals",        // name
        0.92,            // decay rate
        "ocean"          // palette name from registry
    );
    registerStatefulColorFunction(21, vocals);
    
    Serial.println("Stateful color functions initialized:");
    Serial.println("  - fire2012 (index 8) - Uses 'fire' palette");
    Serial.println("  - fire2012_blue (index 9) - Uses 'bluefire' palette");
    Serial.println("  - fire2012_green (index 10) - Uses 'greenfire' palette");
    Serial.println("  - audio (index 11) - Uses 'rainbow' palette");
    Serial.println("  - vumeter (index 12) - Classic VU meter colors");
    Serial.println("  - plasma (index 13) - Uses 'plasma' palette");
    Serial.println("  - particles (index 14) - Uses 'rainbow' palette");
    Serial.println("  - freqbands (index 17) - Frequency band visualizer");
    Serial.println("  - basspulse (index 18) - Bass-reactive pulse");
    Serial.println("  - spectrum (index 19) - Classic spectrum analyzer");
    Serial.println("  - beatdetect (index 20) - Beat detection flash");
    Serial.println("  - vocals (index 21) - Vocal frequency highlighter");
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
    
    // Set palette based on function index
    // Note: We use index-based checking instead of dynamic_cast because Arduino/Teensy
    // compiles with -fno-rtti (no Run-Time Type Information)
    bool success = false;
    
    if(funcIndex >= 8 && funcIndex <= 10) {  // Fire functions (8-10)
        Fire2012ColorFunction* fireFunc = static_cast<Fire2012ColorFunction*>(statefulColorFunctions[funcIndex]);
        fireFunc->setPaletteName(paletteName);
        success = true;
    } else if(funcIndex == 11) {  // Audio reactive
        AudioReactiveColorFunction* audioFunc = static_cast<AudioReactiveColorFunction*>(statefulColorFunctions[funcIndex]);
        audioFunc->setPaletteName(paletteName);
        success = true;
    } else if(funcIndex == 12) {  // VU Meter
        VUMeterColorFunction* vuFunc = static_cast<VUMeterColorFunction*>(statefulColorFunctions[funcIndex]);
        vuFunc->setPaletteName(paletteName);
        success = true;
    } else if(funcIndex == 13) {  // Plasma
        PlasmaColorFunction* plasmaFunc = static_cast<PlasmaColorFunction*>(statefulColorFunctions[funcIndex]);
        plasmaFunc->setPaletteName(paletteName);
        success = true;
    } else if(funcIndex == 14) {  // Particles
        ParticleColorFunction* particleFunc = static_cast<ParticleColorFunction*>(statefulColorFunctions[funcIndex]);
        particleFunc->setPaletteName(paletteName);
        success = true;
    } else if(funcIndex == 17) {  // Frequency bands
        FrequencyBandVisualizer* freqFunc = static_cast<FrequencyBandVisualizer*>(statefulColorFunctions[funcIndex]);
        freqFunc->setPaletteName(paletteName);
        success = true;
    } else if(funcIndex == 18) {  // Bass Pulse
        BassPulseFunction* bassFunc = static_cast<BassPulseFunction*>(statefulColorFunctions[funcIndex]);
        bassFunc->setPaletteName(paletteName);
        success = true;
    } else if(funcIndex == 19) {  // Spectrum
        SpectrumAnalyzer* specFunc = static_cast<SpectrumAnalyzer*>(statefulColorFunctions[funcIndex]);
        specFunc->setPaletteName(paletteName);
        success = true;
    } else if(funcIndex == 20) {  // Beat detector
        BeatDetector* beatFunc = static_cast<BeatDetector*>(statefulColorFunctions[funcIndex]);
        beatFunc->setPaletteName(paletteName);
        success = true;
    } else if(funcIndex == 21) {  // Vocals
        VocalHighlighter* vocalFunc = static_cast<VocalHighlighter*>(statefulColorFunctions[funcIndex]);
        vocalFunc->setPaletteName(paletteName);
        success = true;
    }
    
    if(success) {
        Serial.println("Switched " + functionName + " to palette: " + paletteName);
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
    switchPalette("fire2012", paletteName);
    switchPalette("fire2012_blue", paletteName);
    switchPalette("fire2012_green", paletteName);
    switchPalette("audio", paletteName);
    switchPalette("plasma", paletteName);
    switchPalette("particles", paletteName);
    switchPalette("freqbands", paletteName);
    switchPalette("beatdetect", paletteName);
    switchPalette("vumeter", paletteName);
    switchPalette("basspulse", paletteName);
    switchPalette("spectrum", paletteName);
    switchPalette("vocals", paletteName);
}

// Randomize all palettes
void randomizeAllPalettes() {
    Serial.println("Randomizing all palettes...");
    
    int numPalettes = PaletteRegistry::getCount();
    
    // Give each function a different random palette
    switchPalette("fire2012", PaletteRegistry::getNameByIndex(random(numPalettes)));
    switchPalette("fire2012_blue", PaletteRegistry::getNameByIndex(random(numPalettes)));
    switchPalette("fire2012_green", PaletteRegistry::getNameByIndex(random(numPalettes)));
    switchPalette("audio", PaletteRegistry::getNameByIndex(random(numPalettes)));
    switchPalette("plasma", PaletteRegistry::getNameByIndex(random(numPalettes)));
    switchPalette("particles", PaletteRegistry::getNameByIndex(random(numPalettes)));
    switchPalette("freqbands", PaletteRegistry::getNameByIndex(random(numPalettes)));
    switchPalette("beatdetect", PaletteRegistry::getNameByIndex(random(numPalettes)));
    switchPalette("vumeter", PaletteRegistry::getNameByIndex(random(numPalettes)));
    switchPalette("basspulse", PaletteRegistry::getNameByIndex(random(numPalettes)));
    switchPalette("spectrum", PaletteRegistry::getNameByIndex(random(numPalettes)));
    switchPalette("vocals", PaletteRegistry::getNameByIndex(random(numPalettes)));
}
