# Palette Switching Guide

## Overview
The color function system supports dynamic palette switching at runtime. This allows you to change the color schemes of various effects without restarting or recompiling.

## Available Palettes

### Custom Palettes
- **fire** - Classic fire (red/orange/yellow)
- **bluefire** - Blue fire effect
- **greenfire** - Green fire effect  
- **plasma** - Purple/magenta/orange plasma
- **ocean** - Deep blue to cyan ocean colors
- **forest** - Dark to light greens
- **sunset** - Purple/red/orange/yellow sunset
- **rainbow** - Full spectrum rainbow
- **lava** - Hot lava (black to white through reds/oranges)
- **ice** - Cool ice (black to white through blues)

### FastLED Built-in Palettes
- **heat** - Heat map colors
- **party** - Bright party colors
- **cloud** - Soft cloud colors
- **ocean_builtin** - FastLED ocean colors
- **forest_builtin** - FastLED forest colors
- **rainbow_builtin** - FastLED rainbow
- **rainbowstripe** - Rainbow stripes

## Functions That Support Palettes

### Fire Effects (Indices 8-10)
- `fire2012` - Classic fire effect
- `fire2012_blue` - Blue fire variant
- `fire2012_green` - Green fire variant

### Audio-Reactive (Index 11)
- `audio` - General audio reactive effect

### Visual Effects
- `plasma` (Index 13) - Plasma effect
- `particles` (Index 14) - Particle system
- `freqbands` (Index 17) - Frequency band visualizer
- `beatdetect` (Index 20) - Beat detection flash

## Palette Switching Functions

### 1. switchPalette(functionName, paletteName)
Switch a specific function to a specific palette.

```cpp
switchPalette("fire2012", "ocean");     // Fire effect uses ocean colors
switchPalette("audio", "lava");         // Audio effect uses lava colors
switchPalette("plasma", "rainbow");     // Plasma uses rainbow colors
```

### 2. cycleAllPalettes()
Cycle all palettized functions to the next palette in the registry.

```cpp
cycleAllPalettes();  // All effects move to next palette
```

### 3. randomizeAllPalettes()
Assign random palettes to all palettized functions.

```cpp
randomizeAllPalettes();  // Each function gets a random palette
```

## Usage in Sequences

### Basic Pattern
```cpp
seq->startNewSequence("My Sequence", 60, true);

// Set palette before the step
switchPalette("fire2012", "ocean");
seq->addStep("flowoctahedron", {"dark", "fire2012", "fire2012", "rainbow"}, 10, FADE, 2);

// Change palette for next step
switchPalette("fire2012", "lava");
seq->addStep("cycle", {"dark", "fire2012", "plasma", "fire2012"}, 10, FADE, 2);
```

## Complete Examples

### Example 1: Single Function Palette Journey
One effect cycles through different color schemes:

```cpp
seq->startNewSequence("Palette Progression", 60, true);

// Classic fire
switchPalette("fire2012", "fire");
seq->addStep("flowoctahedron", {"dark", "fire2012", "fire2012", "fire2012"}, 10, FADE, 2);

// Hot lava
switchPalette("fire2012", "lava");
seq->addStep("cycle", {"dark", "fire2012", "fire2012", "rainbow"}, 10, FADE, 2);

// Cool ocean
switchPalette("fire2012", "ocean");
seq->addStep("octachain", {"dark", "fire2012", "fire2012", "staticblue"}, 10, FADE, 2);

// Natural forest
switchPalette("fire2012", "forest");
seq->addStep("cycles", {"dark", "fire2012", "rainbow", "fire2012"}, 10, FADE, 2);

// Frozen ice
switchPalette("fire2012", "ice");
seq->addStep("twentyfourcell", {"dark", "fire2012", "fire2012", "particles"}, 10, FADE, 2);

// Party time!
switchPalette("fire2012", "party");
seq->addStep("hypercube", {"dark", "fire2012", "plasma", "fire2012"}, 10, FADE, 2);
```

### Example 2: Coordinated Palettes
Multiple functions use the same palette:

```cpp
seq->startNewSequence("Coordinated Palettes", 60, true);

// All rainbow
switchPalette("fire2012", "rainbow");
switchPalette("audio", "rainbow");
switchPalette("plasma", "rainbow");
switchPalette("particles", "rainbow");
seq->addStep("flowoctahedron", {"dark", "fire2012", "plasma", "audio"}, 10, FADE, 2,
             AudioSourceConfig(AUDIO_MICROPHONE));

// All lava
switchPalette("fire2012", "lava");
switchPalette("audio", "lava");
switchPalette("plasma", "lava");
switchPalette("particles", "lava");
seq->addStep("cycle", {"dark", "audio", "particles", "plasma"}, 10, FADE, 2);

// All ocean
switchPalette("fire2012", "ocean");
switchPalette("audio", "ocean");
switchPalette("plasma", "ocean");
switchPalette("particles", "ocean");
seq->addStep("octachain", {"dark", "plasma", "fire2012", "audio"}, 10, FADE, 2);
```

### Example 3: Contrasting Palettes
Different functions use complementary palettes:

```cpp
seq->startNewSequence("Mixed Palettes", 40, true);

// Fire + Ocean + Rainbow (hot + cool + spectrum)
switchPalette("fire2012", "fire");
switchPalette("plasma", "ocean");
switchPalette("particles", "rainbow");
seq->addStep("flowoctahedron", {"dark", "fire2012", "plasma", "particles"}, 10, FADE, 2);

// Ice + Lava + Forest (cool + hot + natural)
switchPalette("fire2012", "ice");
switchPalette("plasma", "lava");
switchPalette("particles", "forest");
seq->addStep("cycle", {"dark", "particles", "fire2012", "plasma"}, 10, FADE, 2);

// Sunset + Party + Heat (warm + bright + hot)
switchPalette("fire2012", "sunset");
switchPalette("plasma", "party");
switchPalette("particles", "heat");
seq->addStep("octachain", {"dark", "plasma", "particles", "fire2012"}, 10, FADE, 2);
```

### Example 4: Audio + Palette Pairing
Match palettes to audio mood:

```cpp
seq->startNewSequence("Audio Palette Journey", 50, true);

// Energetic: Rainbow + Microphone
switchPalette("audio", "rainbow");
seq->addStep("flowoctahedron", {"dark", "audio", "audio", "audio"}, 10, FADE, 2,
             AudioSourceConfig(AUDIO_MICROPHONE));

// Hot: Lava + Fire colors
switchPalette("audio", "lava");
switchPalette("fire2012", "fire");
seq->addStep("cycle", {"dark", "audio", "audio", "fire2012"}, 10, FADE, 2);

// Cool: Ocean + Ice colors
switchPalette("audio", "ocean");
switchPalette("plasma", "ice");
seq->addStep("octachain", {"dark", "audio", "audio", "plasma"}, 10, FADE, 2);

// Natural: Forest colors
switchPalette("audio", "forest");
switchPalette("freqbands", "forest");
seq->addStep("cycles", {"dark", "freqbands", "audio", "particles"}, 10, FADE, 2);

// Party: Party colors!
switchPalette("audio", "party");
switchPalette("beatdetect", "party");
seq->addStep("twentyfourcell", {"dark", "beatdetect", "audio", "fire2012"}, 10, FADE, 2);
```

### Example 5: Ultimate Combo (Audio Source + Palette)
Combine audio source switching with palette changes:

```cpp
seq->startNewSequence("Ultimate Audio+Palette", 60, true);

// Mic + Rainbow
switchPalette("audio", "rainbow");
switchPalette("particles", "rainbow");
seq->addStep("flowoctahedron", {"dark", "audio", "audio", "particles"}, 10, FADE, 2,
             AudioSourceConfig(AUDIO_MICROPHONE));

// SD Card + Hot colors
switchPalette("audio", "lava");
switchPalette("fire2012", "fire");
seq->addStep("cycle", {"dark", "audio", "fire2012", "audio"}, 10, FADE, 2,
             AudioSourceConfig(AUDIO_SD_CARD, "drums.wav", 1.0, true));

// Keep SD + Cool colors
switchPalette("audio", "ocean");
switchPalette("plasma", "ice");
seq->addStep("octachain", {"dark", "audio", "plasma", "audio"}, 10, FADE, 2,
             AudioSourceConfig(AUDIO_KEEP_CURRENT));

// Mic + Natural colors
switchPalette("audio", "forest");
switchPalette("freqbands", "forest");
seq->addStep("cycles", {"dark", "freqbands", "audio", "particles"}, 10, FADE, 2,
             AudioSourceConfig(AUDIO_MICROPHONE));

// SD Card + Party (fast beat + party colors!)
switchPalette("audio", "party");
switchPalette("beatdetect", "party");
seq->addStep("twentyfourcell", {"dark", "beatdetect", "audio", "fire2012"}, 10, FADE, 2,
             AudioSourceConfig(AUDIO_SD_CARD, "beat.wav", 1.5, true));

// Finale: Mic + Sunset
switchPalette("audio", "sunset");
switchPalette("plasma", "sunset");
seq->addStep("hypercube", {"dark", "audio", "plasma", "audio"}, 10, FADE, 2,
             AudioSourceConfig(AUDIO_MICROPHONE));
```

## Runtime Commands (if implemented in main sketch)

```
palette <function> <palette>  - Switch specific function to palette
cycle_palettes                - Cycle all to next palette
random_palettes               - Randomize all palettes
list_palettes                 - Show available palettes
```

## Color Theory Tips

### Complementary Palettes
- **Fire** + **Ice** - Hot vs. cool contrast
- **Lava** + **Ocean** - Warm vs. cold
- **Sunset** + **Forest** - Earthy combinations

### Progressive Palettes (smooth transitions)
1. Fire → Lava → Sunset (hot progression)
2. Ice → Ocean → Party (cool to energetic)
3. Forest → Rainbow → Party (natural to vibrant)

### Mood Palettes
- **Energetic**: rainbow, party, heat
- **Calm**: ocean, ice, forest
- **Dramatic**: fire, lava, sunset
- **Natural**: forest, ocean, rainbow

## Advanced Techniques

### Palette Tempo Matching
Match palette changes to audio tempo:
```cpp
// Slow ambient music → slow palette changes
switchPalette("audio", "ocean");
seq->addStep(..., 20, FADE, 4);  // Long duration, slow fade

// Fast beat → rapid palette changes
switchPalette("audio", "party");
seq->addStep(..., 5, INSTANT, 0);  // Short duration, instant
```

### Palette Storytelling
Create emotional arcs through palette progression:
```cpp
// Dawn: Ice → Sunset → Rainbow (dark to light)
// Day: Rainbow → Party → Heat (energetic)
// Dusk: Heat → Sunset → Fire (warm down)
// Night: Fire → Lava → Ocean → Ice (cool down)
```

## Technical Notes

### When Palettes Update
- Palette changes take effect immediately via `switchPalette()`
- Changes persist until next `switchPalette()` call
- Each effect maintains its own palette reference

### Performance
- Palette switching is lightweight (just updating a pointer)
- No performance impact from switching palettes
- Multiple effects can share the same palette

### Palette Registry
All palettes are registered at startup in `PaletteRegistry::initialize()`
- Custom palettes defined in `paletteregistry.cpp`
- FastLED built-in palettes also available
- Maximum 50 palettes supported

## Troubleshooting

### Palette Not Changing
1. Verify function name is correct (case-insensitive)
2. Check palette name exists: `PaletteRegistry::printRegistry()`
3. Ensure function supports palettes (see list above)

### Function Not Found Error
```
Error: Function 'audiooo' not found
```
Fix: Check spelling - should be "audio" not "audiooo"

### Palette Not Found Error
```
Error: Palette 'greeen' not found in registry
```
Fix: Check available palettes - should be "greenfire" not "greeen"
