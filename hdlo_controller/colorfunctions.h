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
#include "audiosystem.h"  // Centralized audio system with FFT

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
    
    // Virtual function for palette support - override getPaletteName() in derived classes
    // Default setPalette() calls setPaletteName() if it exists (for compatibility)
    virtual String getPaletteName() const { return ""; }
    virtual void setPalette(String paletteName) {}  // Override this in palette-supporting classes
    
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
    String getPaletteName() const override { return paletteName; }
    void setPalette(String name) override { setPaletteName(name); }
};

/////////////////////////////////////////
// AUDIO REACTIVE COLOR FUNCTION
// Uses centralized AudioSystem with FFT
//

class AudioReactiveColorFunction : public StatefulColorFunction {
private:
    static const int NUM_LEDS = 128;
    
    byte brightness[NUM_LEDS];
    int sensitivity;
    byte currentLevel;
    float decay;
    String paletteName;
    bool useHSV;
    
public:
    AudioReactiveColorFunction(String functionName = "audio", 
                               int pin = A0,  // Kept for compatibility, but unused
                               int sens = 100,
                               float decayRate = 0.95,
                               String palette = "rainbow",
                               bool hsvMode = false)
        : StatefulColorFunction(functionName, 20),
          sensitivity(sens),
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
        currentLevel = 0;
    }
    
    void updateState() override {
        // Get audio level from centralized audio system
        float audioLevel = AudioSystem::getLevel();
        
        // Scale by sensitivity and convert to byte (0-255)
        byte newLevel = constrain(audioLevel * sensitivity * 2550, 0, 255);
        currentLevel = newLevel;
        
        // Create audio-reactive spread effect
        int center = NUM_LEDS / 2;
        int spread = (currentLevel * NUM_LEDS) / (255 * 2);
        
        // Apply decay to all LEDs
        for(int i = 0; i < NUM_LEDS; i++) {
            brightness[i] = brightness[i] * decay;
        }
        
        // Add new brightness based on audio level
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
    void setPaletteName(String name) { paletteName = name; }
    String getPaletteName() const override { return paletteName; }
    void setPalette(String name) override { setPaletteName(name); }
    void setHSVMode(bool mode) { useHSV = mode; }
    byte getCurrentLevel() const { return currentLevel; }
};

/////////////////////////////////////////
// VU METER COLOR FUNCTION
// Uses centralized AudioSystem with FFT
//

class VUMeterColorFunction : public StatefulColorFunction {
private:
    static const int NUM_LEDS = 128;
    int sensitivity;
    byte peakLevel;
    byte currentLevel;
    byte peakHold;
    unsigned long peakHoldTime;
    unsigned long lastPeakTime;
    bool useClassicColors;
    String paletteName;  // Use palette instead of fixed peak color
    
public:
    VUMeterColorFunction(String functionName = "vumeter",
                        int pin = A0,  // Kept for compatibility, but unused
                        int sens = 100,
                        bool classic = false,  // Changed default to use palette
                        String palette = "rainbow")
        : StatefulColorFunction(functionName, 20),
          sensitivity(sens),
          peakLevel(0),
          currentLevel(0),
          peakHold(0),
          peakHoldTime(500),
          lastPeakTime(0),
          useClassicColors(classic),
          paletteName(palette) {
        reset();
    }
    
    void reset() override {
        peakLevel = 0;
        currentLevel = 0;
        peakHold = 0;
        lastPeakTime = 0;
    }
    
    void updateState() override {
        // Get audio level from centralized audio system
        float audioLevel = AudioSystem::getLevel();
        
        // Scale by sensitivity and convert to byte (0-255)
        byte level = constrain(audioLevel * sensitivity * 2550, 0, 255);
        currentLevel = level;
        
        // Track peak with hold time
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
                // Classic VU meter colors
                if(position < 0.5) {
                    color = CRGB::Green;
                } else if(position < 0.75) {
                    color = CRGB::Yellow;
                } else {
                    color = CRGB::Red;
                }
            } else {
                // Use palette - map position to palette
                CRGBPalette16* palette = PaletteRegistry::findByName(paletteName);
                if(palette == nullptr) {
                    palette = PaletteRegistry::findByName("rainbow");
                }
                byte paletteIndex = position * 255;
                color = ColorFromPalette(*palette, paletteIndex);
                // Scale by level for smooth fill
                byte brightness = (ledLevel * 255) / max(1, currentLevel);
                color.nscale8(brightness);
            }
        } else if(abs(ledLevel - peakHold) < 5) {
            // Peak indicator - use bright color from palette
            CRGBPalette16* palette = PaletteRegistry::findByName(paletteName);
            if(palette == nullptr) {
                palette = PaletteRegistry::findByName("rainbow");
            }
            color = ColorFromPalette(*palette, 255);  // Brightest color
        }
        
        return color;
    }
    
    void setSensitivity(int sens) { sensitivity = constrain(sens, 10, 500); }
    void setPeakHoldTime(unsigned long ms) { peakHoldTime = ms; }
    void setClassicMode(bool classic) { useClassicColors = classic; }
    void setPaletteName(String name) { paletteName = name; }
    String getPaletteName() const override { return paletteName; }
    void setPalette(String name) override { setPaletteName(name); }
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
    String getPaletteName() const override { return paletteName; }
    void setPalette(String name) override { setPaletteName(name); }
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
    String getPaletteName() const override { return paletteName; }
    void setPalette(String name) override { setPaletteName(name); }
    void setGravity(float g) { gravity = g; }
    void setEmissionRate(int rate) { emissionRate = max(1, rate); }
    void setRandomColors(bool random) { randomColors = random; }
};

/////////////////////////////////////////
// CYLON EFFECT
// Classic scanning eye effect with trailing tail
//

class CylonEffect : public StatefulColorFunction {
private:
    float position;
    float velocity;
    float speed;
    String paletteName;
    
public:
    CylonEffect(String functionName = "cylon",
               float scanSpeed = 0.03,
               String palette = "fire")
        : StatefulColorFunction(functionName, 20),
          position(0.0),
          velocity(scanSpeed),
          speed(scanSpeed),
          paletteName(palette) {
        reset();
    }
    
    void reset() override {
        position = 0.0;
        velocity = speed;
    }
    
    void updateState() override {
        // Move the position
        position += velocity;
        
        // Bounce at edges
        if(position >= 1.0) {
            position = 1.0;
            velocity = -speed;
        } else if(position <= 0.0) {
            position = 0.0;
            velocity = speed;
        }
    }
    
    CRGB getColor(float ledPosition) override {
        // Calculate distance from scanning position
        float distance = ledPosition - position;
        
        // Determine if this LED is in front or behind based on direction
        bool isAhead = (velocity > 0) ? (distance > 0) : (distance < 0);
        distance = abs(distance);
        
        float brightness = 0;
        
        if(isAhead) {
            // Leading edge - bright spot
            if(distance < 0.05) {
                brightness = 1.0 - (distance / 0.05);
            }
        } else {
            // Trailing tail - fades off
            if(distance < 0.25) {
                brightness = 0.8 * (1.0 - (distance / 0.25));
            }
        }
        
        // Get color from palette
        CRGBPalette16* palette = PaletteRegistry::findByName(paletteName);
        if(palette == nullptr) {
            palette = PaletteRegistry::findByName("fire");
        }
        
        // Use position for palette color
        byte paletteIndex = ledPosition * 255;
        CRGB color = ColorFromPalette(*palette, paletteIndex);
        color.nscale8(brightness * 255);
        
        return color;
    }
    
    void setSpeed(float newSpeed) { 
        speed = abs(newSpeed);
        if(velocity > 0) velocity = speed;
        else velocity = -speed;
    }
    void setPaletteName(String name) { paletteName = name; }
    String getPaletteName() const override { return paletteName; }
    void setPalette(String name) override { setPaletteName(name); }
};

/////////////////////////////////////////
// AUDIO CYLON EFFECT
// Cylon effect where speed varies with volume and leading edge color 
// represents the dominant vocal frequency bins mapped to palette colors
//

class AudioCylonEffect : public StatefulColorFunction {
private:
    static const int NUM_LEDS = 128;
    static const int NUM_VOCAL_BINS = 16;  // Use bands 0-15 for ~0-344Hz (fundamental + lower harmonics)
    
    float position;
    float velocity;
    float baseSpeed;
    byte trailBrightness[NUM_LEDS];
    CRGB trailColor[NUM_LEDS];
    String paletteName;
    
public:
    AudioCylonEffect(String functionName = "audiocylon",
                    float scanSpeed = 0.02,
                    String palette = "rainbow")
        : StatefulColorFunction(functionName, 20),
          position(0.0),
          velocity(scanSpeed),
          baseSpeed(scanSpeed),
          paletteName(palette) {
        reset();
    }
    
    void reset() override {
        position = 0.0;
        velocity = baseSpeed;
        for(int i = 0; i < NUM_LEDS; i++) {
            trailBrightness[i] = 0;
            trailColor[i] = CRGB::Black;
        }
    }
    
    void updateState() override {
        // Get audio level for speed modulation
        float audioLevel = AudioSystem::getLevel();
        
        // Modulate speed based on audio level (0.5x to 3x base speed)
        float speedMultiplier = 0.5 + (audioLevel * 2.5);
        float currentSpeed = baseSpeed * speedMultiplier;
        
        // Update velocity magnitude while preserving direction
        if(velocity > 0) {
            velocity = currentSpeed;
        } else {
            velocity = -currentSpeed;
        }
        
        // Move the position
        position += velocity;
        
        // Bounce at edges
        if(position >= 1.0) {
            position = 1.0;
            velocity = -abs(velocity);
        } else if(position <= 0.0) {
            position = 0.0;
            velocity = abs(velocity);
        }
        
        // Fade all trail brightness
        for(int i = 0; i < NUM_LEDS; i++) {
            trailBrightness[i] = trailBrightness[i] * 0.95;
        }
        
        // Get frequency bins in vocal range - using bands 0-7 based on actual data
        float binEnergies[NUM_VOCAL_BINS];
        float maxEnergy = 0;
        
        // Debug: Show vocal range bands (abbreviated for readability)
        static unsigned long lastDebug = 0;
        if(millis() - lastDebug > 1000) {
            Serial.print("AudioCylon [0-15]: ");
            for(int i = 0; i < NUM_VOCAL_BINS; i++) {
                Serial.print(AudioSystem::getBand(i), 3);
                Serial.print(" ");
            }
            lastDebug = millis();
        }
        
        for(int i = 0; i < NUM_VOCAL_BINS; i++) {
            binEnergies[i] = AudioSystem::getBand(i) * 10000000;  // 10 million boost
            if(binEnergies[i] > maxEnergy) maxEnergy = binEnergies[i];
        }
        
        // Debug boosted values occasionally
        if(millis() - lastDebug > 1000) {
            Serial.print(" | Boosted: ");
            for(int i = 0; i < NUM_VOCAL_BINS; i++) {
                Serial.print(binEnergies[i], 0);
                Serial.print(" ");
            }
        }
        
        // Get palette
        CRGBPalette16* palette = PaletteRegistry::findByName(paletteName);
        if(palette == nullptr) {
            palette = PaletteRegistry::findByName("rainbow");
        }
        
        // Blend colors from dominant bins - use only top 3 for clearer colors
        CRGB leadingColor = CRGB::Black;
        float totalEnergy = 0;
        
        // Find top 3 bins by energy
        int topBins[3] = {0, 0, 0};
        float topEnergies[3] = {0, 0, 0};
        
        for(int i = 0; i < NUM_VOCAL_BINS; i++) {
            if(binEnergies[i] > topEnergies[0]) {
                topBins[2] = topBins[1];
                topEnergies[2] = topEnergies[1];
                topBins[1] = topBins[0];
                topEnergies[1] = topEnergies[0];
                topBins[0] = i;
                topEnergies[0] = binEnergies[i];
            } else if(binEnergies[i] > topEnergies[1]) {
                topBins[2] = topBins[1];
                topEnergies[2] = topEnergies[1];
                topBins[1] = i;
                topEnergies[1] = binEnergies[i];
            } else if(binEnergies[i] > topEnergies[2]) {
                topBins[2] = i;
                topEnergies[2] = binEnergies[i];
            }
            totalEnergy += binEnergies[i];
        }
        
        if(totalEnergy > 1) {
            // Just use the dominant bin's color from the palette
            int dominantBin = topBins[0];
            
            // Map bin to palette position evenly across full palette range
            byte paletteIndex = (dominantBin * 255) / (NUM_VOCAL_BINS - 1);
            leadingColor = ColorFromPalette(*palette, paletteIndex);
            
            // Apply brightness boost
            leadingColor.nscale8(min(255, (int)(50 * (topEnergies[0] / 100000))));
        } else {
            // No vocal energy - use first palette color
            leadingColor = ColorFromPalette(*palette, 0);
        }
        
        // Debug final color and top bins
        if(millis() - lastDebug < 50) {  // Same update cycle
            Serial.print(" | Dom bin: ");
            Serial.print(topBins[0]);
            Serial.print(" | RGB: ");
            Serial.print(leadingColor.r);
            Serial.print(",");
            Serial.print(leadingColor.g);
            Serial.print(",");
            Serial.println(leadingColor.b);
        }
        
        // Paint leading edge with interpolation to avoid gaps
        float ledPos = position * (NUM_LEDS - 1);
        int led1 = (int)ledPos;
        int led2 = (velocity > 0) ? led1 + 1 : led1 - 1;  // Direction-aware
        float frac = ledPos - led1;
        if(velocity < 0) frac = 1.0 - frac;  // Reverse fractional position
        
        // Set both LEDs for smooth motion
        if(led1 >= 0 && led1 < NUM_LEDS) {
            trailBrightness[led1] = 255;
            trailColor[led1] = leadingColor;
        }
        if(led2 >= 0 && led2 < NUM_LEDS) {
            trailBrightness[led2] = 255 * frac;  // Fade based on fractional position
            trailColor[led2] = leadingColor;
        }
    }
    
    CRGB getColor(float ledPosition) override {
        int ledIndex = ledPosition * (NUM_LEDS - 1);
        ledIndex = constrain(ledIndex, 0, NUM_LEDS - 1);
        
        CRGB color = trailColor[ledIndex];
        color.nscale8(trailBrightness[ledIndex]);
        
        return color;
    }
    
    void setBaseSpeed(float newSpeed) { baseSpeed = abs(newSpeed); }
    void setPaletteName(String name) { paletteName = name; }
    String getPaletteName() const override { return paletteName; }
    void setPalette(String name) override { setPaletteName(name); }
};


/////////////////////////////////////////
// BEAT DETECTOR
// Detects beats and creates flash effects
//

class BeatDetector : public StatefulColorFunction {
private:
    static const int NUM_LEDS = 128;
    static const int HISTORY_SIZE = 40;
    
    float bassHistory[HISTORY_SIZE];
    int historyIndex;
    float beatThreshold;
    bool beatDetected;
    unsigned long lastBeatTime;
    float beatBrightness;
    String paletteName;
    
    float getAverageBass() {
        float sum = 0;
        for(int i = 0; i < HISTORY_SIZE; i++) {
            sum += bassHistory[i];
        }
        return sum / HISTORY_SIZE;
    }
    
public:
    BeatDetector(String functionName = "beatdetect",
                String palette = "fire")
        : StatefulColorFunction(functionName, 20),
          historyIndex(0),
          beatThreshold(1.5),  // Must be 1.5x average
          beatDetected(false),
          lastBeatTime(0),
          beatBrightness(0),
          paletteName(palette) {
        reset();
    }
    
    void reset() override {
        for(int i = 0; i < HISTORY_SIZE; i++) {
            bassHistory[i] = 0;
        }
        historyIndex = 0;
        beatDetected = false;
        beatBrightness = 0;
    }
    
    void updateState() override {
        // Get current bass level
        float currentBass = AudioSystem::getBass();
        
        // Store in history
        bassHistory[historyIndex] = currentBass;
        historyIndex = (historyIndex + 1) % HISTORY_SIZE;
        
        // Calculate average
        float avgBass = getAverageBass();
        
        // Detect beat (current level is much higher than average)
        unsigned long now = millis();
        if(currentBass > avgBass * beatThreshold && 
           now - lastBeatTime > 200) {  // Minimum 200ms between beats
            beatDetected = true;
            lastBeatTime = now;
            beatBrightness = 255;
        }
        
        // Decay brightness after beat
        if(beatDetected) {
            beatBrightness *= 0.85;
            if(beatBrightness < 10) {
                beatDetected = false;
            }
        }
    }
    
    CRGB getColor(float position) override {
        if(!beatDetected) return CRGB::Black;
        
        // Flash effect on beat
        CRGBPalette16* palette = PaletteRegistry::findByName(paletteName);
        if(palette == nullptr) {
            palette = PaletteRegistry::findByName("fire");
        }
        
        byte index = position * 255;
        CRGB color = ColorFromPalette(*palette, index);
        color.nscale8(beatBrightness);
        
        return color;
    }
    
    void setBeatThreshold(float threshold) { 
        beatThreshold = constrain(threshold, 1.1, 3.0); 
    }
    void setPaletteName(String name) { paletteName = name; }
    String getPaletteName() const override { return paletteName; }
    void setPalette(String name) override { setPaletteName(name); }
};

/////////////////////////////////////////
// VOCAL HIGHLIGHTER
// Responds specifically to vocal/mid frequencies
//

class VocalHighlighter : public StatefulColorFunction {
private:
    static const int NUM_LEDS = 128;
    
    byte brightness[NUM_LEDS];
    float vocalLevel;
    float decay;
    String paletteName;  // Use palette instead of fixed color
    
public:
    VocalHighlighter(String functionName = "vocals",
                    float decayRate = 0.9,
                    String palette = "ocean")
        : StatefulColorFunction(functionName, 20),
          vocalLevel(0),
          decay(decayRate),
          paletteName(palette) {
        reset();
    }
    
    void reset() override {
        for(int i = 0; i < NUM_LEDS; i++) {
            brightness[i] = 0;
        }
        vocalLevel = 0;
    }
    
    void updateState() override {
        // Vocals are typically in the 200-2000 Hz range
        // That maps to bands 10-19 in our system
        vocalLevel = AudioSystem::getBandRange(12, 18);
        
        // Apply decay to all LEDs
        for(int i = 0; i < NUM_LEDS; i++) {
            brightness[i] = brightness[i] * decay;
        }
        
        // Add brightness in center based on vocal level
        int center = NUM_LEDS / 2;
        int spread = (int)(vocalLevel * 50);  // Max 50 LEDs spread
        
        byte newBrightness = constrain(vocalLevel * 2550, 0, 255);
        
        for(int i = -spread; i <= spread; i++) {
            int pos = center + i;
            if(pos >= 0 && pos < NUM_LEDS) {
                brightness[pos] = max(brightness[pos], newBrightness);
            }
        }
    }
    
    CRGB getColor(float position) override {
        int index = (int)(position * (NUM_LEDS - 1));
        index = constrain(index, 0, NUM_LEDS - 1);
        
        // Get color from palette based on position
        CRGBPalette16* palette = PaletteRegistry::findByName(paletteName);
        if(palette == nullptr) {
            palette = PaletteRegistry::findByName("ocean");
        }
        
        byte paletteIndex = position * 255;
        CRGB color = ColorFromPalette(*palette, paletteIndex);
        color.nscale8(brightness[index]);
        
        return color;
    }
    
    void setDecay(float d) { decay = constrain(d, 0.5, 0.99); }
    void setPaletteName(String name) { paletteName = name; }
    String getPaletteName() const override { return paletteName; }
    void setPalette(String name) override { setPaletteName(name); }
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

inline CRGB constantlyDark(float position) {
    return CRGB(0, 0, 0);
}


// Breathing function - pulses color 0 from a palette
inline CRGB breathingColor(float position) {
    // Get the white palette (or any palette you want)
    CRGBPalette16* palette = PaletteRegistry::findByName("white");
    if(palette == nullptr) {
        // Fallback to first registered palette if white not found
        palette = PaletteRegistry::getByIndex(0);
    }
    
    // Breathing effect: sine wave for smooth pulsing
    float breathAmount = (sin(millis() / 1000.0) + 1.0) / 2.0; // 0 to 1
    
    // Get color 0 from palette (index 0)
    CRGB color = ColorFromPalette(*palette, 0);
    
    // Apply breathing to brightness
    color.nscale8(breathAmount * MAXBRIGHTNESS);
    
    return color;
}

// Simple color function - shows the entire palette gradient
inline CRGB simpleColor(float position) {
    // Get the white palette
    CRGBPalette16* palette = PaletteRegistry::findByName("white");
    if(palette == nullptr) {
        palette = PaletteRegistry::getByIndex(0);
    }
    
    // Map position (0-1) to palette index (0-255)
    byte paletteIndex = position * 255;
    
    // Get color from palette
    CRGB color = ColorFromPalette(*palette, paletteIndex);
    
    // Scale to max brightness
    color.nscale8(MAXBRIGHTNESS);
    
    return color;
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

// Get the current palette name for a given function
String getCurrentPaletteName(String functionName);

// Cycle all palettized functions to the next palette in the registry
void cycleAllPalettes();

// Randomize all palettes - each function gets a different random palette
void randomizeAllPalettes();

#endif  // COLORFUNCTIONS_H
