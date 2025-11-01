////////////////////////////////////////////
//
//   colorfunctions.h
//
// Color function system with automatic palette management
// Uses PaletteRegistry for centralized palette handling
//
#ifndef COLORFUNCTIONS_H
#define COLORFUNCTIONS_H

#include <FastLED.h>
#include <Arduino.h>
#include "models.h"
#include "paletteregistry.h"

const int numcolorfunctions = 100;  // Maximum number of color functions
const int MAXBRIGHTNESS = 160;

/////////////////////////////////////////
// STATEFUL COLOR FUNCTION BASE CLASS
//

class StatefulColorFunction {
protected:
    String name;
    unsigned long lastUpdateTime;
    unsigned int updateInterval;
    unsigned long currentFrameNumber;
    static unsigned long globalFrameNumber;
    
public:
    StatefulColorFunction(String functionName, unsigned int updateIntervalMs = 20)
        : name(functionName), lastUpdateTime(0), updateInterval(updateIntervalMs),
          currentFrameNumber(0) {}
    
    virtual ~StatefulColorFunction() {}
    
    virtual CRGB getColor(float position) = 0;
    
    void updateIfNeeded(unsigned long currentTime) {
        if (currentFrameNumber == globalFrameNumber) {
            return;
        }
        
        if (currentTime - lastUpdateTime >= updateInterval) {
            updateState();
            lastUpdateTime = currentTime;
        }
        
        currentFrameNumber = globalFrameNumber;
    }
    
    virtual void updateState() = 0;
    virtual void reset() = 0;
    
    String getName() const { return name; }
    
    static void beginFrame() {
        globalFrameNumber++;
    }
    
    bool wasUpdatedThisFrame() const {
        return currentFrameNumber == globalFrameNumber;
    }
};

/////////////////////////////////////////
// FIRE2012 COLOR FUNCTION
//

class Fire2012ColorFunction : public StatefulColorFunction {
private:
    static const int NUM_LEDS = 128;
    byte heat[NUM_LEDS];
    int cooling;
    int sparking;
    bool gReverseDirection;
    String paletteName;
    
    void updateFire() {
        for(int i = 0; i < NUM_LEDS; i++) {
            int cooldown = random8(0, ((cooling * 10) / NUM_LEDS) + 2);
            if(cooldown > heat[i]) {
                heat[i] = 0;
            } else {
                heat[i] = heat[i] - cooldown;
            }
        }
        
        for(int k = NUM_LEDS - 1; k >= 2; k--) {
            heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
        }
        
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
                          String palette = "fire")
        : StatefulColorFunction(functionName, 20),
          cooling(coolingValue),
          sparking(sparkingValue),
          gReverseDirection(reverseDirection),
          paletteName(palette) {
        reset();
    }
    
    void reset() override {
        for(int i = 0; i < NUM_LEDS; i++) {
            heat[i] = 0;
        }
        lastUpdateTime = 0;
    }
    
    void updateState() override {
        updateFire();
    }
    
    CRGB getColor(float position) override {
        int ledIndex = (int)(position * (NUM_LEDS - 1));
        ledIndex = constrain(ledIndex, 0, NUM_LEDS - 1);
        
        if(gReverseDirection) {
            ledIndex = (NUM_LEDS - 1) - ledIndex;
        }
        
        byte temperature = heat[ledIndex];
        
        // Get palette from registry
        CRGBPalette16* palette = PaletteRegistry::findByName(paletteName);
        if(palette == nullptr) {
            palette = PaletteRegistry::findByName("fire"); // Fallback
        }
        
        return ColorFromPalette(*palette, temperature);
    }
    
    void setCooling(int value) { cooling = constrain(value, 0, 255); }
    void setSparking(int value) { sparking = constrain(value, 0, 255); }
    void setDirection(bool reverse) { gReverseDirection = reverse; }
    void setPaletteName(String name) { paletteName = name; }
};

/////////////////////////////////////////
// AUDIO REACTIVE COLOR FUNCTION
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
    String paletteName;
    bool useHSV;
    
    byte readAudioLevel() {
        int sample = analogRead(audioPin);
        int centered = abs(sample - 512);
        byte level = map(centered, 0, 512, 0, 255);
        level = constrain(level * sensitivity / 100, 0, 255);
        return level;
    }
    
    byte getSmoothedLevel() {
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
                               float decayRate = 0.95,
                               String palette = "rainbow",
                               bool hsvMode = false)
        : StatefulColorFunction(functionName, 10),
          audioPin(pin),
          sensitivity(sens),
          historyIndex(0),
          currentLevel(0),
          decay(decayRate),
          paletteName(palette),
          useHSV(hsvMode) {
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
        byte newLevel = readAudioLevel();
        
        audioHistory[historyIndex] = newLevel;
        historyIndex = (historyIndex + 1) % HISTORY_SIZE;
        
        currentLevel = getSmoothedLevel();
        
        int center = NUM_LEDS / 2;
        int spread = (currentLevel * NUM_LEDS) / (255 * 2);
        
        for(int i = 0; i < NUM_LEDS; i++) {
            brightness[i] = brightness[i] * decay;
        }
        
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
        
        if(useHSV) {
            byte hue = map(value, 0, 255, 0, 160);
            return CHSV(hue, 255, value);
        } else {
            CRGBPalette16* palette = PaletteRegistry::findByName(paletteName);
            if(palette == nullptr) {
                palette = PaletteRegistry::findByName("rainbow");
            }
            
            CRGB color = ColorFromPalette(*palette, value);
            color.nscale8(value);
            
            return color;
        }
    }
    
    void setSensitivity(int sens) { sensitivity = constrain(sens, 10, 500); }
    void setDecay(float d) { decay = constrain(d, 0.5, 0.99); }
    void setAudioPin(int pin) { audioPin = pin; }
    void setPaletteName(String name) { paletteName = name; }
    void setHSVMode(bool mode) { useHSV = mode; }
    byte getCurrentLevel() const { return currentLevel; }
};

/////////////////////////////////////////
// VU METER COLOR FUNCTION
//

class VUMeterColorFunction : public StatefulColorFunction {
private:
    static const int NUM_LEDS = 128;
    int audioPin;
    int sensitivity;
    byte peakLevel;
    byte currentLevel;
    byte peakHold;
    unsigned long peakHoldTime;
    unsigned long lastPeakTime;
    bool useClassicColors;
    CRGB peakColor;
    
public:
    VUMeterColorFunction(String functionName = "vumeter",
                        int pin = A0,
                        int sens = 100,
                        bool classic = true,
                        CRGB peakCol = CRGB::White)
        : StatefulColorFunction(functionName, 10),
          audioPin(pin),
          sensitivity(sens),
          peakLevel(0),
          currentLevel(0),
          peakHold(0),
          peakHoldTime(500),
          lastPeakTime(0),
          useClassicColors(classic),
          peakColor(peakCol) {
        reset();
    }
    
    void reset() override {
        peakLevel = 0;
        currentLevel = 0;
        peakHold = 0;
        lastPeakTime = 0;
    }
    
    void updateState() override {
        int sample = analogRead(audioPin);
        int centered = abs(sample - 512);
        byte level = map(centered, 0, 512, 0, 255);
        level = constrain(level * sensitivity / 100, 0, 255);
        
        currentLevel = level;
        
        if(level > peakHold) {
            peakHold = level;
            lastPeakTime = millis();
        }
        
        if(millis() - lastPeakTime > peakHoldTime) {
            peakHold = max(0, peakHold - 2);
        }
    }
    
    CRGB getColor(float position) override {
        byte ledLevel = position * 255;
        CRGB color = CRGB::Black;
        
        if(ledLevel <= currentLevel) {
            if(useClassicColors) {
                if(position < 0.5) {
                    color = CRGB::Green;
                } else if(position < 0.75) {
                    color = CRGB::Yellow;
                } else {
                    color = CRGB::Red;
                }
            }
        } else if(abs(ledLevel - peakHold) < 5) {
            color = peakColor;
        }
        
        return color;
    }
    
    void setSensitivity(int sens) { sensitivity = constrain(sens, 10, 500); }
    void setPeakHoldTime(unsigned long ms) { peakHoldTime = ms; }
    void setClassicMode(bool classic) { useClassicColors = classic; }
    void setPeakColor(CRGB color) { peakColor = color; }
    byte getCurrentLevel() const { return currentLevel; }
};

/////////////////////////////////////////
// PLASMA COLOR FUNCTION
//

class PlasmaColorFunction : public StatefulColorFunction {
private:
    static const int NUM_LEDS = 128;
    float phase1, phase2, phase3;
    float speed1, speed2, speed3;
    float scale1, scale2, scale3;
    String paletteName;
    
public:
    PlasmaColorFunction(String functionName = "plasma",
                       float spd1 = 0.02, float spd2 = 0.03, float spd3 = 0.01,
                       float scl1 = 4.0, float scl2 = 3.0, float scl3 = 5.0,
                       String palette = "plasma")
        : StatefulColorFunction(functionName, 20),
          phase1(0), phase2(0), phase3(0),
          speed1(spd1), speed2(spd2), speed3(spd3),
          scale1(scl1), scale2(scl2), scale3(scl3),
          paletteName(palette) {
        reset();
    }
    
    void reset() override {
        phase1 = phase2 = phase3 = 0;
    }
    
    void updateState() override {
        phase1 += speed1;
        phase2 += speed2;
        phase3 += speed3;
    }
    
    CRGB getColor(float position) override {
        float value = sin(position * scale1 + phase1);
        value += sin(position * scale2 + phase2);
        value += sin(position * scale3 + phase3);
        
        value = (value + 3.0) * 255.0 / 6.0;
        byte index = constrain(value, 0, 255);
        
        CRGBPalette16* palette = PaletteRegistry::findByName(paletteName);
        if(palette == nullptr) {
            palette = PaletteRegistry::findByName("plasma");
        }
        
        return ColorFromPalette(*palette, index);
    }
    
    void setPaletteName(String name) { paletteName = name; }
    void setSpeeds(float s1, float s2, float s3) { 
        speed1 = s1; speed2 = s2; speed3 = s3; 
    }
    void setScales(float s1, float s2, float s3) { 
        scale1 = s1; scale2 = s2; scale3 = s3; 
    }
};

/////////////////////////////////////////
// PARTICLE SYSTEM COLOR FUNCTION
//

class ParticleColorFunction : public StatefulColorFunction {
private:
    static const int MAX_PARTICLES = 20;
    static const int NUM_LEDS = 128;
    
    struct Particle {
        float position;
        float velocity;
        byte life;
        byte hue;
        
        Particle() : position(0), velocity(0), life(0), hue(0) {}
    };
    
    Particle particles[MAX_PARTICLES];
    float gravity;
    int emissionRate;
    int emissionCounter;
    String paletteName;
    bool randomColors;
    
    void emitParticle() {
        for(int i = 0; i < MAX_PARTICLES; i++) {
            if(particles[i].life == 0) {
                particles[i].position = 0;
                particles[i].velocity = random8(20, 80) / 100.0;
                particles[i].life = 255;
                particles[i].hue = randomColors ? random8() : (i * 255 / MAX_PARTICLES);
                break;
            }
        }
    }
    
public:
    ParticleColorFunction(String functionName = "particles",
                         float grav = -0.01,
                         int emitRate = 3,
                         String palette = "rainbow",
                         bool randColors = true)
        : StatefulColorFunction(functionName, 20),
          gravity(grav),
          emissionRate(emitRate),
          emissionCounter(0),
          paletteName(palette),
          randomColors(randColors) {
        reset();
    }
    
    void reset() override {
        for(int i = 0; i < MAX_PARTICLES; i++) {
            particles[i] = Particle();
        }
        emissionCounter = 0;
    }
    
    void updateState() override {
        for(int i = 0; i < MAX_PARTICLES; i++) {
            if(particles[i].life > 0) {
                particles[i].velocity += gravity;
                particles[i].position += particles[i].velocity;
                
                particles[i].life = max(0, particles[i].life - 5);
                
                if(particles[i].position < 0 || particles[i].position > 1) {
                    particles[i].life = 0;
                }
            }
        }
        
        emissionCounter++;
        if(emissionCounter >= emissionRate) {
            emissionCounter = 0;
            emitParticle();
        }
    }
    
    CRGB getColor(float position) override {
        CRGB totalColor = CRGB::Black;
        
        for(int i = 0; i < MAX_PARTICLES; i++) {
            if(particles[i].life > 0) {
                float distance = abs(particles[i].position - position);
                if(distance < 0.05) {
                    float influence = (0.05 - distance) * 20;
                    influence *= particles[i].life / 255.0;
                    
                    CRGBPalette16* palette = PaletteRegistry::findByName(paletteName);
                    if(palette == nullptr) {
                        palette = PaletteRegistry::findByName("rainbow");
                    }
                    
                    CRGB particleColor = ColorFromPalette(*palette, particles[i].hue);
                    particleColor.nscale8(influence * 255);
                    
                    totalColor += particleColor;
                }
            }
        }
        
        return totalColor;
    }
    
    void setPaletteName(String name) { paletteName = name; }
    void setGravity(float g) { gravity = g; }
    void setEmissionRate(int rate) { emissionRate = max(1, rate); }
    void setRandomColors(bool random) { randomColors = random; }
};

/////////////////////////////////////////
// COLOR FUNCTION REGISTRY
//

extern ColorFunction colorFunctionArray[numcolorfunctions];
extern String colorFunctionNames[numcolorfunctions];
extern StatefulColorFunction* statefulColorFunctions[numcolorfunctions];
extern int numStatefulColorFunctions;

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

inline CRGB callStatefulColorFunction(int index, float position) {
    if (index >= 0 && index < numcolorfunctions && 
        statefulColorFunctions[index] != nullptr) {
        statefulColorFunctions[index]->updateIfNeeded(millis());
        return statefulColorFunctions[index]->getColor(position);
    }
    return CRGB(0, 0, 0);
}

/////////////////////////////////////////
// HELPER FUNCTIONS
//

int findColorFunctionByName(String name);
CRGB getColorFromFunction(int index, float position);
void printActiveStatefulFunctions();
void initializeStatefulColorFunctions();

/////////////////////////////////////////
// PALETTE SWITCHING FUNCTIONS
//

// Switch a specific function to use a different palette
void switchPalette(String functionName, String paletteName);

// Cycle all palettized functions to the next palette in the registry
void cycleAllPalettes();

// Randomize all palettes - each function gets a different random palette
void randomizeAllPalettes();

#endif  // COLORFUNCTIONS_H
