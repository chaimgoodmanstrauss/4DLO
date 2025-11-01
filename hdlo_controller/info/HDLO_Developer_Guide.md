# HDLO LED Controller - Developer Guide

A comprehensive guide for extending and customizing the Arduino-based LED controller for a 120-edge 3D/4D geometric sculpture with audio-reactive capabilities.

---

## Table of Contents
1. [System Overview](#system-overview)
2. [Adding New Sequences](#adding-new-sequences)
3. [Creating Color Functions](#creating-color-functions)
4. [Adding Palettes](#adding-palettes)
5. [Creating Models](#creating-models)
6. [Quick Reference](#quick-reference)

---

## System Overview

### Architecture

The system controls a 120-edge geometric sculpture with individually addressable LEDs using:
- **Hardware**: Teensy 4.x + OctoWS2811 + Audio shield
- **Main Loop**: Audio → Sequences → LED mapping → Display
- **Key Components**: Models (topology), Color Functions (effects), Palettes (color schemes), Sequences (choreography)

### Main Flow (`hdlo_controller.ino`)

```cpp
void setup() {
    1. Initialize edge data
    2. Initialize OctoWS2811 and FastLED
    3. Initialize audio system
    4. Initialize palettes
    5. Initialize color functions
    6. Initialize models
    7. Create and start sequences
}

void loop() {
    1. Update audio system (FFT analysis)
    2. Update frame counter
    3. Update sequence (transitions, timing)
    4. Map sequences to LED array
    5. Display LEDs
}
```

### File Structure

```
hdlo_controller.ino       # Main Arduino sketch
sequences.cpp/h           # Sequence definitions (YOU EDIT THIS MOST)
modelsequence.cpp/h       # Sequence engine
colorfunctions.cpp/h      # Color effect implementations
paletteregistry.cpp/h     # Color palette definitions
models.cpp/h              # Model manipulation
hdlo_models.cpp/h         # Base model definitions
audiosystem.cpp/h         # Audio processing
edgepermutations.cpp/h    # Edge transformation system
```

---

## Adding New Sequences

### Basic Syntax

Edit `sequences.cpp` in the `initializeSequences()` function:

```cpp
void initializeSequences(modelsequence* seq) {
    seq->clearRegistry();
    
    // Start a new sequence
    seq->startNewSequence("My Sequence Name", 
                          60,      // duration in seconds
                          true,    // enabled
                          AudioSourceConfig(...));  // optional audio config
    
    // Add steps to the sequence
    seq->addStep("modelname", {
        "dark",                      // Background (usually dark)
        {"colorfunction1", "palette1"},  // Function with palette
        {"colorfunction2", "palette2"},
        "colorfunction3"             // Function with default palette
    }, 
    10,      // step duration (seconds)
    FADE,    // transition type
    2);      // transition duration (seconds)
    
    // Add more steps...
    seq->addStep("anothermodel", {...}, 15, FADE, 3);
}
```

### Available Models

From `hdlo_models.cpp`:
- **Basic**: `flowoctahedron`, `octachain`, `cycle`, `cycles`, `altcycles`
- **Advanced**: `allcycles`, `cube`, `hypercube`, `hypercubes`, `twentyfourcell`, `graycodes`
- **Custom**: Any models created via permutations/merging (see [Creating Models](#creating-models))

Use serial command `models` to see all registered models at runtime.

### Available Color Functions

From `colorfunctions.cpp`:

**Basic & Static**:
- `dark` - Black background
- `rainbow` - Rainbow gradient
- `bluetored` - Blue to red gradient
- `staticblue`, `staticred`, `staticgreen` - Solid colors

**Animated**:
- `pulsingblue`, `pulsingwhite`, `pulsingred` - Pulsing effects
- `cylon` - Scanning effect

**Fire Effects**:
- `fire2012` - Classic fire effect
- `fire2012_blue` - Blue fire
- `fire2012_green` - Green fire

**Audio-Reactive** (require audio input, all support palettes):
- `audio` - General audio reactivity
- `vumeter` - VU meter visualization
- `freqbands` - Frequency band display
- `basspulse` - Bass frequency pulse
- `spectrum` - Full spectrum analyzer
- `beatdetect` - Beat detection flash
- `vocals` - Vocal frequency highlighter

**Generative**:
- `plasma` - Plasma effect
- `particles` - Particle system

### Available Palettes

From `paletteregistry.cpp`:

**Custom Gradients**:
- `fire`, `bluefire`, `greenfire` - Fire variants
- `plasma` - Purple/magenta/yellow
- `ocean` - Deep blue to cyan
- `forest` - Dark green to yellow-green
- `sunset` - Purple to yellow
- `rainbow` - Full spectrum
- `lava` - Black to white through reds/oranges
- `ice` - Black to white through blues

**FastLED Built-ins**:
- `heat`, `party`, `cloud`
- `ocean_builtin`, `forest_builtin`, `rainbow_builtin`, `rainbowstripe`

Use serial command `palettes` to see all registered palettes.

### Transition Types

- `INSTANT` - Immediate switch, no transition
- `FADE` - Cross-fade between steps (smooth blend)
- `WIPE` - Sequential wipe across edges

### Audio Configuration

**Microphone Input**:
```cpp
AudioSourceConfig(AUDIO_MICROPHONE)  // Pure microphone
```

**SD Card Playback**:
```cpp
AudioSourceConfig(AUDIO_SD_CARD, "filename.wav")  // Note: Always plays at normal speed
```

**Microphone with Auto-Fallback**:
```cpp
// Switches to SD file after 3 seconds of silence
AudioSourceConfig("ambient.wav", 3000)  // Fallback file, timeout in milliseconds
```

**Keep Current Audio** (no change):
```cpp
AudioSourceConfig()  // or omit parameter entirely
```

**Advanced Configuration**:
```cpp
AudioSourceConfig config;
config.type = AUDIO_MICROPHONE;
config.enableFallback = true;
config.fallbackFile = "nature.wav";
config.silenceThreshold = 0.02;      // Sensitivity
config.silenceTimeout = 8000;        // 8 seconds

seq->startNewSequence("My Sequence", 90, true, config);
```

**Note**: Variable playback rate is not supported by the AudioPlaySdWav library. All audio plays at normal speed.

### Complete Sequence Example

```cpp
// Microphone with fallback
seq->startNewSequence("Reactive with Fallback", 60, true, 
                      AudioSourceConfig("ambient.wav", 3000));

seq->addStep("flowoctahedron", {
    "dark",
    {"audio", "fire"},
    {"vumeter", "fire"},
    {"beatdetect", "fire"}
}, 10, FADE, 2);

seq->addStep("cycle", {
    "dark",
    {"audio", "ocean"},
    {"freqbands", "ocean"},
    {"plasma", "ocean"}
}, 10, FADE, 2);

seq->addStep("octachain", {
    "dark",
    {"audio", "rainbow"},
    {"vumeter", "rainbow"},
    {"particles", "rainbow"}
}, 10, FADE, 2);

// SD Card sequence
seq->startNewSequence("Music Sync", 45, true,
                      AudioSourceConfig(AUDIO_SD_CARD, "track1.wav"));

seq->addStep("hypercube", {
    "dark",
    {"audio", "party"},
    {"vumeter", "party"},
    {"plasma", "party"}
}, 15, FADE, 2);

// Visual-only (no audio change)
seq->startNewSequence("Pure Visuals", 60, true);

seq->addStep("twentyfourcell", {
    "dark",
    {"fire2012", "lava"},
    {"plasma", "sunset"},
    {"rainbow"}
}, 10, FADE, 2);
```

---

## Creating Color Functions

### Simple Stateless Functions

For basic effects without persistent state, add to `colorfunctions.cpp`:

```cpp
// 1. Implement the function
CRGB myNewEffect(float position) {
    // position: 0.0 to 1.0 along the edge
    // Use millis() for time-based animation
    unsigned long t = millis();
    
    // Example: moving wave
    float wave = sin((position * 2 * PI) + (t / 1000.0));
    uint8_t brightness = (wave + 1.0) * 127.5;
    
    return CRGB(brightness, 0, 255 - brightness);
}

// 2. Add to colorFunctionArray (pick unused slot, e.g., 22)
ColorFunction colorFunctionArray[numcolorfunctions] = {
    constantlyDark,     // 0
    rainbow,            // 1
    // ... existing functions ...
    vocalsWrapper,      // 21
    myNewEffect,        // 22 - YOUR NEW FUNCTION
    nullptr,            // 23
    // ...
};

// 3. Add the name
String colorFunctionNames[numcolorfunctions] = {
    "dark",             // 0
    "rainbow",          // 1
    // ... existing names ...
    "vocals",           // 21
    "myneweffect",      // 22 - YOUR NEW NAME
    "",                 // 23
    // ...
};

// 4. Use in sequences
seq->addStep("flowoctahedron", {
    "dark",
    {"myneweffect", "rainbow"}
}, 10, FADE, 2);
```

### Stateful Functions with Palettes (RECOMMENDED)

For complex effects with animation state and palette support, the system now uses polymorphism for automatic integration:

```cpp
// In colorfunctions.h - Define your class
class MyAdvancedFunction : public StatefulColorFunction {
private:
    String paletteName;
    float phase;
    float speed;
    
public:
    MyAdvancedFunction(String name, float spd, String palette)
        : StatefulColorFunction(name, 20),  // 20ms update interval
          paletteName(palette),
          phase(0),
          speed(spd) {}
    
    // Required: Override getColor
    CRGB getColor(float position) override {
        // Get palette from registry
        CRGBPalette16* palette = PaletteRegistry::findByName(paletteName);
        if(palette == nullptr) {
            palette = PaletteRegistry::findByName("rainbow"); // Fallback
        }
        
        // Your effect logic here
        float wave = sin(position * TWO_PI + phase);
        uint8_t index = (uint8_t)((wave * 0.5 + 0.5) * 255);
        
        return ColorFromPalette(*palette, index);
    }
    
    // Required: Override updateState (called every 20ms)
    void updateState() override {
        phase += speed;
        if(phase > TWO_PI) phase -= TWO_PI;
    }
    
    // Required: Override reset
    void reset() override {
        phase = 0;
    }
    
    // IMPORTANT: Override these for AUTOMATIC palette support
    String getPaletteName() const override { 
        return paletteName; 
    }
    
    void setPalette(String name) override { 
        paletteName = name; 
    }
    
    // Optional: Add your own setters
    void setSpeed(float spd) { speed = spd; }
};
```

**Why override getPaletteName() and setPalette()?**
These virtual methods enable **automatic integration** with the palette system. Once you override them:
- `getCurrentPaletteName("myfunction")` automatically works
- `switchPalette("myfunction", "ocean")` automatically works  
- `cycleAllPalettes()` automatically cycles your function
- `randomizeAllPalettes()` automatically randomizes your function

**No manual type-checking code needed!**

### Registering Your Stateful Function

```cpp
// In colorfunctions.cpp - Add wrapper function
CRGB myAdvancedWrapper(float position) {
    return callStatefulColorFunction(22, position);  // Use next available slot
}

// Add to function array
ColorFunction colorFunctionArray[numcolorfunctions] = {
    // ... existing ...
    vocalsWrapper,        // 21
    myAdvancedWrapper,    // 22 - YOUR FUNCTION
};

// Add to names array
String colorFunctionNames[numcolorfunctions] = {
    // ... existing ...
    "vocals",           // 21
    "myadvanced",       // 22 - YOUR NAME
};

// Initialize in initializeStatefulColorFunctions()
void initializeStatefulColorFunctions() {
    // ... existing initializations ...
    
    // Your function (index 22)
    MyAdvancedFunction* myFunc = new MyAdvancedFunction(
        "myadvanced",    // name (must match colorFunctionNames)
        0.05,            // speed parameter
        "rainbow"        // default palette
    );
    registerStatefulColorFunction(22, myFunc);
    
    Serial.println("Stateful color functions initialized");
}
```

**That's it!** Your function now automatically supports all palette operations.
}, 10, FADE, 2);
```

### Complex Stateful Functions

For effects with persistent state (audio-reactive, fire, particles, etc.), create a class:

#### Step 1: Define the Class (in `colorfunctions.h`)

```cpp
class MyReactiveFunction : public StatefulColorFunction {
private:
    String paletteName;
    float smoothedBass;      // Persistent state
    float energy[128];       // Per-position buffer
    
public:
    MyReactiveFunction(String name, String palette)
        : StatefulColorFunction(name, 20),  // name, update interval (ms)
          paletteName(palette),
          smoothedBass(0) {
        memset(energy, 0, sizeof(energy));
    }
    
    // Called once per frame (lazy evaluation)
    void updateState() override {
        // Get audio data from centralized system
        float bassLevel = AudioSystem::getBassLevel();
        float midLevel = AudioSystem::getMidLevel();
        float highLevel = AudioSystem::getHighLevel();
        
        // Smooth audio for visual stability
        smoothedBass = smoothedBass * 0.8 + bassLevel * 0.2;
        
        // Update energy buffer with decay
        for(int i = 0; i < 128; i++) {
            energy[i] *= 0.95;  // Decay over time
        }
        
        // Add new energy based on audio
        if(bassLevel > 0.3) {
            int pos = random(128);
            energy[pos] = bassLevel;
        }
    }
    
    // Called for each LED position
    CRGB getColor(float position) override {
        // Map position to buffer index
        int idx = constrain((int)(position * 128), 0, 127);
        
        // Get energy at this position
        float e = energy[idx];
        
        // Add smoothed bass for global brightness
        e = constrain(e + smoothedBass * 0.5, 0.0, 1.0);
        
        // Map to palette
        CRGBPalette16 palette = PaletteRegistry::getPalette(paletteName);
        uint8_t paletteIndex = e * 255;
        return ColorFromPalette(palette, paletteIndex);
    }
    
    void reset() override {
        smoothedBass = 0;
        memset(energy, 0, sizeof(energy));
    }
    
    // Palette support - REQUIRED for runtime palette switching
    void setPaletteName(String newPalette) {
        paletteName = newPalette;
    }
    
    String getPaletteName() const { return paletteName; }
};
```

#### Step 2: Register It (in `colorfunctions.cpp`)

```cpp
void initializeStatefulColorFunctions() {
    // ... existing functions ...
    
    // Create and register at slot 22
    MyReactiveFunction* myReactive = new MyReactiveFunction(
        "myreactive",    // function name
        "fire"           // default palette
    );
    registerStatefulColorFunction(22, myReactive);
}
```

#### Step 3: Add Wrapper and Name

```cpp
// Add wrapper function
CRGB myReactiveWrapper(float position) {
    return callStatefulColorFunction(22, position);
}

// Add to arrays
ColorFunction colorFunctionArray[numcolorfunctions] = {
    // ... existing ...
    myReactiveWrapper,  // 22
};

String colorFunctionNames[numcolorfunctions] = {
    // ... existing ...
    "myreactive",       // 22
};
```

#### Step 4: Use It

```cpp
seq->addStep("flowoctahedron", {
    "dark",
    {"myreactive", "fire"},
    {"myreactive", "ocean"}
}, 10, FADE, 2);
```

#### Step 5: Enable Palette Switching (Optional)

To support runtime palette switching via serial commands, add your function to `switchPalette()` in `colorfunctions.cpp`:

```cpp
void switchPalette(String functionName, String paletteName) {
    // ... existing validation ...
    
    bool success = false;
    
    // ... existing index checks ...
    
    // Add your new function by index
    else if(funcIndex == 22) {  // Your function's index
        MyReactiveFunction* myFunc = static_cast<MyReactiveFunction*>(statefulColorFunctions[funcIndex]);
        myFunc->setPaletteName(paletteName);
        success = true;
    }
    
    // ...
}
```

And add to `getCurrentPaletteName()` for palette restoration:

```cpp
String getCurrentPaletteName(String functionName) {
    // ... existing code ...
    
    else if(funcIndex == 22) {  // Your function's index
        MyReactiveFunction* myFunc = static_cast<MyReactiveFunction*>(statefulColorFunctions[funcIndex]);
        return myFunc->getPaletteName();
    }
    
    return "";
}
```

**Note**: We use index-based checking with `static_cast` because Arduino/Teensy compiles with `-fno-rtti` (no RTTI support). Simply add an `else if` clause for your function's slot index.

### Available Audio Data

The `AudioSystem` class provides centralized audio analysis:

```cpp
// Basic levels (0.0 to 1.0)
float bass = AudioSystem::getBassLevel();      // Low frequencies
float mid = AudioSystem::getMidLevel();        // Mid frequencies
float high = AudioSystem::getHighLevel();      // High frequencies
float overall = AudioSystem::getOverallLevel(); // Total energy

// Beat detection
bool beat = AudioSystem::getBeatDetected();    // True on beat

// Frequency bands (8 bands across spectrum)
float band = AudioSystem::getFrequencyBand(bandIndex); // 0-7

// Raw FFT data (advanced usage)
// See audiosystem.h for FFT bin access
```

### Color Function Best Practices

- **Lazy Evaluation**: `updateState()` is only called once per frame, even if the function is used on multiple edges
- **Smooth Audio**: Always smooth audio values to avoid jittery visuals
- **Decay Effects**: Use multiplicative decay (e.g., `value *= 0.95`) for smooth trails
- **Constrain Values**: Always constrain to valid ranges before converting to colors
- **Use Palettes**: Map your values (0.0-1.0) to palette indices (0-255) for consistent color schemes
- **Palette Support**: Always implement `setPaletteName()` and `getPaletteName()` for audio-reactive functions - this enables runtime switching and proper sequence transitions

---

## Adding Palettes

### Option A: Custom Gradient Palette

Edit `paletteregistry.cpp`:

```cpp
// 1. Define palette in anonymous namespace (after existing palettes)
namespace {
    // ... existing palettes ...
    
    DEFINE_GRADIENT_PALETTE( CustomCyber_gp ) {
        0,    0,    0,    0,    // Black
       64,    0,  255,  255,    // Cyan
      128,  255,    0,  255,    // Magenta
      192,  255,  255,    0,    // Yellow
      255,  255,  255,  255     // White
    };
    
    DEFINE_GRADIENT_PALETTE( CustomNeon_gp ) {
        0,   10,    0,   50,    // Deep purple
       85,  255,    0,  200,    // Hot pink
      170,    0,  255,  100,    // Green
      255,  255,  200,    0     // Orange
    };
    
    DEFINE_GRADIENT_PALETTE( CustomVaporwave_gp ) {
        0,   255,    0,  128,   // Pink
       64,  128,    0,  255,    // Purple
      128,    0,  128,  255,    // Blue
      192,    0,  255,  255,    // Cyan
      255,  255,  128,  192     // Light pink
    };
} // anonymous namespace

// 2. Register in initialize() function
void PaletteRegistry::initialize() {
    if(initialized) return;
    
    Serial.println("Initializing Palette Registry...");
    
    // ... existing registrations ...
    
    // Register your new palettes
    registerPalette("cyber", CustomCyber_gp);
    registerPalette("neon", CustomNeon_gp);
    registerPalette("vaporwave", CustomVaporwave_gp);
    
    initialized = true;
    Serial.println("Palette Registry initialized with " + 
                   String(numRegistered) + " palettes");
}
```

**Gradient Format**:
- Each line: `position, R, G, B`
- Position: 0-255 (FastLED interpolates between)
- RGB: 0-255 for each channel
- You can have 2-16+ color stops

**Tips**:
- Start at 0, end at 255
- More stops = more control over transitions
- Use online tools to visualize gradients
- Test with `fire2012` to see how palette looks

### Option B: Use FastLED Built-in Palettes

```cpp
void PaletteRegistry::initialize() {
    // ... existing ...
    
    // Additional FastLED built-ins
    registerPalette("lava_builtin", LavaColors_p);
    registerPalette("rainbow_builtin", RainbowColors_p);
    registerPalette("stripe", RainbowStripeColors_p);
}
```

Available built-ins: `HeatColors_p`, `LavaColors_p`, `OceanColors_p`, `ForestColors_p`, `RainbowColors_p`, `RainbowStripeColors_p`, `PartyColors_p`, `CloudColors_p`

### Option C: Programmatic Palette

For algorithmic or computed palettes:

```cpp
CRGBPalette16 createCustomPalette() {
    CRGBPalette16 pal;
    
    // Method 1: Fill with 16 specific colors
    pal = CRGBPalette16(
        CRGB::Red,      CRGB::Orange,   CRGB::Yellow,   CRGB::Green,
        CRGB::Cyan,     CRGB::Blue,     CRGB::Purple,   CRGB::Pink,
        CRGB::Red,      CRGB::Orange,   CRGB::Yellow,   CRGB::Green,
        CRGB::Cyan,     CRGB::Blue,     CRGB::Purple,   CRGB::Pink
    );
    
    // Method 2: Fill algorithmically
    for(int i = 0; i < 16; i++) {
        uint8_t hue = i * 16;  // Evenly spaced hues
        pal[i] = CHSV(hue, 255, 255);
    }
    
    return pal;
}

// In initialize()
void PaletteRegistry::initialize() {
    // ... existing ...
    
    CRGBPalette16 myPal = createCustomPalette();
    registerPalette("mycustom", myPal);
}
```

### Using Palettes in Sequences

```cpp
seq->addStep("flowoctahedron", {
    "dark",
    {"fire2012", "cyber"},      // Your new palette
    {"audio", "neon"},
    {"plasma", "vaporwave"}
}, 10, FADE, 2);
```

### Runtime Palette Changes

Use serial commands:
- `palettes` - List all available palettes
- `cycle` - Cycle all color functions to next palette
- `random` - Randomize all palettes
- `switch fire2012 neon` - Switch specific function to specific palette

---

## Creating Models

Models define the edge topology - how the 120 edges connect and flow. Three approaches:

### Option A: Define a New Base Model (Advanced)

Requires understanding the edge structure. Each model is defined as:
```cpp
std::array<std::array<int, 6>, 120> modeldata
```

Where each edge has 6 integers defining its topology. Study existing models in `hdlo_models.cpp` (like `flowoctahedrondata`) to understand the structure.

Add to `hdlo_models.cpp`:
```cpp
// Define your edge data structure
std::array<std::array<int, 6>, 120> mynewmodeldata = {
    // 120 edges with 6 values each
    // (requires deep understanding of the geometry)
};

void initializemodels() {
    // ... existing models ...
    ourcolormodels[11] = new colormodel(mynewmodeldata, "mynewmodel");
}
```

### Option B: Create Permutations (Recommended - Easy)

Transform existing models using edge permutations. Edit `models.cpp` in `initializefancymodels()`:

```cpp
void initializefancymodels() {
    // Single permutation
    colormodel::applyEdgePermutation(
        "flowoctahedron",                    // base model
        EdgePermutation::getPermutation("rot24"),  // transformation
        "flowocta_rotated"                   // new name
    );
    
    // Multiple permutations (compound transformation)
    String perms[] = {"rot24", "mirror_xy", "rot120"};
    colormodel::applyEdgePermutationSequence(
        "cycle",              // base model
        perms,                // array of transformations
        3,                    // number of transformations
        "cycle_transformed"   // new name
    );
    
    // Create variations
    colormodel::applyEdgePermutation("hypercube", 
        EdgePermutation::getPermutation("mirror_xz"), 
        "hypercube_mirrored");
    
    colormodel::applyEdgePermutation("octachain", 
        EdgePermutation::getPermutation("invert"), 
        "octachain_inverted");
}
```

**Available Named Permutations** (from `namedpermutations.h` and `edgepermutations.cpp`):

Basic transformations:
- `identity` - No change
- `rot24` - 24-edge rotation
- `rot120` - 120-edge rotation (reversal)
- `mirror_xy` - Mirror across XY plane
- `mirror_xz` - Mirror across XZ plane
- `mirror_yz` - Mirror across YZ plane
- `invert` - Inverse transformation

Check all available permutations at runtime with serial command or by calling `EdgePermutation::printRegistry()`.

### Option C: Merge Two Models

Combine aspects of two models to create hybrids:

```cpp
void initializefancymodels() {
    // Simple merge
    colormodel::mergeModels(
        "flowoctahedron",    // first model
        "hypercube",         // second model
        "flowhypercube"      // new name
    );
    
    // Merge then permute
    colormodel* merged = colormodel::mergeModels(
        "cycle", 
        "cube", 
        "cyclecube"
    );
    
    // Apply transformation to merged model
    merged->applyEdgePermutation(
        EdgePermutation::getPermutation("rot24"), 
        "cyclecube_rotated"
    );
    
    // Complex example: merge and transform
    colormodel::mergeModels("octachain", "twentyfourcell", "octa24cell");
    colormodel::applyEdgePermutation("octa24cell", 
        EdgePermutation::getPermutation("mirror_xy"), 
        "octa24cell_mirrored");
}
```

### Checking Available Models

**At compile time**: Look in `hdlo_models.cpp` and `models.cpp`

**At runtime**:
- Serial command: `models`
- Or observe startup output when `colormodel::printRegistry()` is called
- The system prints: `"Registered model: [name]"` for each model

### Using Custom Models

Once registered, use them in sequences like any other model:

```cpp
seq->addStep("flowocta_rotated", {
    "dark",
    {"fire2012", "fire"},
    {"audio", "ocean"}
}, 10, FADE, 2);

seq->addStep("cyclecube_mirrored", {
    "dark",
    {"plasma", "sunset"},
    {"particles", "rainbow"}
}, 15, FADE, 3);
```

---

## Quick Reference

### Complete Example: Adding Everything

Here's how to add a new color function, palette, model, and use them together:

#### 1. Add Palette (`paletteregistry.cpp`)

```cpp
namespace {
    DEFINE_GRADIENT_PALETTE( CustomAurora_gp ) {
        0,    0,   64,   32,    // Deep teal
       85,    0,  128,  255,    // Blue
      170,  128,  255,  128,    // Green
      255,  255,  192,  255     // Light purple
    };
}

void PaletteRegistry::initialize() {
    // ... existing ...
    registerPalette("aurora", CustomAurora_gp);
}
```

#### 2. Add Color Function (`colorfunctions.h` and `.cpp`)

```cpp
// In colorfunctions.h
class AuroraFunction : public StatefulColorFunction {
private:
    String paletteName;
    float waves[3];  // Multiple wave frequencies
    
public:
    AuroraFunction(String name, String palette)
        : StatefulColorFunction(name, 30), paletteName(palette) {
        waves[0] = 0; waves[1] = 0; waves[2] = 0;
    }
    
    void updateState() override {
        unsigned long t = millis();
        waves[0] = sin(t / 2000.0) * 0.5 + 0.5;
        waves[1] = sin(t / 3000.0) * 0.5 + 0.5;
        waves[2] = sin(t / 5000.0) * 0.5 + 0.5;
    }
    
    CRGB getColor(float position) override {
        float combined = 0;
        combined += sin(position * PI * 2 + waves[0] * TWO_PI) * 0.3;
        combined += sin(position * PI * 4 + waves[1] * TWO_PI) * 0.3;
        combined += sin(position * PI * 6 + waves[2] * TWO_PI) * 0.4;
        combined = (combined + 1.0) * 0.5;  // Normalize to 0-1
        
        CRGBPalette16 pal = PaletteRegistry::getPalette(paletteName);
        return ColorFromPalette(pal, combined * 255);
    }
    
    void reset() override {
        waves[0] = waves[1] = waves[2] = 0;
    }
    
    void setPalette(String newPalette) { paletteName = newPalette; }
    String getPalette() const { return paletteName; }
};

// In colorfunctions.cpp
void initializeStatefulColorFunctions() {
    // ... existing ...
    AuroraFunction* aurora = new AuroraFunction("aurora", "aurora");
    registerStatefulColorFunction(22, aurora);
}

CRGB auroraWrapper(float position) {
    return callStatefulColorFunction(22, position);
}

ColorFunction colorFunctionArray[numcolorfunctions] = {
    // ... existing ...
    auroraWrapper,  // 22
};

String colorFunctionNames[numcolorfunctions] = {
    // ... existing ...
    "aurora",       // 22
};
```

#### 3. Add Model (`models.cpp`)

```cpp
void initializefancymodels() {
    // ... existing ...
    
    // Create new model from permutation
    colormodel::applyEdgePermutation("flowoctahedron", 
        EdgePermutation::getPermutation("mirror_xy"), 
        "flowocta_mirror");
    
    // Or merge two models
    colormodel::mergeModels("cycle", "hypercube", "cyclecube");
}
```

#### 4. Use in Sequence (`sequences.cpp`)

```cpp
void initializeSequences(modelsequence* seq) {
    seq->clearRegistry();
    
    seq->startNewSequence("Aurora Dreams", 60, true, 
                          AudioSourceConfig("ambient.wav", 3000));
    
    seq->addStep("flowocta_mirror", {
        "dark",
        {"aurora", "aurora"},
        {"plasma", "aurora"},
        {"particles", "ice"}
    }, 15, FADE, 3);
    
    seq->addStep("cyclecube", {
        "dark",
        {"aurora", "ocean"},
        {"fire2012", "aurora"},
        {"audio", "sunset"}
    }, 15, FADE, 3);
}
```

### Serial Commands Reference

Available at runtime through Serial Monitor (115200 baud):

**Information**:
- `help` - Show all commands
- `models` - List all registered models
- `palettes` - List all registered palettes
- `sequences` - Show current sequence info
- `audio` - Show audio levels
- `source` - Show current audio source

**Control**:
- `next` - Force next sequence
- `cycle` - Cycle all palettes
- `random` - Randomize all palettes
- `switch [function] [palette]` - Switch specific palette (e.g., `switch fire2012 neon`)

**Audio**:
- `mic` - Switch to microphone input
- `play [file]` - Play SD card file (e.g., `play track1.wav`)
- `loop [on/off]` - Enable/disable looping
- `stop` - Stop playback
- `pause` - Pause playback (note: restarts from beginning on resume)
- `resume` - Resume playback (restarts from beginning)
- `gain [0.0-1.0]` - Set microphone gain

**Note**: Playback rate control is not supported. The `pause` command stops playback, and `resume` restarts from the beginning.

### Slot Allocation Guide

When adding new functions, use available slots:

**Color Function Slots** (`numcolorfunctions = 100`):
- 0-21: Used by existing functions
- 22-99: Available for your custom functions

**Model Registry**:
- No hard limit, automatically managed
- Base models: 0-10
- Custom models added via `initializefancymodels()`

**Palette Registry**:
- No hard limit, automatically managed
- All palettes registered in `PaletteRegistry::initialize()`

### Development Workflow

1. **Test Basic**: Start with simple changes to `sequences.cpp`
2. **Add Palette**: Test with existing functions
3. **Add Function**: Test with existing palettes and models
4. **Add Model**: Test with existing functions and palettes
5. **Combine**: Create complete custom sequences

**Debugging Tips**:
- Use `Serial.println()` liberally in your functions
- Test each component independently
- Use `printActiveStatefulFunctions()` to see what's running
- Monitor serial output during startup for registration confirmations
- Use serial commands to test runtime changes

### Common Patterns

**Time-based animation**:
```cpp
unsigned long t = millis();
float phase = (t / 1000.0);  // Seconds
float wave = sin(phase);
```

**Audio-reactive brightness**:
```cpp
float level = AudioSystem::getOverallLevel();
uint8_t brightness = level * 255;
```

**Position-based color**:
```cpp
CRGBPalette16 pal = PaletteRegistry::getPalette(paletteName);
uint8_t index = position * 255;
return ColorFromPalette(pal, index);
```

**Smooth transitions**:
```cpp
// Exponential smoothing
smoothedValue = smoothedValue * 0.9 + newValue * 0.1;
```

**Decay effects**:
```cpp
// Multiplicative decay
buffer[i] *= 0.95;  // 5% decay per frame
```

### Best Practices

1. **Always constrain values** before color conversion
2. **Smooth audio data** to prevent jitter
3. **Use palettes** instead of hardcoded colors
4. **Test incrementally** - one change at a time
5. **Name clearly** - use descriptive names for functions/models/palettes
6. **Comment your code** - explain complex algorithms
7. **Memory management** - be careful with dynamic allocation in stateful functions
8. **Performance** - keep `getColor()` fast, put heavy computation in `updateState()`

---

## Recent Improvements (2025)

### Streamlined Color Function Registration

The system now uses polymorphism for automatic palette integration. When creating palette-supporting color functions:

**Old Way (Manual)**: Required editing `getCurrentPaletteName()` and `switchPalette()` with type-checking code for each new function (137 lines of boilerplate).

**New Way (Automatic)**: Simply override two virtual methods in your class:
```cpp
String getPaletteName() const override { return paletteName; }
void setPalette(String name) override { paletteName = name; }
```

Benefits:
- **67% less code** to maintain
- **Automatic integration** with all palette commands
- **Type-safe** - compiler catches missing implementations
- **Extensible** - new functions work immediately

### Audio System Constants

All FFT magic numbers are now named constants:
```cpp
const int NUM_FFT_BANDS = 40;
const int BASS_BAND_START = 0;
const int BASS_BAND_END = 9;
// ... etc
```

This makes audio code self-documenting and easier to tune.

### Safety Improvements

- NULL pointer checks in sequence transitions
- Array bounds validation in registry
- Boot-time safety checks
- Proper handling of `millis()` overflow

### Simplified Audio Configuration

- Removed non-functional playback rate feature
- Clearer documentation of pause/resume behavior
- Streamlined AudioSourceConfig structure

---

## Additional Resources

- **FastLED Documentation**: http://fastled.io/
- **Arduino Audio Library**: https://www.pjrc.com/teensy/td_libs_Audio.html
- **OctoWS2811**: https://www.pjrc.com/teensy/td_libs_OctoWS2811.html

For questions or contributions, refer to the project repository or documentation.

---

*Last Updated: 2025*
