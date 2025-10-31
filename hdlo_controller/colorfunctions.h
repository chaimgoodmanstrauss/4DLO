////////////////////////////////////////////
//
//
//   colorfunctions.h
//
// a colorfunction has one responsibility: return a CRGB at a given position
// It is free to make additional use of time or other data, but that's its business.
//
// ARCHITECTURE:
// - Simple stateless functions: Defined as inline functions
// - Complex stateful functions: Defined as classes inheriting from StatefulColorFunction
// - OPTIMIZATION: Stateful functions only update when actually used in a frame
//
#ifndef COLORFUNCTIONS_H
#define COLORFUNCTIONS_H

#include <FastLED.h>
#include <Arduino.h>
#include "models.h"

const int numcolorfunctions = 100;  // Maximum number of color functions
const int MAXBRIGHTNESS = 160;

/////////////////////////////////////////
// STATEFUL COLOR FUNCTION BASE CLASS
//
// For color functions that need to maintain state and update over time
// (e.g., fire effects, particle systems, cellular automata)
//
// LAZY UPDATE SYSTEM:
// - Functions only update when first accessed in a frame
// - Saves CPU by not updating unused effects
//

class StatefulColorFunction {
protected:
    String name;
    unsigned long lastUpdateTime;
    unsigned int updateInterval;  // Milliseconds between updates
    unsigned long currentFrameNumber;  // Track which frame we last updated
    static unsigned long globalFrameNumber;  // Increments each loop iteration
    
public:
    StatefulColorFunction(String functionName, unsigned int updateIntervalMs = 20)
        : name(functionName), lastUpdateTime(0), updateInterval(updateIntervalMs),
          currentFrameNumber(0) {}
    
    virtual ~StatefulColorFunction() {}
    
    // Must be implemented by derived classes
    virtual CRGB getColor(float position) = 0;
    
    // Update internal state - called automatically when first accessed in a frame
    void updateIfNeeded(unsigned long currentTime) {
        // Check if we've already updated this frame
        if (currentFrameNumber == globalFrameNumber) {
            return;  // Already updated this frame
        }
        
        // Check if enough time has passed
        if (currentTime - lastUpdateTime >= updateInterval) {
            updateState();
            lastUpdateTime = currentTime;
        }
        
        // Mark this frame as updated
        currentFrameNumber = globalFrameNumber;
    }
    
    // Override this to implement state updates
    virtual void updateState() = 0;
    
    // Reset to initial state
    virtual void reset() = 0;
    
    String getName() const { return name; }
    
    // Call at the start of each loop iteration
    static void beginFrame() {
        globalFrameNumber++;
    }
    
    // For debugging: check if function updated this frame
    bool wasUpdatedThisFrame() const {
        return currentFrameNumber == globalFrameNumber;
    }
};

/////////////////////////////////////////
// FIRE2012 COLOR FUNCTION
//
// Implements the Fire2012 effect from FastLED as an edge color function
//

class Fire2012ColorFunction : public StatefulColorFunction {
private:
    static const int NUM_LEDS = 128;  // Virtual LED array size
    byte heat[NUM_LEDS];
    
    // Fire2012 parameters
    int cooling;
    int sparking;
    bool gReverseDirection;
    CRGBPalette16 palette;
    
    // Fire2012 algorithm adapted from FastLED examples
    void updateFire() {
        // Step 1: Cool down every cell a little
        for(int i = 0; i < NUM_LEDS; i++) {
            int cooldown = random8(0, ((cooling * 10) / NUM_LEDS) + 2);
            if(cooldown > heat[i]) {
                heat[i] = 0;
            } else {
                heat[i] = heat[i] - cooldown;
            }
        }
        
        // Step 2: Heat from each cell drifts 'up' and diffuses a little
        for(int k = NUM_LEDS - 1; k >= 2; k--) {
            heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
        }
        
        // Step 3: Randomly ignite new 'sparks' near the bottom
        if(random8() < sparking) {
            int y = random8(7);
            heat[y] = qadd8(heat[y], random8(160, 255));
        }
    }
    
public:
    Fire2012ColorFunction(String functionName = "fire2012", 
                          int coolingValue = 55,
                          int sparkingValue = 120,
                          bool reverseDirection = false,
                          CRGBPalette16 firePalette = HeatColors_p)
        : StatefulColorFunction(functionName, 20),  // Update every 20ms
          cooling(coolingValue),
          sparking(sparkingValue),
          gReverseDirection(reverseDirection),
          palette(firePalette) {
        reset();
    }
    
    void reset() override {
        // Initialize heat array to zero
        for(int i = 0; i < NUM_LEDS; i++) {
            heat[i] = 0;
        }
        lastUpdateTime = 0;
    }
    
    void updateState() override {
        updateFire();
    }
    
    CRGB getColor(float position) override {
        // Map position (0-1) to LED index
        int ledIndex = (int)(position * (NUM_LEDS - 1));
        ledIndex = constrain(ledIndex, 0, NUM_LEDS - 1);
        
        // Reverse direction if needed
        if(gReverseDirection) {
            ledIndex = (NUM_LEDS - 1) - ledIndex;
        }
        
        // Map heat to color using palette
        byte temperature = heat[ledIndex];
        CRGB color = ColorFromPalette(palette, temperature);
        
        return color;
    }
    
    // Setters for runtime adjustment
    void setCooling(int value) { cooling = constrain(value, 0, 255); }
    void setSparking(int value) { sparking = constrain(value, 0, 255); }
    void setDirection(bool reverse) { gReverseDirection = reverse; }
    void setPalette(CRGBPalette16 newPalette) { palette = newPalette; }
};

/////////////////////////////////////////
// AUDIO REACTIVE COLOR FUNCTION
//
// Responds to audio input with dynamic colors
// Requires audio input on analog pin
//

class AudioReactiveColorFunction : public StatefulColorFunction {
private:
    static const int NUM_LEDS = 128;
    static const int HISTORY_SIZE = 8;
    
    byte brightness[NUM_LEDS];
    int audioPin;
    int sensitivity;
    byte audioHistory[HISTORY_SIZE];
    int historyIndex;
    byte currentLevel;
    float decay;
    
    // Simple peak detection
    byte readAudioLevel() {
        int sample = analogRead(audioPin);
        
        // Map 10-bit ADC (0-1023) to 0-255
        // Center around 512 (silence) and take absolute value
        int centered = abs(sample - 512);
        byte level = map(centered, 0, 512, 0, 255);
        
        // Apply sensitivity
        level = constrain(level * sensitivity / 100, 0, 255);
        
        return level;
    }
    
    byte getSmoothedLevel() {
        // Average recent history for smoother response
        int sum = 0;
        for(int i = 0; i < HISTORY_SIZE; i++) {
            sum += audioHistory[i];
        }
        return sum / HISTORY_SIZE;
    }
    
public:
    AudioReactiveColorFunction(String functionName = "audio", 
                               int pin = A0,
                               int sens = 100,
                               float decayRate = 0.95)
        : StatefulColorFunction(functionName, 10),  // Update every 10ms for responsiveness
          audioPin(pin),
          sensitivity(sens),
          historyIndex(0),
          currentLevel(0),
          decay(decayRate) {
        reset();
    }
    
    void reset() override {
        for(int i = 0; i < NUM_LEDS; i++) {
            brightness[i] = 0;
        }
        for(int i = 0; i < HISTORY_SIZE; i++) {
            audioHistory[i] = 0;
        }
        historyIndex = 0;
        currentLevel = 0;
    }
    
    void updateState() override {
        // Read audio level
        byte newLevel = readAudioLevel();
        
        // Update history
        audioHistory[historyIndex] = newLevel;
        historyIndex = (historyIndex + 1) % HISTORY_SIZE;
        
        // Get smoothed level
        currentLevel = getSmoothedLevel();
        
        // Update LED array - create expanding wave from center
        int center = NUM_LEDS / 2;
        int spread = (currentLevel * NUM_LEDS) / (255 * 2);
        
        // Decay all LEDs
        for(int i = 0; i < NUM_LEDS; i++) {
            brightness[i] = brightness[i] * decay;
        }
        
        // Add new peak
        for(int i = 0; i < spread; i++) {
            int pos1 = center + i;
            int pos2 = center - i;
            
            if(pos1 < NUM_LEDS) {
                brightness[pos1] = max(brightness[pos1], currentLevel);
            }
            if(pos2 >= 0) {
                brightness[pos2] = max(brightness[pos2], currentLevel);
            }
        }
    }
    
    CRGB getColor(float position) override {
        int index = (int)(position * (NUM_LEDS - 1));
        index = constrain(index, 0, NUM_LEDS - 1);
        
        byte value = brightness[index];
        
        // Map to color - low frequencies = red, high = blue
        byte hue = map(value, 0, 255, 0, 160);  // Red to blue spectrum
        
        return CHSV(hue, 255, value);
    }
    
    // Setters for runtime adjustment
    void setSensitivity(int sens) { sensitivity = constrain(sens, 10, 500); }
    void setDecay(float d) { decay = constrain(d, 0.5, 0.99); }
    void setAudioPin(int pin) { audioPin = pin; }
    
    // Getter for current audio level (for debugging)
    byte getCurrentLevel() const { return currentLevel; }
};

/////////////////////////////////////////
// VU METER COLOR FUNCTION
//
// Classic VU meter style audio visualization
//

class VUMeterColorFunction : public StatefulColorFunction {
private:
    static const int NUM_LEDS = 128;
    int audioPin;
    int sensitivity;
    byte peakLevel;
    byte currentLevel;
    byte peakHold;
    int peakHoldTime;
    unsigned long lastPeakTime;
    
public:
    VUMeterColorFunction(String functionName = "vumeter",
                        int pin = A0,
                        int sens = 100)
        : StatefulColorFunction(functionName, 10),
          audioPin(pin),
          sensitivity(sens),
          peakLevel(0),
          currentLevel(0),
          peakHold(0),
          peakHoldTime(500),
          lastPeakTime(0) {
        reset();
    }
    
    void reset() override {
        peakLevel = 0;
        currentLevel = 0;
        peakHold = 0;
        lastPeakTime = 0;
    }
    
    void updateState() override {
        // Read audio level
        int sample = analogRead(audioPin);
        int centered = abs(sample - 512);
        byte level = map(centered, 0, 512, 0, 255);
        level = constrain(level * sensitivity / 100, 0, 255);
        
        currentLevel = level;
        
        // Update peak
        if(level > peakHold) {
            peakHold = level;
            lastPeakTime = millis();
        }
        
        // Peak hold decay
        if(millis() - lastPeakTime > peakHoldTime) {
            peakHold = max(0, peakHold - 2);
        }
    }
    
    CRGB getColor(float position) override {
        // Calculate which "LED" we're at
        byte ledLevel = position * 255;
        
        // Color based on position: green (low) -> yellow (mid) -> red (high)
        CRGB color = CRGB::Black;
        
        if(ledLevel <= currentLevel) {
            // Active portion of meter
            if(position < 0.5) {
                color = CRGB::Green;  // Low levels = green
            } else if(position < 0.75) {
                color = CRGB::Yellow;  // Mid levels = yellow
            } else {
                color = CRGB::Red;  // High levels = red
            }
        } else if(abs(ledLevel - peakHold) < 5) {
            // Peak indicator
            color = CRGB::White;
        }
        
        return color;
    }
    
    void setSensitivity(int sens) { sensitivity = constrain(sens, 10, 500); }
    void setPeakHoldTime(int ms) { peakHoldTime = ms; }
    byte getCurrentLevel() const { return currentLevel; }
};

/////////////////////////////////////////
// COLOR FUNCTION REGISTRY
//
// Manages both stateless and stateful color functions
//

// Color function array (stateless functions as simple function pointers)
extern ColorFunction colorFunctionArray[numcolorfunctions];
extern String colorFunctionNames[numcolorfunctions];

// Stateful color function registry
extern StatefulColorFunction* statefulColorFunctions[numcolorfunctions];
extern int numStatefulColorFunctions;

// Function to register a stateful color function
void registerStatefulColorFunction(int index, StatefulColorFunction* func);

/////////////////////////////////////////
// SIMPLE STATELESS COLOR FUNCTIONS
//

inline CRGB rainbow(float position) {
    return CHSV(((int)(position * 255)), 255, MAXBRIGHTNESS);
}

inline CRGB bluetored(float position) {
    return CRGB(position * MAXBRIGHTNESS, 0, (1 - position) * MAXBRIGHTNESS);
}

inline CRGB cylon(float position) {
    return CHSV((millis() % 1000) / 1000.0 * 255, 255, 
                MAXBRIGHTNESS * max(0.0, cos(3.141 * position)));
}

inline CRGB staticblue(float position) {
    return CRGB(0, 0, MAXBRIGHTNESS);
}

inline CRGB staticred(float position) {
    return CRGB(0, MAXBRIGHTNESS, 0);
}

inline CRGB staticgreen(float position) {
    return CRGB(MAXBRIGHTNESS, 0, 0);
}

inline CRGB pulsingblue(float position) {
    return CRGB(0, 0, (sin(millis() / 1000.) + 1) / 2 * MAXBRIGHTNESS);
}

inline CRGB pulsingwhite(float position) {
    float val = ((sin(position * 6.28) + 1) * (sin(millis() / 1000.) + 1) / 5 + .2) * MAXBRIGHTNESS;
    return CRGB(val, val, .6 * val);
}

inline CRGB pulsingred(float position) {
    return CRGB(0, (sin(millis() / 1000.) + 1) / 2 * MAXBRIGHTNESS, 0);
}

inline CRGB constantlyDark(float position) {
    return CRGB(0, 0, 0);
}

// Wrapper for stateful color functions to be used as ColorFunction
// This allows stateful functions to be called through the standard interface
// OPTIMIZATION: Only updates when first accessed in a frame
inline CRGB callStatefulColorFunction(int index, float position) {
    if (index >= 0 && index < numcolorfunctions && 
        statefulColorFunctions[index] != nullptr) {
        // Lazy update: only update if needed
        statefulColorFunctions[index]->updateIfNeeded(millis());
        return statefulColorFunctions[index]->getColor(position);
    }
    return CRGB(0, 0, 0);
}

/////////////////////////////////////////
// HELPER FUNCTIONS FOR COLOR FUNCTION MANAGEMENT
//

// Find a color function by name
int findColorFunctionByName(String name);

// Get color from either stateless or stateful function
CRGB getColorFromFunction(int index, float position);

// Print which stateful functions were updated this frame (for debugging)
void printActiveStatefulFunctions();

#endif  // COLORFUNCTIONS_H
