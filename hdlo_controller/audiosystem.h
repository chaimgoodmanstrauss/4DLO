////////////////////////////////////////////
//
//   audiosystem.h
//
// Centralized audio input system with FFT analysis
// Supports multiple audio sources: Microphone or SD Card playback
//

#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H

#include <Arduino.h>
#include <Audio.h>
#include <SD.h>

// FFT Band Constants
const int NUM_FFT_BANDS = 40;
const int FFT_LAST_BAND = 39;
const int BASS_BAND_START = 0;
const int BASS_BAND_END = 9;
const int MID_BAND_START = 10;
const int MID_BAND_END = 19;
const int TREBLE_BAND_START = 20;
const int TREBLE_BAND_END = 29;
const int HIGH_TREBLE_BAND_START = 30;
const int HIGH_TREBLE_BAND_END = 39;

// Audio Hardware Constants (SGTL5000)
const int MAX_MIC_GAIN = 63;
const int MAX_LINE_IN_LEVEL = 15;

// FFT Bin Mapping
const int LOW_BAND_BIN_MULTIPLIER = 5;
const int HIGH_BAND_BIN_OFFSET = 100;
const int HIGH_BAND_BIN_MULTIPLIER = 10;
const int HIGH_BAND_THRESHOLD = 20;

/////////////////////////////////////////
// AUDIO SOURCE INTERFACE
//
// Abstract base class for different audio sources
//

class AudioSource {
public:
    virtual ~AudioSource() {}
    
    // Check if audio data is available
    virtual bool available() = 0;
    
    // Update and cache FFT data
    virtual void update() = 0;
    
    // Get overall audio level (0.0 to 1.0+)
    virtual float getLevel() = 0;
    
    // Get peak level with hold
    virtual float getPeakLevel() = 0;
    
    // Get specific frequency band (0-39)
    virtual float getBand(int bandIndex) = 0;
    
    // Get raw FFT bin (0-511 for FFT1024)
    virtual float getBin(int binIndex) = 0;
    
    // Get average of a range of bands
    virtual float getBandRange(int startBand, int endBand) = 0;
    
    // Get bass level (average of low frequency bands)
    virtual float getBass() = 0;
    
    // Get mid level
    virtual float getMid() = 0;
    
    // Get treble level
    virtual float getTreble() = 0;
    
    // Get high treble level
    virtual float getHighTreble() = 0;
    
    // Get maximum band value (useful for audio detection)
    virtual float getMaxBand() = 0;
    
    // Print current levels (debugging)
    virtual void printLevels() = 0;
};

/////////////////////////////////////////
// MICROPHONE SOURCE
//
// Live microphone input with FFT analysis
//

class MicrophoneSource : public AudioSource {
private:
    static const int myInput = AUDIO_INPUT_MIC;
    
    AudioInputI2S audioInput;
    AudioAnalyzeFFT1024 myFFT;
    AudioOutputI2S audioOutput;
    AudioConnection patchCord1;
    AudioControlSGTL5000 audioShield;
    
    float cachedBands[NUM_FFT_BANDS];
    float cachedBins[512];       // Lazy cache for bins
    bool binCached[512];          // Track which bins are cached
    unsigned long cacheGeneration; // Invalidate cache each update
    unsigned long lastFFTUpdate;
    static const unsigned long FFT_UPDATE_INTERVAL = 20; // 20ms
    
    float currentLevel;
    float peakLevel;
    unsigned long lastPeakTime;
    static const unsigned long PEAK_HOLD_TIME = 300; // 300ms
    
    bool initialized;
    
public:
    MicrophoneSource();
    ~MicrophoneSource() {}
    
    void initialize();
    bool available() override;
    void update() override;
    
    float getLevel() override { return currentLevel; }
    float getPeakLevel() override { return peakLevel; }
    float getBand(int bandIndex) override;
    float getBin(int binIndex) override;
    float getBandRange(int startBand, int endBand) override;
    float getBass() override { return getBandRange(BASS_BAND_START, BASS_BAND_END); }
    float getMid() override { return getBandRange(MID_BAND_START, MID_BAND_END); }
    float getTreble() override { return getBandRange(TREBLE_BAND_START, TREBLE_BAND_END); }
    float getHighTreble() override { return getBandRange(HIGH_TREBLE_BAND_START, HIGH_TREBLE_BAND_END); }
    float getMaxBand();  // Get maximum band value (for audio detection)
    
    void setMicGain(float gain);
    void setLineInLevel(float level);
    void printLevels() override;
};

/////////////////////////////////////////
// SD CARD SOURCE
//
// Playback from SD card with variable speed and FFT analysis
//

class SDCardSource : public AudioSource {
private:
    AudioPlaySdWav wavPlayer;
    AudioAnalyzeFFT1024 myFFT;
    AudioOutputI2S audioOutput;
    AudioConnection patchCord1; // wavPlayer left -> FFT
    AudioConnection patchCord2; // wavPlayer left -> output left
    AudioConnection patchCord3; // wavPlayer right -> output right
    AudioControlSGTL5000 audioShield;
    
    float cachedBands[NUM_FFT_BANDS];
    float cachedBins[512];       // Lazy cache for bins
    bool binCached[512];          // Track which bins are cached
    unsigned long cacheGeneration; // Invalidate cache each update
    unsigned long lastFFTUpdate;
    static const unsigned long FFT_UPDATE_INTERVAL = 20; // 20ms
    
    float currentLevel;
    float peakLevel;
    unsigned long lastPeakTime;
    static const unsigned long PEAK_HOLD_TIME = 300;
    
    String currentFilename;
    bool initialized;
    bool isPlaying;
    bool loopPlayback;
    
public:
    SDCardSource();
    ~SDCardSource() {}
    
    void initialize();
    bool playFile(const char* filename);
    void setPlaybackRate(float rate); // Not supported - prints error message
    void setLooping(bool loop) { loopPlayback = loop; }
    void stop();
    void pause();
    void resume();
    bool isCurrentlyPlaying() { return isPlaying && wavPlayer.isPlaying(); }
    
    bool available() override;
    void update() override;
    
    float getLevel() override { return currentLevel; }
    float getPeakLevel() override { return peakLevel; }
    float getBand(int bandIndex) override;
    float getBin(int binIndex) override;
    float getBandRange(int startBand, int endBand) override;
    float getBass() override { return getBandRange(BASS_BAND_START, BASS_BAND_END); }
    float getMid() override { return getBandRange(MID_BAND_START, MID_BAND_END); }
    float getTreble() override { return getBandRange(TREBLE_BAND_START, TREBLE_BAND_END); }
    float getHighTreble() override { return getBandRange(HIGH_TREBLE_BAND_START, HIGH_TREBLE_BAND_END); }
    float getMaxBand();  // Get maximum band value (for audio detection)
    
    void printLevels() override;
};

/////////////////////////////////////////
// AUDIO SYSTEM (Main Interface)
//
// Facade that delegates to current audio source
//

class AudioSystem {
private:
    static AudioSource* currentSource;
    static MicrophoneSource* micSource;
    static SDCardSource* sdSource;
    static bool initialized;
    
public:
    // Initialize audio system (call in setup after AudioMemory)
    static void initialize();
    
    // Switch audio sources
    static void useMicrophone();
    static bool useSDCard(const char* filename, float rate = 1.0);
    
    // SD card controls
    static void setPlaybackRate(float rate);
    static void setLooping(bool loop);
    static void stopPlayback();
    static void pausePlayback();
    static void resumePlayback();
    static bool isPlaying();
    
    // Update audio data (call once per frame)
    static void update();
    
    // Audio level access (delegates to current source)
    static float getLevel() { return currentSource ? currentSource->getLevel() : 0.0f; }
    static float getPeakLevel() { return currentSource ? currentSource->getPeakLevel() : 0.0f; }
    static float getBand(int bandIndex) { return currentSource ? currentSource->getBand(bandIndex) : 0.0f; }
    static float getBin(int binIndex) { return currentSource ? currentSource->getBin(binIndex) : 0.0f; }
    static float getBandRange(int startBand, int endBand) { 
        return currentSource ? currentSource->getBandRange(startBand, endBand) : 0.0f; 
    }
    static float getBass() { return currentSource ? currentSource->getBass() : 0.0f; }
    static float getMid() { return currentSource ? currentSource->getMid() : 0.0f; }
    static float getTreble() { return currentSource ? currentSource->getTreble() : 0.0f; }
    static float getHighTreble() { return currentSource ? currentSource->getHighTreble() : 0.0f; }
    static float getMaxBand() { return currentSource ? currentSource->getMaxBand() : 0.0f; }
    
    // Convenience methods with alternative names
    static float getBassLevel() { return getBass(); }
    static float getMidLevel() { return getMid(); }
    static float getHighLevel() { return getHighTreble(); }
    static float getOverallLevel() { return getLevel(); }
    static bool getBeatDetected(); // Implemented in cpp file
    static float getFrequencyBand(int band) { return getBand(band); }
    
    // Microphone-specific controls
    static void setMicGain(float gain);
    static void setLineInLevel(float level);
    
    // Debugging
    static void printLevels();
    static bool isInitialized() { return initialized; }
    static String getCurrentSourceType();
};

#endif // AUDIOSYSTEM_H
