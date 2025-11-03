# Audio Source Switching Guide

## Overview
The modelsequence system supports dynamic audio source switching between microphone input and SD card playback. Each step in a sequence can specify which audio source to use.

## Audio Source Types

### 1. AUDIO_KEEP_CURRENT
- Maintains the current audio source
- Used when you want to keep playing the same SD card file or stay on microphone
- **Default behavior if no audio config is specified**

### 2. AUDIO_MICROPHONE
- Switches to live microphone input
- Uses the Teensy Audio Shield's microphone/line input
- Real-time audio reactivity

### 3. AUDIO_SD_CARD
- Plays a WAV file from the SD card
- Supports variable playback rate (0.01x to 4.0x)
- Supports looping or one-shot playback

## AudioSourceConfig Structure

```cpp
AudioSourceConfig(AudioSourceType type, 
                  String filename = "", 
                  float playbackRate = 1.0, 
                  bool looping = false)
```

### Parameters:
- **type**: AUDIO_KEEP_CURRENT, AUDIO_MICROPHONE, or AUDIO_SD_CARD
- **filename**: Path to WAV file on SD card (only used for AUDIO_SD_CARD)
- **playbackRate**: Playback speed multiplier (0.01 to 4.0)
  - 0.5 = half speed (50%)
  - 1.0 = normal speed (100%)
  - 2.0 = double speed (200%)
- **looping**: true = loop file continuously, false = play once

## Usage Examples

### Example 1: Default (Microphone)
```cpp
// No audio config = uses microphone (or keeps current source)
seq->addStep("flowoctahedron", {"dark", "audio", "audio", "audio"}, 10, FADE, 2);
```

### Example 2: Explicit Microphone
```cpp
seq->addStep("flowoctahedron", {"dark", "audio", "vumeter", "audio"}, 10, FADE, 2,
             AudioSourceConfig(AUDIO_MICROPHONE));
```

### Example 3: SD Card Playback (Normal Speed, Looping)
```cpp
seq->addStep("cycle", {"dark", "audio", "fire2012", "vumeter"}, 10, FADE, 2,
             AudioSourceConfig(AUDIO_SD_CARD, "music.wav", 1.0, true));
```

### Example 4: SD Card Playback (Slow Motion, No Loop)
```cpp
seq->addStep("octachain", {"dark", "audio", "plasma", "audio"}, 20, FADE, 2,
             AudioSourceConfig(AUDIO_SD_CARD, "intro.wav", 0.5, false));
```

### Example 5: SD Card Playback (Double Speed, Looping)
```cpp
seq->addStep("cycles", {"dark", "vumeter", "rainbow", "audio"}, 10, FADE, 2,
             AudioSourceConfig(AUDIO_SD_CARD, "beat.wav", 2.0, true));
```

### Example 6: Keep Current Source
```cpp
// This step will continue using whatever audio source is currently active
seq->addStep("twentyfourcell", {"dark", "audio", "audio", "fire2012"}, 10, FADE, 2,
             AudioSourceConfig(AUDIO_KEEP_CURRENT));
```

## Complete Sequence Example: Source Switching

```cpp
seq->startNewSequence("Multi-Source Demo", 60, true);

// Start with microphone
seq->addStep("flowoctahedron", {"dark", "audio", "audio", "rainbow"}, 10, FADE, 2,
             AudioSourceConfig(AUDIO_MICROPHONE));

// Switch to SD card - play drums at normal speed
seq->addStep("cycle", {"dark", "vumeter", "fire2012", "vumeter"}, 10, FADE, 2,
             AudioSourceConfig(AUDIO_SD_CARD, "drums.wav", 1.0, true));

// Keep playing the same file (no need to reload)
seq->addStep("octachain", {"dark", "audio", "plasma", "audio"}, 10, FADE, 2,
             AudioSourceConfig(AUDIO_KEEP_CURRENT));

// Switch to different file at half speed
seq->addStep("cycles", {"dark", "vumeter", "particles", "audio"}, 10, FADE, 2,
             AudioSourceConfig(AUDIO_SD_CARD, "ambient.wav", 0.5, true));

// Back to microphone for live input
seq->addStep("hypercube", {"dark", "audio", "rainbow", "vumeter"}, 10, FADE, 2,
             AudioSourceConfig(AUDIO_MICROPHONE));

// One-shot playback (no loop) - plays once then stops
seq->addStep("twentyfourcell", {"dark", "audio", "fire2012_blue", "audio"}, 10, FADE, 2,
             AudioSourceConfig(AUDIO_SD_CARD, "effect.wav", 1.0, false));
```

## Variable Speed Playback Examples

```cpp
// Create a tempo progression - same file at different speeds
seq->startNewSequence("Speed Ramp", 40, true);

seq->addStep("flowoctahedron", {"dark", "audio", "plasma", "audio"}, 10, FADE, 1,
             AudioSourceConfig(AUDIO_SD_CARD, "beat.wav", 0.5, true));  // 50% - Slow

seq->addStep("cycle", {"dark", "vumeter", "particles", "vumeter"}, 10, FADE, 1,
             AudioSourceConfig(AUDIO_SD_CARD, "beat.wav", 1.0, true));  // 100% - Normal

seq->addStep("octachain", {"dark", "audio", "fire2012", "audio"}, 10, FADE, 1,
             AudioSourceConfig(AUDIO_SD_CARD, "beat.wav", 1.5, true));  // 150% - Fast

seq->addStep("cycles", {"dark", "vumeter", "rainbow", "vumeter"}, 10, FADE, 1,
             AudioSourceConfig(AUDIO_SD_CARD, "beat.wav", 2.0, true));  // 200% - Very Fast
```

## Best Practices

### 1. SD Card File Organization
```
SD_CARD_ROOT/
├── music.wav        (Background music)
├── beat.wav         (Rhythmic patterns)
├── drums.wav        (Percussion)
├── ambient.wav      (Atmospheric sounds)
├── intro.wav        (One-shot intro)
└── effect.wav       (Sound effects)
```

### 2. Looping vs. One-Shot
- **Looping (true)**: Use for continuous background music, beats, or ambience
- **One-Shot (false)**: Use for intros, transitions, or sound effects

### 3. Playback Rate Guidelines
- **0.25x - 0.5x**: Slow, meditative, or dramatic effect
- **0.75x - 1.25x**: Normal range with slight variation
- **1.5x - 2.0x**: Energetic, fast-paced
- **2.0x - 4.0x**: Extreme speed (use sparingly)

### 4. Audio-Reactive Effects
These color functions respond to audio:
- `audio` - General audio reactivity
- `vumeter` - VU meter visualization
- `freqbands` - Frequency band visualizer
- `basspulse` - Bass-reactive pulse
- `spectrum` - Spectrum analyzer
- `beatdetect` - Beat detection flash
- `vocals` - Vocal frequency highlighter

### 5. CPU Optimization
- Audio functions only update when they're actively being used
- If a sequence has no audio-reactive functions, the audio system doesn't process unnecessarily
- Use `AUDIO_KEEP_CURRENT` to avoid reloading the same file

## Important Notes

### Playback Rate Limitation
⚠️ **Current Implementation**: The standard `AudioPlaySdWav` class doesn't support variable playback rate. The rate parameter is stored but ignored.

**To enable variable playback rate**, you need to:
1. Replace `AudioPlaySdWav` with `AudioPlaySdResmp` in `audiosystem.h`
2. Update the audio connections accordingly
3. The `setPlaybackRate()` method will then work correctly

### File Format Requirements
- **Format**: WAV files only
- **Sample Rate**: 44.1 kHz recommended
- **Bit Depth**: 16-bit recommended
- **Channels**: Mono or Stereo

### SD Card Initialization
Make sure SD card is properly initialized in your setup:
```cpp
void setup() {
  AudioMemory(20);  // Allocate audio memory
  AudioSystem::initialize();  // This initializes SD card
  
  // Rest of setup...
}
```

## Troubleshooting

### Audio Not Playing
1. Check SD card is inserted and formatted (FAT32)
2. Verify WAV file exists at specified path
3. Check serial monitor for error messages
4. Ensure AudioMemory is allocated before AudioSystem::initialize()

### Audio-Reactive Effects Not Working
1. Verify microphone is selected: `AudioSystem::useMicrophone()`
2. Check microphone gain: `AudioSystem::setMicGain(0.5)` (0.0-1.0)
3. Ensure audio-reactive functions are in the active color function slots

### Playback Rate Not Changing
- This is expected with `AudioPlaySdWav`
- Upgrade to `AudioPlaySdResmp` for variable speed playback

## Serial Commands (if implemented in main sketch)
```
mic          - Switch to microphone
sd <file>    - Play SD card file
rate <n>     - Set playback rate (0.01-4.0)
loop on/off  - Enable/disable looping
stop         - Stop playback
```
