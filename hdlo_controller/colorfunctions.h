////////////////////////////////////////////
//
//   colorfunctions.h
//
// Color function system with automatic palette management
// Uses PaletteRegistry for centralized palette handling
//
// AUDIO FUNCTION IMPLEMENTATION GUIDE:
// All new audio-reactive functions should follow these conventions:
// 1. Use direct FFT bin access via AudioSystem::getBin(i) not getLevel() or getBandRange()
// 2. Apply AUDIO_FFT_THRESHOLD filter when reading bins: if(n > AUDIO_FFT_THRESHOLD)
// 3. Use AUDIO_BRIGHTNESS_MULTIPLIER for amplitude scaling (4000 standard, can vary by function)
// 4. Cap all brightness at AUDIO_MAX_BRIGHTNESS (160)
// 5. Default to PALETTE MODE (useHSVMode = false) not HSV mode
// 6. Include palette support: paletteName string, getPaletteName(), setPalette() methods
// 7. Register in both cycleAllPalettes() and randomizeAllPalettes() in .cpp file
// Example bin ranges: Bass 0-9 (~43-430Hz), Vocal 5-45 (~215-1935Hz), Full 0-59 (~43-2580Hz)
//
// STATEFUL FUNCTION SLOTS (in colorfunctions.cpp):
// Index 0: dark, 1: rainbow, 2: cylon, 3: fire2012, 4: audiocylon
// Index 5: audio, 6: vumeter, 7: plasma, 8: particles, 9: beatdetect
// Index 10: vocals, 11: breathing, 12: simplecolor, 13: audio2, 14: fftfire
// Next available: 15+
//
// TUNING PARAMETERS:
// - cooling: Lower = brighter idle, slower fade (5-30 range, default 15-20)
// - brightnessScale: Higher = more responsive (1000-8000 range, default 4000)
// - fftThreshold: Higher = less sensitive to noise (0.001-0.1 range, default 0.01)
// - decay: Higher = trails persist longer (0.5-0.99 range, default 0.9-0.95)
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

// Audio reactive constants (aligned with FFT Fire algorithm)
const int AUDIO_BRIGHTNESS_MULTIPLIER = 4000;  // Standard FFTÃƒÂ¢Ã¢â‚¬Â Ã¢â‚¬â„¢brightness scale
const int AUDIO_MAX_BRIGHTNESS = 160;          // Cap to prevent oversaturation
const float AUDIO_FFT_THRESHOLD = 0.01;        // Minimum FFT value to detect (noise filter)

/////////////////////////////////////////
// FUNCTION PARAMETER
// Simple wrapper for function parameters
//
struct FunctionParameter {
    float value;
    
    FunctionParameter() : value(0.0f) {}
    FunctionParameter(float v) : value(v) {}
    
    operator float() const { return value; }
};

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
    
    // Clone method for deep copying - must be implemented by all subclasses
    virtual StatefulColorFunction* clone() const = 0;
    
    // Parameter setting - override in subclasses that support it
    virtual void setParameters(const std::vector<FunctionParameter>& params) {}
    
    String getName() const { return name; }
    
    // Virtual function for palette support - override getPaletteName() in derived classes
    // Default setPalette() calls setPaletteName() if it exists (for compatibility)
    virtual String getPaletteName() const { return ""; }
    virtual void setPalette(String paletteName) {}  // Override this in palette-supporting classes
    
    static void beginFrame() {
        globalFrameNumber++;
    }
    
    static unsigned long getGlobalFrame() {
        return globalFrameNumber;
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
    
    void setParameters(const std::vector<FunctionParameter>& params) override {
        if(params.size() >= 1) setCooling((int)params[0].value);
        if(params.size() >= 2) setSparking((int)params[1].value);
    }
    
    StatefulColorFunction* clone() const override {
        return new Fire2012ColorFunction(name, cooling, sparking, gReverseDirection, paletteName);
    }
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
    
public:
    AudioReactiveColorFunction(String functionName = "audio", 
                               int pin = A0,  // Kept for compatibility, but unused
                               int sens = 100,
                               float decayRate = 0.95,
                               String palette = "rainbow")
        : StatefulColorFunction(functionName, 20),
          sensitivity(sens),
          currentLevel(0),
          decay(decayRate),
          paletteName(palette) {
        reset();
    }
    
    void reset() override {
        for(int i = 0; i < NUM_LEDS; i++) {
            brightness[i] = 0;
        }
        currentLevel = 0;
    }
    
    void updateState() override {
        // Get audio level using direct FFT bin access (same as fftfire)
        float audioLevel = 0;
        for(int i = 0; i < 60; i++) {
            float n = AudioSystem::getBin(i);
            if(n > AUDIO_FFT_THRESHOLD) {
                audioLevel += n;
            }
        }
        audioLevel = audioLevel / 60.0;  // Average across bins
        
        // Scale by sensitivity and convert to byte, using global constant and cap
        byte newLevel = constrain(audioLevel * sensitivity * AUDIO_BRIGHTNESS_MULTIPLIER / 100, 0, AUDIO_MAX_BRIGHTNESS);
        currentLevel = newLevel;
        
        // Create audio-reactive spread effect
        int center = NUM_LEDS / 2;
        int spread = (currentLevel * NUM_LEDS) / (AUDIO_MAX_BRIGHTNESS * 2);
        
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
        
        // Always use palette mode
        CRGBPalette16* palette = PaletteRegistry::findByName(paletteName);
        if(palette == nullptr) {
            palette = PaletteRegistry::findByName("rainbow");
        }
        
        CRGB color = ColorFromPalette(*palette, value);
        color.nscale8(value);
        
        return color;
    }
    
    void setSensitivity(int sens) { sensitivity = constrain(sens, 10, 500); }
    void setDecay(float d) { decay = constrain(d, 0.5, 0.99); }
    void setPaletteName(String name) { paletteName = name; }
    String getPaletteName() const override { return paletteName; }
    void setPalette(String name) override { setPaletteName(name); }
    byte getCurrentLevel() const { return currentLevel; }
    
    void setParameters(const std::vector<FunctionParameter>& params) override {
        if(params.size() >= 1) setDecay(params[0].value);
        if(params.size() >= 2) setSensitivity((int)params[1].value);
    }
    
    StatefulColorFunction* clone() const override {
        return new AudioReactiveColorFunction(name, 0, sensitivity, decay, paletteName);
    }
};

/////////////////////////////////////////
// AUDIO REACTIVE COLOR FUNCTION 2
// Vocal-range pitch-sensitive version with fine-grained FFT bin analysis
//

class AudioReactiveColorFunction2 : public StatefulColorFunction {
private:
    static const int NUM_LEDS = 128;
    static const int MIN_VOCAL_BIN = 2;   // ~86 Hz (lower male range)
    static const int MAX_VOCAL_BIN = 9;   // ~387 Hz (typical vocal fundamental range)
    
    byte brightness[NUM_LEDS];
    byte hue[NUM_LEDS];       // Store hue per LED for trailing effect
    int sensitivity;
    float decay;
    String paletteName;
    bool useHSV;
    float smoothedPitch;      // Smoothed interpolated pitch (bin number as float)
    float smoothingAlpha;     // Smoothing factor (0-1, higher = more responsive)
    
public:
    AudioReactiveColorFunction2(String functionName = "audio2", 
                               int pin = A0,  // Kept for compatibility, but unused
                               int sens = 100,
                               float decayRate = 0.95,
                               String palette = "rainbow",
                               bool hsvMode = false)
        : StatefulColorFunction(functionName, 20),
          sensitivity(sens),
          decay(decayRate),
          paletteName(palette),
          useHSV(hsvMode),
          smoothedPitch(13.5),  // Initialize to middle of range (~560 Hz)
          smoothingAlpha(0.3) {
        reset();
    }
    
    void reset() override {
        for(int i = 0; i < NUM_LEDS; i++) {
            brightness[i] = 0;
            hue[i] = 0;
        }
        smoothedPitch = 13.5;  // Middle of vocal range
    }
    
    void updateState() override {
        // Calculate vocal level from bins 2-9 for width
        float vocalLevel = 0;
        for(int bin = 2; bin <= 9; bin++) {
            vocalLevel += AudioSystem::getBin(bin);
        }
        vocalLevel = vocalLevel / 8.0;  // Average across bins
        
        // Find the dominant pitch using convolution with neighboring bins
        // This smooths the spectrum and gives more stable pitch detection
        int dominantBin = MIN_VOCAL_BIN;
        float maxConvolved = 0;
        
        // Periodic diagnostics (every 500ms)
        static unsigned long lastDiagnostic = 0;
        unsigned long now = millis();
        bool printDiag = (now - lastDiagnostic > 500);
        
        if(printDiag) {
            Serial.print("Bins: [");
        }
        
        for(int bin = 2; bin <= 9; bin++) {  // Vocal fundamental range (~86-387 Hz)
            // Convolve: 1/10(n-2) + 2/10(n-1) + 1/2(n) + 2/10(n+1) + 1/10(n+2)
            float convolved = 0;
            convolved += AudioSystem::getBin(bin - 2) * 0.1;
            convolved += AudioSystem::getBin(bin - 1) * 0.2;
            convolved += AudioSystem::getBin(bin) * 0.5;
            convolved += AudioSystem::getBin(bin + 1) * 0.2;
            convolved += AudioSystem::getBin(bin + 2) * 0.1;
            
            if(printDiag) {
                Serial.print(AudioSystem::getBin(bin), 3);
                Serial.print("(");
                Serial.print(convolved, 3);
                Serial.print(")");
                if(bin < 9) Serial.print(" ");
            }
            
            if(convolved > maxConvolved) {
                maxConvolved = convolved;
                dominantBin = bin;
            }
        }
        
        if(printDiag) {
            Serial.print("] Winner: bin ");
            Serial.print(dominantBin);
            Serial.print(" (~");
            Serial.print(dominantBin * 43);
            Serial.println(" Hz)");
            lastDiagnostic = now;
        }
        
        // Use center bin's level for noise gate
        float maxLevel = AudioSystem::getBin(dominantBin);
        
        // Only update pitch if there's significant energy (noise gate with global threshold)
        if(maxLevel > AUDIO_FFT_THRESHOLD) {
            // Apply exponential smoothing to the dominant bin
            smoothedPitch = smoothingAlpha * dominantBin + (1.0 - smoothingAlpha) * smoothedPitch;
        }
        // else keep previous smoothedPitch value
        
        // Map smoothed pitch to hue for new pixels
        byte currentHue = map(smoothedPitch * 100, MIN_VOCAL_BIN * 100, MAX_VOCAL_BIN * 100, 0, 255);
        currentHue = constrain(currentHue, 0, 255);
        
        // Calculate spread based on total audio level (keep 1200 multiplier as it's vocal-specific)
        byte vocalBrightness = constrain(vocalLevel * sensitivity * 1200 / 100, 0, AUDIO_MAX_BRIGHTNESS);
        int center = NUM_LEDS / 2;
        int spread = (vocalBrightness * NUM_LEDS) / (AUDIO_MAX_BRIGHTNESS * 2);
        
        // Apply decay to all LEDs (brightness decays, hue stays)
        for(int i = 0; i < NUM_LEDS; i++) {
            brightness[i] = brightness[i] * decay;
        }
        
        // Add new brightness at leading edge with current hue
        for(int i = 0; i < spread; i++) {
            int pos1 = center + i;
            int pos2 = center - i;
            
            if(pos1 < NUM_LEDS) {
                if(vocalBrightness > brightness[pos1]) {
                    brightness[pos1] = vocalBrightness;
                    hue[pos1] = currentHue;  // Update hue only on leading edge
                }
            }
            if(pos2 >= 0) {
                if(vocalBrightness > brightness[pos2]) {
                    brightness[pos2] = vocalBrightness;
                    hue[pos2] = currentHue;  // Update hue only on leading edge
                }
            }
        }
    }
    
    CRGB getColor(float position) override {
        int index = (int)(position * (NUM_LEDS - 1));
        index = constrain(index, 0, NUM_LEDS - 1);
        
        byte value = brightness[index];
        byte ledHue = hue[index];  // Use stored hue for this LED
        
        if(useHSV) {
            // Use stored hue with full saturation
            return CHSV(ledHue, 255, value);
        } else {
            // Map stored hue to palette index
            CRGBPalette16* palette = PaletteRegistry::findByName(paletteName);
            if(palette == nullptr) {
                palette = PaletteRegistry::findByName("rainbow");
            }
            
            CRGB color = ColorFromPalette(*palette, ledHue);
            color.nscale8(value);
            
            return color;
        }
    }
    
    void setSensitivity(int sens) { sensitivity = constrain(sens, 10, 500); }
    void setDecay(float d) { decay = constrain(d, 0.5, 0.99); }
    void setSmoothingAlpha(float alpha) { smoothingAlpha = constrain(alpha, 0.05, 0.95); }
    void setPaletteName(String name) { paletteName = name; }
    String getPaletteName() const override { return paletteName; }
    void setPalette(String name) override { setPaletteName(name); }
    void setHSVMode(bool mode) { useHSV = mode; }
    float getSmoothedPitch() const { return smoothedPitch; }
    
    StatefulColorFunction* clone() const override {
        return new AudioReactiveColorFunction2(name, 0, sensitivity, decay, paletteName, useHSV);
    }
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
    String paletteName;
    
public:
    VUMeterColorFunction(String functionName = "vumeter",
                        int pin = A0,  // Kept for compatibility, but unused
                        int sens = 100,
                        String palette = "vumeter")
        : StatefulColorFunction(functionName, 20),
          sensitivity(sens),
          peakLevel(0),
          currentLevel(0),
          peakHold(0),
          peakHoldTime(500),
          lastPeakTime(0),
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
        // Get audio level using direct FFT bin access (same as fftfire)
        float audioLevel = 0;
        for(int i = 0; i < 60; i++) {
            float n = AudioSystem::getBin(i);
            if(n > AUDIO_FFT_THRESHOLD) {
                audioLevel += n;
            }
        }
        audioLevel = audioLevel / 60.0;  // Average across bins
        
        // Scale by sensitivity - VU meter uses higher multiplier for full-range display
        byte level = constrain(audioLevel * sensitivity * 2550 / 100, 0, AUDIO_MAX_BRIGHTNESS);
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
            // Use palette - map position to palette
            CRGBPalette16* palette = PaletteRegistry::findByName(paletteName);
            if(palette == nullptr) {
                palette = PaletteRegistry::findByName("vumeter");
            }
            byte paletteIndex = position * 255;
            color = ColorFromPalette(*palette, paletteIndex);
            // Scale by level for smooth fill
            byte brightness = (ledLevel * 255) / max(1, currentLevel);
            color.nscale8(brightness);
        } else if(abs(ledLevel - peakHold) < 5) {
            // Peak indicator - use bright color from palette
            CRGBPalette16* palette = PaletteRegistry::findByName(paletteName);
            if(palette == nullptr) {
                palette = PaletteRegistry::findByName("vumeter");
            }
            color = ColorFromPalette(*palette, 255);  // Brightest color
        }
        
        return color;
    }
    
    void setSensitivity(int sens) { sensitivity = constrain(sens, 10, 500); }
    void setPeakHoldTime(unsigned long ms) { peakHoldTime = ms; }
    void setPaletteName(String name) { paletteName = name; }
    String getPaletteName() const override { return paletteName; }
    void setPalette(String name) override { setPaletteName(name); }
    byte getCurrentLevel() const { return currentLevel; }
    
    void setParameters(const std::vector<FunctionParameter>& params) override {
        if(params.size() >= 1) setPeakHoldTime((unsigned long)params[0].value);
        if(params.size() >= 2) setSensitivity((int)params[1].value);
    }
    
    StatefulColorFunction* clone() const override {
        return new VUMeterColorFunction(name, 0, sensitivity, paletteName);
    }
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
    
    void setParameters(const std::vector<FunctionParameter>& params) override {
        if(params.size() >= 1) {
            // Single parameter controls overall speed
            float speedMult = params[0].value;
            setSpeeds(0.02 * speedMult, 0.03 * speedMult, 0.01 * speedMult);
        }
    }
    
    StatefulColorFunction* clone() const override {
        return new PlasmaColorFunction(name, speed1, speed2, speed3, scale1, scale2, scale3, paletteName);
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
    
    StatefulColorFunction* clone() const override {
        return new ParticleColorFunction(name, gravity, emissionRate, paletteName, randomColors);
    }
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
    int loopMode;  // -1: backwards only, 0: bounce, 1: forwards only
    float phase;
    float phaseOffset;
    String paletteName;
    
public:
    CylonEffect(String functionName = "cylon",
               float scanSpeed = 1.0,
               int loop = 0,
               float phaseSpeed = 1.0,
               String palette = "fire")
        : StatefulColorFunction(functionName, 20),
          position(0.0),
          speed(scanSpeed),
          loopMode(constrain(loop, -1, 1)),
          phase(phaseSpeed),
          phaseOffset(0.0),
          paletteName(palette) {
        // Scale speed: speed=1.0 means one complete bounce in 2Ãâ‚¬ seconds
        // velocity per update = speed * (0.020 / Ãâ‚¬)
        // Initialize direction based on loopMode
        if(loopMode == -1) {
            velocity = -(speed * 0.00636620); // Start moving backwards
            position = 1.0; // Start at end
        } else {
            velocity = speed * 0.00636620; // Start moving forwards
        }
        reset();
    }
    
    void reset() override {
        phaseOffset = 0.0;
        if(loopMode == -1) {
            position = 1.0; // Start at end for backwards
            velocity = -(speed * 0.00636620);
        } else {
            position = 0.0; // Start at beginning
            velocity = speed * 0.00636620;
        }
    }
    
    void updateState() override {
        // Move the position
        position += velocity;
        
        // Handle edge behavior based on loopMode
        if(loopMode == 0) {
            // Bounce mode
            if(position >= 1.0) {
                position = 1.0;
                velocity = -(speed * 0.00636620);
            } else if(position <= 0.0) {
                position = 0.0;
                velocity = speed * 0.00636620;
            }
        } else if(loopMode == 1) {
            // Forward loop only
            if(position >= 1.0) {
                position = 0.0; // Wrap to beginning
            }
        } else { // loopMode == -1
            // Backward loop only
            if(position <= 0.0) {
                position = 1.0; // Wrap to end
            }
        }
        
        // Update phase offset for palette cycling
        // phase=1.0 means one complete palette cycle in 2Ãâ‚¬ seconds (matching speed scaling)
        phaseOffset += speed * phase * 0.815; // 256 / (50 * 2Ãâ‚¬) Ã¢â€°Ë† 0.815
        if(phaseOffset >= 256) phaseOffset -= 256;
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
        
        // Use position with phase offset for palette color cycling
        byte paletteIndex = ((int)(ledPosition * 255 + phaseOffset)) % 256;
        CRGB color = ColorFromPalette(*palette, paletteIndex);
        color.nscale8(brightness * 255);
        
        return color;
    }
    
    void setSpeed(float newSpeed) { 
        speed = abs(newSpeed);
        // Apply speed scaling: speed=1.0 means one bounce in 2Ãâ‚¬ seconds
        float scaledSpeed = speed * 0.00636620; // 0.020 / Ãâ‚¬
        if(velocity > 0) velocity = scaledSpeed;
        else velocity = -scaledSpeed;
    }
    void setLoopMode(int mode) { 
        loopMode = constrain(mode, -1, 1);
        // Reset position and velocity based on new mode
        if(loopMode == -1) {
            position = 1.0;
            velocity = -(speed * 0.00636620);
        } else if(loopMode == 1) {
            position = 0.0;
            velocity = speed * 0.00636620;
        }
    }
    void setPaletteName(String name) { paletteName = name; }
    String getPaletteName() const override { return paletteName; }
    void setPalette(String name) override { setPaletteName(name); }
    
    void setPhase(float newPhase) {
        phase = newPhase;
    }
    
    void setParameters(const std::vector<FunctionParameter>& params) override {
        if(params.size() >= 1) setSpeed(params[0].value);
        if(params.size() >= 2) setLoopMode((int)params[1].value);
        if(params.size() >= 3) setPhase(params[2].value);
    }
    
    StatefulColorFunction* clone() const override {
        return new CylonEffect(name, speed, loopMode, phase, paletteName);
    }
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
    int sensitivity;
    int loopMode;  // -1: backwards only, 0: bounce, 1: forwards only
    byte trailBrightness[NUM_LEDS];
    CRGB trailColor[NUM_LEDS];
    String paletteName;
    
public:
    AudioCylonEffect(String functionName = "audiocylon",
                    float scanSpeed = 1.0,
                    int sens = 100,
                    int loop = 0,
                    String palette = "rainbow")
        : StatefulColorFunction(functionName, 20),
          position(0.0),
          baseSpeed(scanSpeed),
          sensitivity(sens),
          loopMode(constrain(loop, -1, 1)),
          paletteName(palette) {
        // Initialize velocity with scaled speed and direction based on loopMode
        if(loopMode == -1) {
            velocity = -(baseSpeed * 0.00636620);
            position = 1.0;
        } else {
            velocity = baseSpeed * 0.00636620;
        }
        reset();
    }
    
    void reset() override {
        if(loopMode == -1) {
            position = 1.0;
            velocity = -(baseSpeed * 0.00636620);
        } else {
            position = 0.0;
            velocity = baseSpeed * 0.00636620;
        }
        for(int i = 0; i < NUM_LEDS; i++) {
            trailBrightness[i] = 0;
            trailColor[i] = CRGB::Black;
        }
    }
    
    void updateState() override {
        // Get audio level for speed modulation using direct bins
        float audioLevel = 0;
        for(int i = 0; i < 60; i++) {
            float n = AudioSystem::getBin(i);
            if(n > AUDIO_FFT_THRESHOLD) {
                audioLevel += n;
            }
        }
        audioLevel = audioLevel / 60.0;
        
        // Speed modulation: 0.5x to (0.5 + audioLevel * 2 * sensitivity)x base speed
        float speedMultiplier = 0.5 + (audioLevel * 2 * sensitivity);
        // Apply speed scaling: speed=1.0 means one bounce in 2Ãâ‚¬ seconds
        float currentSpeed = baseSpeed * speedMultiplier * 0.00636620; // 0.020 / Ãâ‚¬
        
        // Update velocity magnitude while preserving direction
        if(velocity > 0) {
            velocity = currentSpeed;
        } else {
            velocity = -currentSpeed;
        }
        
        // Move the position
        position += velocity;
        
        // Handle edge behavior based on loopMode
        if(loopMode == 0) {
            // Bounce mode
            if(position >= 1.0) {
                position = 1.0;
                velocity = -abs(velocity);
            } else if(position <= 0.0) {
                position = 0.0;
                velocity = abs(velocity);
            }
        } else if(loopMode == 1) {
            // Forward loop only
            if(position >= 1.0) {
                position = 0.0; // Wrap to beginning
            }
        } else { // loopMode == -1
            // Backward loop only
            if(position <= 0.0) {
                position = 1.0; // Wrap to end
            }
        }
        
        // Fade all trail brightness
        for(int i = 0; i < NUM_LEDS; i++) {
            trailBrightness[i] = trailBrightness[i] * 0.95;
        }
        
        // Get frequency bins in vocal range - using bins directly
        float binEnergies[NUM_VOCAL_BINS];
        float maxEnergy = 0;
        
        for(int i = 0; i < NUM_VOCAL_BINS; i++) {
            binEnergies[i] = AudioSystem::getBin(i) * 50000;  // Increased from 10M (was too much)
            if(binEnergies[i] > maxEnergy) maxEnergy = binEnergies[i];
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
    void setSensitivity(int sens) { sensitivity = constrain(sens, 10, 500); }
    void setLoopMode(int mode) { 
        loopMode = constrain(mode, -1, 1);
        // Reset position and velocity based on new mode
        if(loopMode == -1) {
            position = 1.0;
            velocity = -(baseSpeed * 0.00636620);
        } else if(loopMode == 1) {
            position = 0.0;
            velocity = baseSpeed * 0.00636620;
        }
    }
    void setPaletteName(String name) { paletteName = name; }
    String getPaletteName() const override { return paletteName; }
    void setPalette(String name) override { setPaletteName(name); }
    
    void setParameters(const std::vector<FunctionParameter>& params) override {
        if(params.size() >= 1) setBaseSpeed(params[0].value);
        if(params.size() >= 2) setSensitivity((int)params[1].value);
        if(params.size() >= 3) setLoopMode((int)params[2].value);
    }
    
    StatefulColorFunction* clone() const override {
        return new AudioCylonEffect(name, baseSpeed, sensitivity, loopMode, paletteName);
    }
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
        // Get current bass level using direct bins (0-9 = ~43-430 Hz)
        float currentBass = 0;
        for(int i = 0; i <= 9; i++) {
            float n = AudioSystem::getBin(i);
            if(n > AUDIO_FFT_THRESHOLD) {
                currentBass += n;
            }
        }
        currentBass = currentBass / 10.0;  // Average across bins
        
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
            beatBrightness = AUDIO_MAX_BRIGHTNESS;
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
    
    StatefulColorFunction* clone() const override {
        return new BeatDetector(name, paletteName);
    }
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
        // Use bins 5-45 for vocal range (215-1935 Hz with ~43Hz per bin)
        float vocalLevel = 0;
        for(int i = 5; i <= 45; i++) {
            float n = AudioSystem::getBin(i);
            if(n > AUDIO_FFT_THRESHOLD) {
                vocalLevel += n;
            }
        }
        vocalLevel = vocalLevel / 41.0;  // Average across bins
        
        // Apply decay to all LEDs
        for(int i = 0; i < NUM_LEDS; i++) {
            brightness[i] = brightness[i] * decay;
        }
        
        // Add brightness in center based on vocal level
        int center = NUM_LEDS / 2;
        int spread = (int)(vocalLevel * 50);  // Max 50 LEDs spread
        
        byte newBrightness = constrain(vocalLevel * 2550, 0, AUDIO_MAX_BRIGHTNESS);
        
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
    
    StatefulColorFunction* clone() const override {
        return new VocalHighlighter(name, decay, paletteName);
    }
};

/////////////////////////////////////////
// FFT FIRE COLOR FUNCTION
// Fire effect driven by FFT frequency data
// Based on fastLEDAudioshieldFire4.ino algorithm
//

class FFTFireColorFunction : public StatefulColorFunction {
private:
    static const int NUM_LEDS = 128;
    
    // HSV storage (3 values per LED: hue, saturation, brightness)
    byte hue[NUM_LEDS];
    byte saturation[NUM_LEDS];
    byte brightness[NUM_LEDS];
    
    int cooling;              // Cooling rate (20 in original)
    int numFFTBins;           // How many FFT bins to read (60 in original)
    float fftThreshold;       // Minimum FFT value to trigger
    int brightnessScale;      // Multiplier for FFTÃƒÂ¢Ã¢â‚¬Â Ã¢â‚¬â„¢brightness
    int maxBrightness;        // Cap on brightness
    int hueMultiplier;        // FFT bin ÃƒÂ¢Ã¢â‚¬Â Ã¢â‚¬â„¢ hue mapping (6 in original)
    bool useHSVMode;          // true = HSV colors from frequency, false = use palette
    String paletteName;       // Optional: for non-HSV mode
    
public:
    FFTFireColorFunction(String functionName = "fftfire",
                         int coolingValue = 20,
                         int numBins = 60,
                         float threshold = AUDIO_FFT_THRESHOLD,
                         int brightScale = AUDIO_BRIGHTNESS_MULTIPLIER,
                         int maxBright = AUDIO_MAX_BRIGHTNESS,
                         int hueMulti = 6,
                         bool hsvMode = false,  // Default to palette mode
                         String palette = "fire")
        : StatefulColorFunction(functionName, 20),
          cooling(coolingValue),
          numFFTBins(numBins),
          fftThreshold(threshold),
          brightnessScale(brightScale),
          maxBrightness(maxBright),
          hueMultiplier(hueMulti),
          useHSVMode(hsvMode),
          paletteName(palette) {
        reset();
    }
    
    void reset() override {
        for(int i = 0; i < NUM_LEDS; i++) {
            hue[i] = 0;
            saturation[i] = 255;
            brightness[i] = 0;
        }
    }
    
    void updateState() override {
        // Step 1: COOLING - reduce brightness randomly
        for(int i = 0; i < NUM_LEDS - 3; i++) {
            brightness[i] = qsub8(brightness[i], 
                                  random8(0, ((cooling * 10) / NUM_LEDS) + 2));
        }
        
        // Step 2: DIFFUSION - blur upward (brightness)
        for(int k = NUM_LEDS - 1; k >= 2; k--) {
            brightness[k] = (brightness[k-1] + brightness[k-2] + brightness[k-2]) / 3;
        }
        
        // Step 3: DIFFUSION - blur upward (hue)
        for(int k = NUM_LEDS - 1; k >= 2; k--) {
            hue[k] = (hue[k-1] + hue[k-2] + hue[k-2]) / 3;
        }
        
        // Step 4: ADD FFT ENERGY - read FFT bins and create "sparks"
        for(int i = 0; i < numFFTBins; i++) {
            // Read FFT bin directly
            float n = AudioSystem::getBin(i);
            
            if(n > fftThreshold) {
                int ledIndex = i / 4;  // Group 4 FFT bins per LED
                
                if(ledIndex < NUM_LEDS) {
                    // Set hue based on frequency (lower freq = red, higher = blue)
                    hue[ledIndex] = (i * hueMultiplier) % 256;
                    
                    // Add brightness based on amplitude
                    int addBrightness = (int)(n * brightnessScale);
                    brightness[ledIndex] = min(maxBrightness, 
                                              brightness[ledIndex] + addBrightness);
                    
                    // Saturation stays at full
                    saturation[ledIndex] = 255;
                }
            }
        }
        
        // Step 5: DAMPING - reduce base energy
        if(NUM_LEDS > 2) {
            brightness[2] = brightness[2] / 3;
        }
    }
    
    CRGB getColor(float position) override {
        int ledIndex = (int)(position * (NUM_LEDS - 1));
        ledIndex = constrain(ledIndex, 0, NUM_LEDS - 1);
        
        if(useHSVMode) {
            // HSV mode: pure frequency-based rainbow
            return CHSV(hue[ledIndex], saturation[ledIndex], brightness[ledIndex]);
        } else {
            // Palette mode: frequency maps to palette colors
            CRGBPalette16* palette = PaletteRegistry::findByName(paletteName);
            if(palette == nullptr) {
                palette = PaletteRegistry::findByName("fire");
            }
            
            if(brightness[ledIndex] > 20) {
                // Active/bright pixel: use frequency-based rainbow hue for punch
                return CHSV(hue[ledIndex], saturation[ledIndex], brightness[ledIndex]);
            } else {
                // Background/dim pixel: use frequency to select fire palette color, scale by brightness
                // This gives you reds/oranges/yellows in background based on frequency
                CRGB color = ColorFromPalette(*palette, hue[ledIndex]);
                color.nscale8(brightness[ledIndex]);
                return color;
            }
        }
    }
    
    void setCooling(int value) { cooling = constrain(value, 0, 100); }
    void setFFTThreshold(float value) { fftThreshold = constrain(value, 0.001, 0.1); }
    void setBrightnessScale(int value) { brightnessScale = constrain(value, 1000, 8000); }
    void setHSVMode(bool mode) { useHSVMode = mode; }
    void setPaletteName(String name) { paletteName = name; }
    String getPaletteName() const override { return paletteName; }
    void setPalette(String name) override { setPaletteName(name); }
    
    void setParameters(const std::vector<FunctionParameter>& params) override {
        if(params.size() >= 1) setCooling((int)params[0].value);
        if(params.size() >= 2) setHSVMode((bool)((int)params[1].value));
        if(params.size() >= 3) setFFTThreshold(params[2].value);
    }
    
    StatefulColorFunction* clone() const override {
        return new FFTFireColorFunction(name, cooling, numFFTBins, fftThreshold, brightnessScale, maxBrightness, hueMultiplier, useHSVMode, paletteName);
    }
};

/////////////////////////////////////////
// PERLIN COLOR FUNCTION
//

class PerlinColorFunction : public StatefulColorFunction {
private:
    static const int kMatrixWidth = 16;
    static const int kMatrixHeight = 16;
    static const int MAX_DIMENSION = 16;
    
    uint8_t noise[MAX_DIMENSION][MAX_DIMENSION];
    uint16_t x;
    uint16_t y;
    uint16_t z;
    uint16_t speed;
    uint16_t scale;
    float driftSpeed;
    String paletteName;
    uint8_t colorLoop;
    uint8_t ihue;
    
    void fillNoise8() {
        uint8_t dataSmoothing = 0;
        if(speed < 50) {
            dataSmoothing = 200 - (speed * 4);
        }
        
        for(int i = 0; i < MAX_DIMENSION; i++) {
            int ioffset = scale * i;
            for(int j = 0; j < MAX_DIMENSION; j++) {
                int joffset = scale * j;
                
                uint8_t data = inoise8(x + ioffset, y + joffset, z);
                data = qsub8(data, 16);
                data = qadd8(data, scale8(data, 39));
                
                if(dataSmoothing) {
                    uint8_t olddata = noise[i][j];
                    uint8_t newdata = scale8(olddata, dataSmoothing) + 
                                     scale8(data, 256 - dataSmoothing);
                    data = newdata;
                }
                
                noise[i][j] = data;
            }
        }
        
        z += speed;
        x += speed / 8;
        y -= speed / 16;
    }
    
public:
    PerlinColorFunction(String functionName = "perlin",
                            uint16_t animSpeed = 20,
                            uint16_t noiseScale = 30,
                            float drift = 0.0,
                            uint8_t enableColorLoop = 0,
                            String palette = "rainbow")
        : StatefulColorFunction(functionName, 20),
          speed(animSpeed),
          scale(noiseScale),
          driftSpeed(drift),
          paletteName(palette),
          colorLoop(enableColorLoop),
          ihue(0) {
        reset();
    }
    
    void reset() override {
        x = random16();
        y = random16();
        z = random16();
        
        for(int i = 0; i < MAX_DIMENSION; i++) {
            for(int j = 0; j < MAX_DIMENSION; j++) {
                noise[i][j] = 0;
            }
        }
        
        ihue = 0;
        lastUpdateTime = 0;
    }
    
    void updateState() override {
        fillNoise8();
        if(colorLoop) {
            ihue++;
        }
    }
    
    CRGB getColor(float position) override {
        // Apply drift to position (driftSpeed = units per second)
        float drift = fmod(driftSpeed * millis() / 1000.0, 1.0);
        position = fmod(position + drift, 1.0);
        
        // Position is used as-is - edge mapping already applied
        // Map position (whatever range edge provides) to 16x16 matrix
        int totalIndex = (int)(position * (kMatrixWidth * kMatrixHeight - 1));
        totalIndex = constrain(totalIndex, 0, kMatrixWidth * kMatrixHeight - 1);
        
        int i = totalIndex % kMatrixWidth;
        int j = totalIndex / kMatrixWidth;
        
       /* // DEBUG: Show what coordinates we're sampling
        static int sampleCount = 0;
        if(sampleCount < 20) {
            Serial.print("pos="); Serial.print(position, 4);
            Serial.print(" totalIdx="); Serial.print(totalIndex);
            Serial.print(" (i,j)=("); Serial.print(i); Serial.print(","); Serial.print(j);
            Serial.print(") noise["); Serial.print(j); Serial.print("]["); Serial.print(i); Serial.print("]=");
            Serial.println(noise[j][i]);
            sampleCount++;
        }*/
        
        // Use EXACT mapping from original:
        // noise[j][i] for palette index, noise[i][j] for brightness
        uint8_t index = noise[j][i];
        uint8_t bri = noise[i][j];
        
        if(colorLoop) {
            index += ihue;
        }
        
        if(bri > 127) {
            bri = 255;
        } else {
            bri = dim8_raw(bri * 2);
        }
        
        CRGBPalette16* palette = PaletteRegistry::findByName(paletteName);
        if(palette == nullptr) {
            palette = PaletteRegistry::findByName("rainbow");
        }
        
        return ColorFromPalette(*palette, index, bri);
    }
    
    void setSpeed(uint16_t value) { speed = value; }
    void setScale(uint16_t value) { scale = value; }
    void setColorLoop(uint8_t value) { colorLoop = value; }
    void setPaletteName(String name) { paletteName = name; }
    String getPaletteName() const override { return paletteName; }
    void setPalette(String name) override { setPaletteName(name); }
    
    void setParameters(const std::vector<FunctionParameter>& params) override {
        if(params.size() >= 1) speed = (uint16_t)params[0].value;
        if(params.size() >= 2) scale = (uint16_t)params[1].value;
        if(params.size() >= 3) driftSpeed = params[2].value;
        if(params.size() >= 4) colorLoop = (uint8_t)params[3].value;
    }
    
    StatefulColorFunction* clone() const override {
        return new PerlinColorFunction(name, speed, scale, driftSpeed, colorLoop, paletteName);
    }
};

/////////////////////////////////////////
// COLOR FUNCTION REGISTRY
//

extern String colorFunctionNames[numcolorfunctions];
extern StatefulColorFunction* statefulColorFunctions[numcolorfunctions];
extern int numStatefulColorFunctions;

void registerStatefulColorFunction(int index, StatefulColorFunction* func);

/////////////////////////////////////////
// SIMPLE COLOR VIEWER
// Shows palette gradient from 0 to 1
//

class SimpleColorViewer : public StatefulColorFunction {
private:
    String paletteName;
    float pace;
    float phaseOffset;
    
public:
    SimpleColorViewer(String functionName = "simplecolor",
                     String palette = "rainbow",
                     float paceValue = 0.0)
        : StatefulColorFunction(functionName, 20),
          paletteName(palette),
          pace(paceValue),
          phaseOffset(0) {
    }
    
    void reset() override {
        phaseOffset = 0;
    }
    
    void updateState() override {
        if(pace > 0) {
            // pace = 1 means one complete palette roll (256 units) in Ãâ‚¬ seconds
            // Update interval is 20ms = 0.020s
            // Phase advance = (256 / Ãâ‚¬) * 0.020 * pace Ã¢â€°Ë† 1.630 * pace per update
            phaseOffset += 1.630 * pace;
            if(phaseOffset >= 256) phaseOffset -= 256;
        }
    }
    
    CRGB getColor(float position) override {
        // Get the assigned palette
        CRGBPalette16* palette = PaletteRegistry::findByName(paletteName);
        if(palette == nullptr) {
            palette = PaletteRegistry::findByName("rainbow");
        }
        
        // Map position (0-1) to palette index (0-255), add phase offset for rolling
        byte paletteIndex = ((int)(position * 255 + phaseOffset)) % 256;
        
        // Get color from palette
        CRGB color = ColorFromPalette(*palette, paletteIndex);
        
        // Scale to max brightness
        color.nscale8(MAXBRIGHTNESS);
        
        return color;
    }
    
    void setPaletteName(String name) { paletteName = name; }
    String getPaletteName() const override { return paletteName; }
    void setPalette(String name) override { setPaletteName(name); }
    void setPace(float value) { pace = value; }
    
    void setParameters(const std::vector<FunctionParameter>& params) override {
        if(params.size() >= 1) pace = params[0].value;
    }
    
    StatefulColorFunction* clone() const override {
        return new SimpleColorViewer(name, paletteName, pace);
    }
};

/////////////////////////////////////////
// SIMPLE WRAPPER CLASSES
// Minimal wrappers around stateless inline functions for factory registration
//

class DarkColorFunction : public StatefulColorFunction {
public:
    DarkColorFunction(String functionName = "dark")
        : StatefulColorFunction(functionName, 20) {}
    
    void reset() override {}
    void updateState() override {}
    
    CRGB getColor(float position) override {
        return CRGB(0, 0, 0);
    }
    
    StatefulColorFunction* clone() const override {
        return new DarkColorFunction(name);
    }
};

class RainbowColorFunction : public StatefulColorFunction {
private:
    float pace;
    float phaseOffset;
    
public:
    RainbowColorFunction(String functionName = "rainbow", float paceValue = 1.0)
        : StatefulColorFunction(functionName, 20), pace(paceValue), phaseOffset(0) {}
    
    void reset() override {
        phaseOffset = 0;
    }
    
    void updateState() override {
        // pace = 1 means one complete hue roll (256 units) in Ãâ‚¬ seconds
        // Update interval is 20ms = 0.020s
        // Phase advance = (256 / Ãâ‚¬) * 0.020 * pace Ã¢â€°Ë† 1.630 * pace per update
        phaseOffset += 1.630 * pace;
        if(phaseOffset >= 256) phaseOffset -= 256;
    }
    
    CRGB getColor(float position) override {
        int hue = ((int)(position * 255 + phaseOffset)) % 256;
        return CHSV(hue, 255, MAXBRIGHTNESS);
    }
    
    void setPace(float value) { pace = value; }
    
    void setParameters(const std::vector<FunctionParameter>& params) override {
        if(params.size() >= 1) pace = params[0].value;
    }
    
    StatefulColorFunction* clone() const override {
        return new RainbowColorFunction(name, pace);
    }
};

class BreathingColorFunction : public StatefulColorFunction {
private:
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    float pace;
    
public:
    BreathingColorFunction(String functionName = "breathing", 
                          uint8_t r = 255, 
                          uint8_t g = 255, 
                          uint8_t b = 255, 
                          float paceValue = 1.0)
        : StatefulColorFunction(functionName, 20), 
          red(r), green(g), blue(b), pace(paceValue) {}
    
    void reset() override {}
    void updateState() override {}
    
    CRGB getColor(float position) override {
        // pace scales the breathing speed
        // pace = 1.0 means one complete breath cycle in 2Ãâ‚¬ seconds
        float breathAmount = (sin(millis() / 1000.0 * pace) + 1.0) / 2.0;
        
        CRGB color(red, green, blue);
        color.nscale8(breathAmount * MAXBRIGHTNESS);
        
        return color;
    }
    
    void setPace(float value) { pace = value; }
    void setRGB(uint8_t r, uint8_t g, uint8_t b) { 
        red = r; green = g; blue = b; 
    }
    
    void setParameters(const std::vector<FunctionParameter>& params) override {
        if(params.size() >= 1) red = (uint8_t)params[0].value;
        if(params.size() >= 2) green = (uint8_t)params[1].value;
        if(params.size() >= 3) blue = (uint8_t)params[2].value;
        if(params.size() >= 4) pace = params[3].value;
    }
    
    StatefulColorFunction* clone() const override {
        return new BreathingColorFunction(name, red, green, blue, pace);
    }
};

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




inline CRGB callStatefulColorFunction(int index, float position) {
    if (index >= 0 && index < numcolorfunctions && 
        statefulColorFunctions[index] != nullptr) {
        statefulColorFunctions[index]->updateIfNeeded(millis());
        return statefulColorFunctions[index]->getColor(position);
    }
    return CRGB(0, 0, 0);
}

// Legacy inline function for backward compatibility (must come after callStatefulColorFunction)
inline CRGB simpleColor(float position) {
    return callStatefulColorFunction(12, position);
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

/////////////////////////////////////////
// COLOR FUNCTION REGISTRATION
//

// Register all color functions with the factory - MUST be called during setup()
void registerAllColorFunctions();

#endif  // COLORFUNCTIONS_H
