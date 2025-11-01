////////////////////////////////////////////
//
//   audiosystem.cpp
//
// Implementation of polymorphic audio system
//

#include "audiosystem.h"

/////////////////////////////////////////
// MICROPHONE SOURCE IMPLEMENTATION
//

MicrophoneSource::MicrophoneSource()
    : audioInput(),
      myFFT(),
      audioOutput(),
      patchCord1(audioInput, 0, myFFT, 0),
      audioShield(),
      lastFFTUpdate(0),
      currentLevel(0.0),
      peakLevel(0.0),
      lastPeakTime(0),
      initialized(false) {
    for(int i = 0; i < 40; i++) {
        cachedBands[i] = 0.0;
    }
}

void MicrophoneSource::initialize() {
    if(initialized) return;
    
    audioShield.enable();
    audioShield.inputSelect(myInput);
    audioShield.volume(0.5);
    
    initialized = true;
    Serial.println("Microphone source initialized");
}

bool MicrophoneSource::available() {
    return myFFT.available();
}

void MicrophoneSource::update() {
    unsigned long currentTime = millis();
    
    if(currentTime - lastFFTUpdate < FFT_UPDATE_INTERVAL) {
        return;
    }
    
    if(myFFT.available()) {
        lastFFTUpdate = currentTime;
        
        float sum = 0.0;
        for(int i = 0; i < 40; i++) {
            int binIndex = (i < 20) ? (i * 5) : (100 + (i - 20) * 10);
            cachedBands[i] = myFFT.read(binIndex);
            sum += cachedBands[i];
        }
        
        currentLevel = sum / 40.0;
        
        if(currentLevel > peakLevel) {
            peakLevel = currentLevel;
            lastPeakTime = currentTime;
        } else if(currentTime - lastPeakTime > PEAK_HOLD_TIME) {
            peakLevel *= 0.95;
        }
    }
}

float MicrophoneSource::getBand(int bandIndex) {
    if(bandIndex >= 0 && bandIndex < 40) {
        return cachedBands[bandIndex];
    }
    return 0.0;
}

float MicrophoneSource::getBandRange(int startBand, int endBand) {
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

void MicrophoneSource::setMicGain(float gain) {
    if(!initialized) return;
    gain = constrain(gain, 0.0, 1.0);
    audioShield.micGain(gain * 63);
    Serial.println("Mic gain: " + String(gain));
}

void MicrophoneSource::setLineInLevel(float level) {
    if(!initialized) return;
    level = constrain(level, 0.0, 1.0);
    audioShield.lineInLevel(level * 15);
    Serial.println("Line in level: " + String(level));
}

void MicrophoneSource::printLevels() {
    Serial.println("Microphone Levels:");
    Serial.println("  Overall: " + String(currentLevel, 4));
    Serial.println("  Peak: " + String(peakLevel, 4));
    Serial.println("  Bass: " + String(getBass(), 4));
    Serial.println("  Mid: " + String(getMid(), 4));
    Serial.println("  Treble: " + String(getTreble(), 4));
}

/////////////////////////////////////////
// SD CARD SOURCE IMPLEMENTATION
//

SDCardSource::SDCardSource()
    : wavPlayer(),
      myFFT(),
      audioOutput(),
      patchCord1(wavPlayer, 0, myFFT, 0),
      patchCord2(wavPlayer, 0, audioOutput, 0),
      patchCord3(wavPlayer, 1, audioOutput, 1),
      audioShield(),
      lastFFTUpdate(0),
      currentLevel(0.0),
      peakLevel(0.0),
      lastPeakTime(0),
      playbackRate(1.0),
      initialized(false),
      isPlaying(false),
      loopPlayback(false) {
    for(int i = 0; i < 40; i++) {
        cachedBands[i] = 0.0;
    }
}

void SDCardSource::initialize() {
    if(initialized) return;
    
    audioShield.enable();
    audioShield.volume(0.5);
    
    if(!SD.begin(BUILTIN_SDCARD)) {
        Serial.println("SD card init failed!");
        return;
    }
    
    initialized = true;
    Serial.println("SD card source initialized");
}

bool SDCardSource::playFile(const char* filename) {
    if(!initialized) {
        Serial.println("SD source not initialized!");
        return false;
    }
    
    if(wavPlayer.isPlaying()) {
        wavPlayer.stop();
    }
    
    if(wavPlayer.play(filename)) {
        currentFilename = String(filename);
        isPlaying = true;
        Serial.println("Playing: " + currentFilename);
        return true;
    }
    
    Serial.println("Failed to play: " + String(filename));
    return false;
}

void SDCardSource::setPlaybackRate(float rate) {
    playbackRate = constrain(rate, 0.01, 4.0);
    // Note: AudioPlaySdWav does not support variable playback rate
    // This would require using AudioPlaySdResmp instead
    Serial.println("Warning: Playback rate adjustment not supported by AudioPlaySdWav");
    Serial.println("Requested rate: " + String(playbackRate * 100) + "% (ignored)");
}

void SDCardSource::stop() {
    wavPlayer.stop();
    isPlaying = false;
    Serial.println("Playback stopped");
}

void SDCardSource::pause() {
    wavPlayer.stop();
    Serial.println("Playback paused");
}

void SDCardSource::resume() {
    if(currentFilename.length() > 0) {
        playFile(currentFilename.c_str());
    }
}

bool SDCardSource::available() {
    return myFFT.available();
}

void SDCardSource::update() {
    // Handle looping
    if(loopPlayback && isPlaying && !wavPlayer.isPlaying()) {
        playFile(currentFilename.c_str());
    }
    
    unsigned long currentTime = millis();
    
    if(currentTime - lastFFTUpdate < FFT_UPDATE_INTERVAL) {
        return;
    }
    
    if(myFFT.available()) {
        lastFFTUpdate = currentTime;
        
        float sum = 0.0;
        for(int i = 0; i < 40; i++) {
            int binIndex = (i < 20) ? (i * 5) : (100 + (i - 20) * 10);
            cachedBands[i] = myFFT.read(binIndex);
            sum += cachedBands[i];
        }
        
        currentLevel = sum / 40.0;
        
        if(currentLevel > peakLevel) {
            peakLevel = currentLevel;
            lastPeakTime = currentTime;
        } else if(currentTime - lastPeakTime > PEAK_HOLD_TIME) {
            peakLevel *= 0.95;
        }
    }
}

float SDCardSource::getBand(int bandIndex) {
    if(bandIndex >= 0 && bandIndex < 40) {
        return cachedBands[bandIndex];
    }
    return 0.0;
}

float SDCardSource::getBandRange(int startBand, int endBand) {
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

void SDCardSource::printLevels() {
    Serial.println("SD Card Playback Levels:");
    Serial.println("  File: " + currentFilename);
    Serial.println("  Rate: " + String(playbackRate * 100) + "%");
    Serial.println("  Playing: " + String(isPlaying ? "Yes" : "No"));
    Serial.println("  Overall: " + String(currentLevel, 4));
    Serial.println("  Peak: " + String(peakLevel, 4));
    Serial.println("  Bass: " + String(getBass(), 4));
    Serial.println("  Mid: " + String(getMid(), 4));
    Serial.println("  Treble: " + String(getTreble(), 4));
}

/////////////////////////////////////////
// AUDIO SYSTEM STATIC MEMBERS
//

AudioSource* AudioSystem::currentSource = nullptr;
MicrophoneSource* AudioSystem::micSource = nullptr;
SDCardSource* AudioSystem::sdSource = nullptr;
bool AudioSystem::initialized = false;

/////////////////////////////////////////
// AUDIO SYSTEM IMPLEMENTATION
//

void AudioSystem::initialize() {
    if(initialized) {
        Serial.println("AudioSystem already initialized");
        return;
    }
    
    Serial.println("Initializing AudioSystem...");
    
    // Create microphone source (default)
    micSource = new MicrophoneSource();
    micSource->initialize();
    currentSource = micSource;
    
    initialized = true;
    Serial.println("AudioSystem initialized (Microphone)");
}

void AudioSystem::useMicrophone() {
    if(!initialized) {
        Serial.println("AudioSystem not initialized!");
        return;
    }
    
    if(!micSource) {
        micSource = new MicrophoneSource();
        micSource->initialize();
    }
    
    currentSource = micSource;
    Serial.println("Switched to Microphone");
}

bool AudioSystem::useSDCard(const char* filename, float rate) {
    if(!initialized) {
        Serial.println("AudioSystem not initialized!");
        return false;
    }
    
    if(!sdSource) {
        sdSource = new SDCardSource();
        sdSource->initialize();
    }
    
    if(sdSource->playFile(filename)) {
        sdSource->setPlaybackRate(rate);
        currentSource = sdSource;
        Serial.println("Switched to SD Card");
        return true;
    }
    
    return false;
}

void AudioSystem::setPlaybackRate(float rate) {
    if(sdSource) {
        sdSource->setPlaybackRate(rate);
    }
}

void AudioSystem::setLooping(bool loop) {
    if(sdSource) {
        sdSource->setLooping(loop);
    }
}

void AudioSystem::stopPlayback() {
    if(sdSource) {
        sdSource->stop();
    }
}

void AudioSystem::pausePlayback() {
    if(sdSource) {
        sdSource->pause();
    }
}

void AudioSystem::resumePlayback() {
    if(sdSource) {
        sdSource->resume();
    }
}

bool AudioSystem::isPlaying() {
    return sdSource ? sdSource->isCurrentlyPlaying() : false;
}

void AudioSystem::update() {
    if(currentSource) {
        currentSource->update();
    }
}

void AudioSystem::setMicGain(float gain) {
    if(micSource) {
        micSource->setMicGain(gain);
    }
}

void AudioSystem::setLineInLevel(float level) {
    if(micSource) {
        micSource->setLineInLevel(level);
    }
}

void AudioSystem::printLevels() {
    if(currentSource) {
        currentSource->printLevels();
    }
}

String AudioSystem::getCurrentSourceType() {
    if(currentSource == micSource) return "Microphone";
    if(currentSource == sdSource) return "SD Card";
    return "None";
}

bool AudioSystem::getBeatDetected() {
    // Simple beat detection based on bass level and peak
    if(!currentSource) return false;
    
    float bass = currentSource->getBass();
    float peak = currentSource->getPeakLevel();
    
    // Detect beat if bass is close to peak (indicating strong low-frequency hit)
    return (bass > 0.3 && bass > peak * 0.8);
}
