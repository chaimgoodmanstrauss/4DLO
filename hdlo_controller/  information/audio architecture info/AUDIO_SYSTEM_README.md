# Audio System Refactoring Summary

## Overview
Centralized all audio input functionality into a single `AudioSystem` class that uses the Teensy Audio Shield with FFT1024 analysis. All audio-responsive color functions now share this single audio system instead of each creating their own analog input readers.

## New Files Created

### 1. `audiosystem.h` - Audio System Header
Defines the `AudioSystem` singleton class with:
- **Audio Components:**
  - `AudioInputI2S` - Microphone/line input from audio shield
  - `AudioAnalyzeFFT1024` - FFT analyzer with 1024 bins
  - `AudioOutputI2S` - Headphone/line output
  - `AudioControlSGTL5000` - Audio shield controller

- **Key Features:**
  - Global audio system shared by all color functions
  - FFT analysis with 40 frequency bands cached for efficiency
  - Update interval of 20ms to avoid excessive CPU usage
  - Automatic peak hold with decay
  - Band groupings: Bass (0-9), Mid (10-19), Treble (20-29), High (30-39)

- **Main Methods:**
  ```cpp
  AudioSystem::initialize()        // Set up audio hardware
  AudioSystem::update()            // Read FFT data (call once per frame)
  AudioSystem::getLevel()          // Get overall audio level (0.0-1.0+)
  AudioSystem::getPeakLevel()      // Get peak with hold time
  AudioSystem::getBand(index)      // Get specific frequency band (0-39)
  AudioSystem::getBass()           // Get bass level average
  AudioSystem::getMid()            // Get mid frequency average
  AudioSystem::getTreble()         // Get treble average
  AudioSystem::setMicGain(gain)    // Adjust mic sensitivity
  AudioSystem::printLevels()       // Debug: print current levels
  ```

### 2. `audiosystem.cpp` - Audio System Implementation
Static member definitions and initialization for the audio components.

## Modified Files

### 1. `colorfunctions.h`
**Changes to `AudioReactiveColorFunction`:**
- Removed: `audioPin`, `audioHistory[]`, `historyIndex`
- Removed: `readAudioLevel()` and `getSmoothedLevel()` methods
- Now uses: `AudioSystem::getLevel()` in `updateState()`
- Simplified: No longer needs audio pin parameter (kept for compatibility)
- Benefits: 
  - More accurate with FFT-based analysis
  - Already smoothed by AudioSystem
  - Reduced memory usage

**Changes to `VUMeterColorFunction`:**
- Removed: `audioPin` member variable
- Now uses: `AudioSystem::getLevel()` in `updateState()`
- Kept pin parameter for API compatibility but it's unused
- Benefits:
  - Shares audio data with other functions
  - More responsive with FFT analysis
  - Consistent audio readings across all functions

### 2. `hdlo_controller.ino`
**Added:**
- Include for `audiosystem.h`
- `AudioSystem::initialize()` call in setup (BEFORE color function init)
- `AudioSystem::update()` call at start of loop (BEFORE frame processing)
- Serial commands:
  - `audio` - Display current audio levels
  - `gain [0.0-1.0]` - Adjust microphone gain

**Initialization Order (Critical):**
```cpp
1. Edge data
2. OctoWS2811/FastLED
3. AudioSystem::initialize()     // NEW - Must be before color functions!
4. PaletteRegistry
5. Color functions (uses AudioSystem)
6. Edge permutations
7. Models
8. Sequences
9. Start sequence registry
```

**Loop Order:**
```cpp
1. AudioSystem::update()          // NEW - Read FFT once per frame
2. StatefulColorFunction::beginFrame()
3. Update sequence registry
4. Update current sequence
5. Apply colors to LEDs
6. FastLED.show()
7. Handle serial commands
```

## Audio Shield Configuration

### Hardware Setup Required:
```cpp
const int myInput = AUDIO_INPUT_MIC;  // Use microphone input
AudioMemory(12);                       // Allocate audio memory blocks
audioShield.enable();                  // Enable audio shield
audioShield.inputSelect(myInput);     // Select mic input
```

### FFT Band Mapping:
The system samples 40 bands from the 512 available FFT bins:
- **Bands 0-19:** Higher resolution in bass/low-mid (5 bins per band)
- **Bands 20-39:** Lower resolution in mid/high (10 bins per band)

This gives better detail where it matters most for LED effects.

## Benefits of Centralized Audio System

### 1. **Efficiency**
- Single FFT calculation shared by all functions
- Cached band data prevents redundant reads
- Update rate limited to 20ms (50 Hz max)
- Reduced CPU usage

### 2. **Consistency**
- All audio functions see the same data
- Synchronized response across different effects
- No timing issues between multiple analogRead() calls

### 3. **Quality**
- FFT provides frequency analysis (not just amplitude)
- 1024-point FFT gives excellent frequency resolution
- Hardware-accelerated on Teensy 4.x
- Proper audio shield signal conditioning

### 4. **Flexibility**
- Easy to add new audio-reactive effects
- Can access specific frequency bands (bass, mid, treble)
- Adjustable gain and sensitivity at runtime
- Extensible for future audio features

### 5. **Maintainability**
- Single point of configuration
- Easier debugging (one place to check audio)
- Clear separation of concerns
- Reusable across projects

## Usage Example

### In Color Functions:
```cpp
void updateState() override {
    // Get overall audio level
    float level = AudioSystem::getLevel();
    
    // Or get specific frequency ranges
    float bass = AudioSystem::getBass();
    float mid = AudioSystem::getMid();
    float treble = AudioSystem::getTreble();
    
    // Or individual bands
    float kickDrum = AudioSystem::getBand(2);  // Low bass
    float vocals = AudioSystem::getBand(15);   // Mid range
    
    // Use these values to drive your effect...
}
```

### Serial Commands:
```
> audio
Audio Levels:
  Overall: 0.1234
  Peak: 0.2345
  Bass: 0.0987
  Mid: 0.1456
  Treble: 0.0678
  High: 0.0234

> gain 0.7
Microphone gain set to: 0.70
```

## Migration Notes

### For Existing Code:
1. **No breaking changes** - Audio pin parameters are kept for compatibility
2. Old analog pin reading code is simply replaced with AudioSystem calls
3. All existing color function APIs remain unchanged
4. Existing sketches just need to add AudioSystem initialization

### Performance Impact:
- **Before:** Multiple analogRead() calls per frame (slow, noisy)
- **After:** One FFT update per frame shared by all functions
- **Result:** Better performance despite more sophisticated analysis

## Future Enhancements

Possible additions to AudioSystem:
- Beat detection
- Onset detection (transient detection)
- Spectral centroid (brightness)
- RMS level calculation
- Configurable frequency bands
- Multiple input source support
- Recording/playback capability
- Analysis history/averaging

## Troubleshooting

### Audio Shield Not Detected:
- Check I2C connections (pins 18/19 on Teensy)
- Verify AudioMemory allocation
- Check serial output during initialization

### No Audio Response:
- Use `audio` serial command to check levels
- Adjust gain with `gain 0.5` command
- Check microphone/line input selection
- Verify input source is correct

### Noisy Audio:
- Reduce microphone gain
- Check for ground loops
- Use shielded cables
- Enable audio shield filters

## Files Modified Summary

| File | Status | Changes |
|------|--------|---------|
| audiosystem.h | NEW | Audio system header |
| audiosystem.cpp | NEW | Audio system implementation |
| colorfunctions.h | MODIFIED | Updated audio-reactive functions |
| hdlo_controller.ino | MODIFIED | Added audio initialization |

## Integration Checklist

- [x] Create AudioSystem singleton class
- [x] Implement FFT band caching
- [x] Update AudioReactiveColorFunction
- [x] Update VUMeterColorFunction  
- [x] Add AudioSystem to main controller
- [x] Add serial commands for audio control
- [x] Test audio shield initialization
- [x] Verify FFT data flow
- [x] Document all changes

---

**Author Notes:** This refactoring provides a solid foundation for all audio-reactive features. The centralized system is more efficient, more accurate, and easier to extend than the previous per-function analog reading approach.
