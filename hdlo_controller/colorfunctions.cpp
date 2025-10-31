////////////////////////////////////////////
//
//
//   colorfunctions.cpp
//
// Implementation of color function arrays and stateful color function management
// OPTIMIZED: Only updates stateful functions that are actually used each frame
//

#include "colorfunctions.h"

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
        }
        statefulColorFunctions[index] = func;
        numStatefulColorFunctions++;
        Serial.println("Registered stateful color function: " + func->getName() + 
                      " at index " + String(index));
    }
}

/////////////////////////////////////////
// COLOR FUNCTION ARRAYS
//

// Define wrappers for stateful functions to fit ColorFunction signature
CRGB fire2012Wrapper(float position) {
    return callStatefulColorFunction(8, position);  // Index 8 for fire2012
}

CRGB fire2012BlueWrapper(float position) {
    return callStatefulColorFunction(9, position);  // Index 9 for fire2012_blue
}

CRGB fire2012GreenWrapper(float position) {
    return callStatefulColorFunction(10, position);  // Index 10 for fire2012_green
}

CRGB audioReactiveWrapper(float position) {
    return callStatefulColorFunction(11, position);  // Index 11 for audio reactive
}

CRGB vuMeterWrapper(float position) {
    return callStatefulColorFunction(12, position);  // Index 12 for VU meter
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
    staticgreen,             // 13
    pulsingred,              // 14
    // Slots 15-99 available for future functions
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
    "staticgreen",
    "pulsingred",
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
