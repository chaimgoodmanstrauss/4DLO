////////////////////////////////////////////
//
//   audiosystem.cpp
//
// Implementation of the centralized audio system
//

#include "audiosystem.h"

// Static member definitions

// Audio components
AudioInputI2S AudioSystem::audioInput;
AudioAnalyzeFFT1024 AudioSystem::myFFT;
AudioOutputI2S AudioSystem::audioOutput;

// Audio connections - connect input to FFT analyzer
AudioConnection AudioSystem::patchCord1(audioInput, 0, myFFT, 0);

// Audio shield controller
AudioControlSGTL5000 AudioSystem::audioShield;

// State variables
bool AudioSystem::initialized = false;
float AudioSystem::cachedBands[40] = {0};
unsigned long AudioSystem::lastFFTUpdate = 0;
float AudioSystem::currentLevel = 0.0;
float AudioSystem::peakLevel = 0.0;
unsigned long AudioSystem::lastPeakTime = 0;
