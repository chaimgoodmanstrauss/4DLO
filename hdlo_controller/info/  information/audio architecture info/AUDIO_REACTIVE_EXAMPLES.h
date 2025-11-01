////////////////////////////////////////////
//
//   AUDIO_REACTIVE_EXAMPLES.h
//
// Examples of creating custom audio-reactive color functions
// using the centralized AudioSystem
//

#ifndef AUDIO_REACTIVE_EXAMPLES_H
#define AUDIO_REACTIVE_EXAMPLES_H

#include "colorfunctions.h"
#include "audiosystem.h"
#include "paletteregistry.h"

/////////////////////////////////////////
// EXAMPLE 1: FREQUENCY BAND VISUALIZER
// Maps different frequency bands to different positions
//

class FrequencyBandVisualizer : public StatefulColorFunction {
private:
    static const int NUM_LEDS = 128;
    static const int NUM_BANDS = 4;  // Bass, Low-mid, Mid, Treble
    
    float bandLevels[NUM_BANDS];
    String paletteName;
    
public:
    FrequencyBandVisualizer(String functionName = "freqbands",
                           String palette = "rainbow")
        : StatefulColorFunction(functionName, 20),
          paletteName(palette) {
        reset();
    }
    
    void reset() override {
        for(int i = 0; i < NUM_BANDS; i++) {
            bandLevels[i] = 0;
        }
    }
    
    void updateState() override {
        // Get frequency bands from AudioSystem
        bandLevels[0] = AudioSystem::getBass();        // 0-200 Hz
        bandLevels[1] = AudioSystem::getBandRange(10, 14);  // Low-mid
        bandLevels[2] = AudioSystem::getBandRange(15, 24);  // Mid
        bandLevels[3] = AudioSystem::getTreble();      // High
    }
    
    CRGB getColor(float position) override {
        // Divide LED strip into 4 zones
        int zone = (int)(position * NUM_BANDS);
        zone = constrain(zone, 0, NUM_BANDS - 1);
        
        // Get brightness for this zone
        byte brightness = bandLevels[zone] * 255 * 2;  // Scale up
        
        // Map to palette
        CRGBPalette16* palette = PaletteRegistry::findByName(paletteName);
        if(palette == nullptr) {
            palette = PaletteRegistry::findByName("rainbow");
        }
        
        byte paletteIndex = zone * (255 / NUM_BANDS);
        CRGB color = ColorFromPalette(*palette, paletteIndex);
        color.nscale8(brightness);
        
        return color;
    }
    
    void setPaletteName(String name) { paletteName = name; }
};

/////////////////////////////////////////
// EXAMPLE 2: BASS PULSE
// Pulses brightness based on bass frequencies
//

class BassPulseFunction : public StatefulColorFunction {
private:
    static const int NUM_LEDS = 128;
    
    float currentBass;
    float peakBass;
    unsigned long lastPeakTime;
    CRGB baseColor;
    
public:
    BassPulseFunction(String functionName = "basspulse",
                     CRGB base = CRGB::Blue)
        : StatefulColorFunction(functionName, 20),
          currentBass(0),
          peakBass(0),
          lastPeakTime(0),
          baseColor(base) {
        reset();
    }
    
    void reset() override {
        currentBass = 0;
        peakBass = 0;
        lastPeakTime = 0;
    }
    
    void updateState() override {
        // Get bass level
        currentBass = AudioSystem::getBass();
        
        // Track peaks
        if(currentBass > peakBass) {
            peakBass = currentBass;
            lastPeakTime = millis();
        }
        
        // Decay peak
        if(millis() - lastPeakTime > 200) {
            peakBass *= 0.9;
        }
    }
    
    CRGB getColor(float position) override {
        // Scale brightness based on bass level
        byte brightness = constrain(currentBass * 2550, 0, 255);
        
        CRGB color = baseColor;
        color.nscale8(brightness);
        
        // Add white flash on peak
        if(currentBass > peakBass * 0.9) {
            color += CRGB(brightness/2, brightness/2, brightness/2);
        }
        
        return color;
    }
    
    void setBaseColor(CRGB color) { baseColor = color; }
};

/////////////////////////////////////////
// EXAMPLE 3: SPECTRUM ANALYZER
// Classic spectrum analyzer effect
//

class SpectrumAnalyzer : public StatefulColorFunction {
private:
    static const int NUM_LEDS = 128;
    static const int NUM_BANDS = 20;  // More bands for detailed display
    
    float bandHeights[NUM_BANDS];
    float peakHeights[NUM_BANDS];
    unsigned long peakTimes[NUM_BANDS];
    
public:
    SpectrumAnalyzer(String functionName = "spectrum")
        : StatefulColorFunction(functionName, 20) {
        reset();
    }
    
    void reset() override {
        for(int i = 0; i < NUM_BANDS; i++) {
            bandHeights[i] = 0;
            peakHeights[i] = 0;
            peakTimes[i] = 0;
        }
    }
    
    void updateState() override {
        // Sample 20 bands across the spectrum
        for(int i = 0; i < NUM_BANDS; i++) {
            // Map band to frequency range
            int bandIndex = i * 2;  // Spread across 40 available bands
            float level = AudioSystem::getBand(bandIndex);
            
            // Smooth the height
            bandHeights[i] = bandHeights[i] * 0.7 + level * 0.3;
            
            // Track peaks
            if(bandHeights[i] > peakHeights[i]) {
                peakHeights[i] = bandHeights[i];
                peakTimes[i] = millis();
            }
            
            // Decay peaks
            if(millis() - peakTimes[i] > 300) {
                peakHeights[i] *= 0.95;
            }
        }
    }
    
    CRGB getColor(float position) override {
        // Determine which band this LED is in
        int band = (int)(position * NUM_BANDS);
        band = constrain(band, 0, NUM_BANDS - 1);
        
        // Position within band (0.0 to 1.0)
        float bandPos = fmod(position * NUM_BANDS, 1.0);
        
        CRGB color = CRGB::Black;
        
        // Check if this position should be lit
        if(bandPos < bandHeights[band]) {
            // Color gradient based on height
            if(bandPos < 0.33) {
                color = CRGB::Green;
            } else if(bandPos < 0.66) {
                color = CRGB::Yellow;
            } else {
                color = CRGB::Red;
            }
        }
        
        // Add white peak indicator
        if(abs(bandPos - peakHeights[band]) < 0.05) {
            color = CRGB::White;
        }
        
        return color;
    }
};

/////////////////////////////////////////
// EXAMPLE 4: BEAT DETECTOR
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
};

/////////////////////////////////////////
// EXAMPLE 5: VOCAL HIGHLIGHTER
// Responds specifically to vocal/mid frequencies
//

class VocalHighlighter : public StatefulColorFunction {
private:
    static const int NUM_LEDS = 128;
    
    byte brightness[NUM_LEDS];
    float vocalLevel;
    float decay;
    CRGB vocalColor;
    
public:
    VocalHighlighter(String functionName = "vocals",
                    float decayRate = 0.9,
                    CRGB color = CRGB::Cyan)
        : StatefulColorFunction(functionName, 20),
          vocalLevel(0),
          decay(decayRate),
          vocalColor(color) {
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
        
        CRGB color = vocalColor;
        color.nscale8(brightness[index]);
        
        return color;
    }
    
    void setDecay(float d) { decay = constrain(d, 0.5, 0.99); }
    void setVocalColor(CRGB color) { vocalColor = color; }
};

/////////////////////////////////////////
// HOW TO USE THESE EXAMPLES
//

/*
// 1. Register your custom function in colorfunctions.cpp

void initializeStatefulColorFunctions() {
    // ... existing functions ...
    
    // Add your custom function
    FrequencyBandVisualizer* freqViz = new FrequencyBandVisualizer("freqbands", "rainbow");
    registerStatefulColorFunction(15, freqViz);
    
    BassPulseFunction* bassPulse = new BassPulseFunction("basspulse", CRGB::Purple);
    registerStatefulColorFunction(16, bassPulse);
    
    SpectrumAnalyzer* spectrum = new SpectrumAnalyzer("spectrum");
    registerStatefulColorFunction(17, spectrum);
    
    BeatDetector* beatDetect = new BeatDetector("beatdetect", "lava");
    registerStatefulColorFunction(18, beatDetect);
    
    VocalHighlighter* vocals = new VocalHighlighter("vocals", 0.92, CRGB::Aqua);
    registerStatefulColorFunction(19, vocals);
}

// 2. Add wrapper functions in colorfunctions.cpp

CRGB freqBandsWrapper(float position) {
    return callStatefulColorFunction(15, position);
}

CRGB bassPulseWrapper(float position) {
    return callStatefulColorFunction(16, position);
}

// ... etc for other functions ...

// 3. Add to colorFunctionArray

ColorFunction colorFunctionArray[numcolorfunctions] = {
    // ... existing functions 0-14 ...
    staticgreen,           // 15
    pulsingred,           // 16
    freqBandsWrapper,     // 17
    bassPulseWrapper,     // 18
    spectrumWrapper,      // 19
    beatDetectWrapper,    // 20
    vocalsWrapper,        // 21
    // ...
};

// 4. Add names

String colorFunctionNames[numcolorfunctions] = {
    // ... existing names ...
    "freqbands",
    "basspulse",
    "spectrum",
    "beatdetect",
    "vocals",
    // ...
};

// 5. Use in sequences (sequences.cpp)

seq->addStep("flowoctahedron", {"dark", "freqbands", "basspulse", "spectrum"}, 10, FADE, 2);
seq->addStep("cycle", {"dark", "beatdetect", "vocals", "freqbands"}, 10, FADE, 2);

// 6. All functions automatically use the shared AudioSystem!
//    - No need to initialize audio hardware per function
//    - All functions see the same FFT data
//    - Efficient and synchronized
*/

#endif // AUDIO_REACTIVE_EXAMPLES_H
