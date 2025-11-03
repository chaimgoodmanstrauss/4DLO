# Palette Switching Quick Reference

## Functions That Support Palettes
```cpp
fire2012          // Fire effects
fire2012_blue     
fire2012_green    
audio             // Audio reactive
plasma            // Plasma effect
particles         // Particle system
freqbands         // Frequency visualizer
beatdetect        // Beat detector
```

## Available Palettes
```cpp
// Custom
"fire"            "bluefire"      "greenfire"
"plasma"          "ocean"         "forest"
"sunset"          "rainbow"       "lava"
"ice"

// FastLED Built-in
"heat"            "party"         "cloud"
"ocean_builtin"   "forest_builtin"
"rainbow_builtin" "rainbowstripe"
```

## Basic Usage
```cpp
// Switch palette
switchPalette("fire2012", "ocean");

// In a sequence
switchPalette("fire2012", "lava");
seq->addStep("flowoctahedron", {"dark", "fire2012", "fire2012", "rainbow"}, 10, FADE, 2);

switchPalette("fire2012", "ice");
seq->addStep("cycle", {"dark", "fire2012", "plasma", "fire2012"}, 10, FADE, 2);
```

## Common Patterns

### Single Function Journey
```cpp
switchPalette("fire2012", "fire");    // Step 1
switchPalette("fire2012", "lava");    // Step 2
switchPalette("fire2012", "ocean");   // Step 3
switchPalette("fire2012", "ice");     // Step 4
```

### All Functions Coordinated
```cpp
switchPalette("fire2012", "rainbow");
switchPalette("audio", "rainbow");
switchPalette("plasma", "rainbow");
switchPalette("particles", "rainbow");
```

### Contrasting Palettes
```cpp
switchPalette("fire2012", "fire");    // Hot
switchPalette("plasma", "ice");       // Cool
switchPalette("particles", "forest"); // Natural
```

### With Audio Source
```cpp
switchPalette("audio", "rainbow");
seq->addStep(..., AudioSourceConfig(AUDIO_MICROPHONE));

switchPalette("audio", "lava");
seq->addStep(..., AudioSourceConfig(AUDIO_SD_CARD, "beat.wav", 1.0, true));
```

## Color Mood Guide
- **Energetic**: rainbow, party, heat, rainbowstripe
- **Calm**: ocean, ice, cloud, forest
- **Dramatic**: fire, lava, sunset
- **Cool**: ice, ocean, ocean_builtin, bluefire
- **Warm**: fire, lava, heat, greenfire
- **Natural**: forest, forest_builtin, greenfire

## Palette Progressions

### Temperature Gradient
```cpp
ice → ocean → forest → fire → lava
(cool)                        (hot)
```

### Energy Gradient
```cpp
ocean → forest → rainbow → party → heat
(calm)                          (intense)
```

### Time of Day
```cpp
ice → ocean → rainbow → sunset → fire → lava
(dawn)                                  (night)
```

## Example Sequence Template
```cpp
seq->startNewSequence("My Palette Show", 60, true);

switchPalette("fire2012", "fire");
switchPalette("audio", "rainbow");
seq->addStep("flowoctahedron", {"dark", "fire2012", "audio", "plasma"}, 10, FADE, 2,
             AudioSourceConfig(AUDIO_MICROPHONE));

switchPalette("fire2012", "ocean");
switchPalette("audio", "ice");
seq->addStep("cycle", {"dark", "audio", "fire2012", "particles"}, 10, FADE, 2);

switchPalette("fire2012", "party");
switchPalette("audio", "party");
seq->addStep("octachain", {"dark", "fire2012", "audio", "audio"}, 10, FADE, 2);
```
