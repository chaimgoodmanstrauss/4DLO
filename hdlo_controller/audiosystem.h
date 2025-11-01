////////////////////////////////////////////
//
//   audiosystem.h
//
// Centralized audio input system with FFT analysis
// Provides audio data to all audio-responsive color functions
//

#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H

#include <Arduino.h>
#include <Audio.h>

/////////////////////////////////////////
// AUDIO SYSTEM CLASS
//
// Singleton class that manages audio input and FFT analysis
// All audio-responsive functions should use this instead of
// creating their own audio objects
//

class AudioSystem {
private:
    // Audio input selection
    static const int myInput = AUDIO_INPUT_MIC;
    
    // Audio components - created in data flow order:
    // inputs/sources -> processing -> outputs
    static AudioInputI2S audioInput;          // Audio shield: mic or line-in
    static AudioAnalyzeFFT1024 myFFT;         // FFT analyzer
    static AudioOutputI2S audioOutput;        // Audio shield: headphones & line-out
    
    // Audio connections
    static AudioConnection patchCord1;
    
    // Audio shield controller
    static AudioControlSGTL5000 audioShield;
    
    // Initialization flag
    static bool initialized;
    
    // Cached FFT data for efficiency
    static float cachedBands[40];             // Store 40 frequency bands
    static unsigned long lastFFTUpdate;       // Track when we last updated FFT
    static const unsigned long FFT_UPDATE_INTERVAL = 20; // Update every 20ms
    
    // Overall audio level (RMS-like value)
    static float currentLevel;
    static float peakLevel;
    static unsigned long lastPeakTime;
    static const unsigned long PEAK_HOLD_TIME = 300; // Hold peak for 300ms
    
public:
    // Initialize the audio system
    static void initialize() {
        if(initialized) {
            Serial.println("Audio system already initialized");
            return;
        }
        
        Serial.println("Initializing audio system...");
        
        // NOTE: AudioMemory() MUST be called in setup() before this!
        
        // Enable and configure audio shield
        audioShield.enable();
        audioShield.inputSelect(myInput);
        audioShield.volume(0.5);  // Set headphone volume
        
        // Initialize cached data
        for(int i = 0; i < 40; i++) {
            cachedBands[i] = 0.0;
        }
        
        currentLevel = 0.0;
        peakLevel = 0.0;
        lastFFTUpdate = 0;
        lastPeakTime = 0;
        
        initialized = true;
        Serial.println("Audio system initialized with FFT1024");
        Serial.println("  - Input: Microphone");
        Serial.println("  - FFT bands: 40");
        Serial.println("  - Update interval: " + String(FFT_UPDATE_INTERVAL) + "ms");
    }
    
    // Update FFT data (call once per frame before using audio data)
    static void update() {
        if(!initialized) {
            Serial.println("Warning: Audio system not initialized!");
            return;
        }
        
        unsigned long currentTime = millis();
        
        // Only update if enough time has passed
        if(currentTime - lastFFTUpdate < FFT_UPDATE_INTERVAL) {
            return;
        }
        
        // Check if FFT data is available
        if(myFFT.available()) {
            lastFFTUpdate = currentTime;
            
            // Read frequency bands
            // FFT1024 gives us 512 frequency bins, we'll sample 40 of them
            float sum = 0.0;
            
            for(int i = 0; i < 40; i++) {
                // Map 40 bands across the useful frequency range
                // Lower frequencies get more resolution
                int binIndex;
                if(i < 20) {
                    // First 20 bands: bins 0-100 (more detail in bass)
                    binIndex = i * 5;
                } else {
                    // Next 20 bands: bins 100-300 (less detail in treble)
                    binIndex = 100 + (i - 20) * 10;
                }
                
                cachedBands[i] = myFFT.read(binIndex);
                sum += cachedBands[i];
            }
            
            // Calculate overall level (average of bands)
            currentLevel = sum / 40.0;
            
            // Update peak with hold time
            if(currentLevel > peakLevel) {
                peakLevel = currentLevel;
                lastPeakTime = currentTime;
            } else if(currentTime - lastPeakTime > PEAK_HOLD_TIME) {
                // Decay peak slowly
                peakLevel *= 0.95;
            }
        }
    }
    
    // Get overall audio level (0.0 to 1.0+)
    static float getLevel() {
        return currentLevel;
    }
    
    // Get peak level with hold
    static float getPeakLevel() {
        return peakLevel;
    }
    
    // Get specific frequency band (0-39)
    // Band 0-9: Bass (20-200 Hz)
    // Band 10-19: Low-mid (200-1000 Hz)
    // Band 20-29: Mid (1-3 kHz)
    // Band 30-39: High (3-10 kHz)
    static float getBand(int bandIndex) {
        if(bandIndex >= 0 && bandIndex < 40) {
            return cachedBands[bandIndex];
        }
        return 0.0;
    }
    
    // Get average of a range of bands
    static float getBandRange(int startBand, int endBand) {
        if(startBand < 0) startBand = 0;
        if(endBand >= 40) endBand = 39;
        if(startBand > endBand) return 0.0;
        
        float sum = 0.0;
        int count = 0;
        
        for(int i = startBand; i <= endBand; i++) {
            sum += cachedBands[i];
            count++;
        }
        
        return (count > 0) ? (sum / count) : 0.0;
    }
    
    // Get bass level (average of low frequency bands)
    static float getBass() {
        return getBandRange(0, 9);
    }
    
    // Get mid level
    static float getMid() {
        return getBandRange(10, 19);
    }
    
    // Get treble level
    static float getTreble() {
        return getBandRange(20, 29);
    }
    
    // Get high treble level
    static float getHighTreble() {
        return getBandRange(30, 39);
    }
    
    // Check if audio system is initialized
    static bool isInitialized() {
        return initialized;
    }
    
    // Set microphone gain (0.0 to 1.0)
    static void setMicGain(float gain) {
        if(!initialized) return;
        gain = constrain(gain, 0.0, 1.0);
        audioShield.micGain(gain * 63); // 0-63 range for SGTL5000
        Serial.println("Microphone gain set to: " + String(gain));
    }
    
    // Set line input level (0.0 to 1.0)
    static void setLineInLevel(float level) {
        if(!initialized) return;
        level = constrain(level, 0.0, 1.0);
        audioShield.lineInLevel(level * 15); // 0-15 range for SGTL5000
        Serial.println("Line input level set to: " + String(level));
    }
    
    // Switch input source
    static void setInputSource(int input) {
        if(!initialized) return;
        audioShield.inputSelect(input);
        Serial.println("Audio input source changed");
    }
    
    // Print current audio levels (for debugging)
    static void printLevels() {
        Serial.println("Audio Levels:");
        Serial.println("  Overall: " + String(currentLevel, 4));
        Serial.println("  Peak: " + String(peakLevel, 4));
        Serial.println("  Bass: " + String(getBass(), 4));
        Serial.println("  Mid: " + String(getMid(), 4));
        Serial.println("  Treble: " + String(getTreble(), 4));
        Serial.println("  High: " + String(getHighTreble(), 4));
    }
};

#endif // AUDIOSYSTEM_H
