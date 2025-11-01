# Quick Start: Integrating the Audio System

## Step 1: Add the Files to Your Project

Copy these files to your Arduino project directory:
```
audiosystem.h
audiosystem.cpp
colorfunctions.h (updated version)
hdlo_controller.ino (updated version)
```

## Step 2: Hardware Setup

Connect your Teensy Audio Shield:
- Ensure I2C pins (18/19 on Teensy 4.x) are connected
- Connect microphone or line input to the audio shield
- Verify power connections

## Step 3: Update Your Main Sketch

### Add the include:
```cpp
#include "audiosystem.h"
```

### Update setup():
```cpp
void setup() {
    Serial.begin(115200);
    
    // ... your existing setup ...
    
    // ADD THIS BEFORE initializing color functions:
    Serial.println("Initializing audio system...");
    AudioSystem::initialize();
    
    // Then initialize color functions as usual:
    initializeStatefulColorFunctions();
    
    // ... rest of setup ...
}
```

### Update loop():
```cpp
void loop() {
    // ADD THIS AT THE VERY START:
    AudioSystem::update();
    
    // Then continue with your normal loop:
    StatefulColorFunction::beginFrame();
    // ... rest of loop ...
}
```

## Step 4: Test It!

1. Upload to your Teensy
2. Open Serial Monitor (115200 baud)
3. You should see:
   ```
   Initializing audio system...
   Audio system initialized with FFT1024
     - Input: Microphone
     - FFT bands: 40
     - Update interval: 20ms
   ```

## Step 5: Test Audio Response

In Serial Monitor, type:
```
audio
```

You should see output like:
```
Audio Levels:
  Overall: 0.1234
  Peak: 0.2345
  Bass: 0.0987
  Mid: 0.1456
  Treble: 0.0678
  High: 0.0234
```

If levels are too low, adjust gain:
```
gain 0.7
```

## Step 6: Enable Audio Sequences

In your sequences file, make sure you have sequences that use audio functions:
```cpp
seq->addStep("flowoctahedron", {"dark", "audio", "audio", "audio"}, 10, FADE, 2);
seq->addStep("cycle", {"dark", "vumeter", "vumeter", "rainbow"}, 10, FADE, 2);
```

## Troubleshooting

### No audio response:
1. Check `audio` command output for levels
2. Adjust gain: `gain 0.8`
3. Verify audio shield connections
4. Check microphone/input source

### Compilation errors:
1. Ensure Audio library is installed (Teensy Audio)
2. Check that all files are in the project directory
3. Verify includes are in correct order

### Audio shield not detected:
1. Check I2C connections (pins 18/19)
2. Verify AudioMemory(12) is called
3. Check serial output during initialization

## That's It!

Your audio-responsive color functions now use the centralized audio system with FFT analysis. All functions automatically share the same audio data for better performance and consistency.

## Next Steps

- Experiment with different gain settings
- Try the example audio-reactive functions in AUDIO_REACTIVE_EXAMPLES.h
- Create your own custom audio effects using AudioSystem API
- Monitor performance with Serial commands

## Serial Commands Reference

```
audio                        - Show current audio levels
gain [0.0-1.0]              - Set microphone gain
palettes                    - List available color palettes
models                      - List available models
sequences                   - Show sequence info
cycle                       - Cycle all palettes
random                      - Randomize all palettes
switch [function] [palette] - Switch function palette
help                        - Show all commands
```

## Common Settings

**For loud environments:**
```
gain 0.3
```

**For quiet environments:**
```
gain 0.9
```

**For bass-heavy music:**
Focus on lower bands (0-9) in your custom effects

**For vocal-focused content:**
Focus on mid bands (10-19) in your custom effects

## Performance Tips

- The system updates at 20ms intervals (50 Hz)
- FFT calculation is shared across all audio functions
- Cached band data prevents redundant reads
- Typical CPU usage: 5-10% on Teensy 4.x

---

Need help? Check AUDIO_SYSTEM_README.md for complete documentation.
