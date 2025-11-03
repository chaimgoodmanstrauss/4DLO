# Automated Stateful Color Functions

## Overview

The stateful color function system has been **fully automated**. You no longer need to manually instantiate or register fire effects, audio reactive functions, or any other stateful color functions.

## What Changed

### ❌ OLD WAY (Manual - Don't do this anymore!)

```cpp
// Had to declare global pointers
Fire2012ColorFunction* fire2012Standard = nullptr;
Fire2012ColorFunction* fire2012Blue = nullptr;
Fire2012ColorFunction* fire2012Green = nullptr;
AudioReactiveColorFunction* audioReactive = nullptr;
VUMeterColorFunction* vuMeter = nullptr;

void setup() {
  // Had to manually instantiate each one
  fire2012Standard = new Fire2012ColorFunction("fire2012", 55, 120, false, HeatColors_p);
  registerStatefulColorFunction(8, fire2012Standard);
  
  // Create palettes manually
  CRGBPalette16 bluePalette = CRGBPalette16(
    CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::White
  );
  fire2012Blue = new Fire2012ColorFunction("fire2012_blue", 45, 100, false, bluePalette);
  registerStatefulColorFunction(9, fire2012Blue);
  
  // ... repeat for every stateful function
}
```

### ✅ NEW WAY (Automatic!)

```cpp
void setup() {
  // Just call one function - everything is handled automatically!
  initializeStatefulColorFunctions();
  
  // That's it! All fire effects, audio functions, etc. are ready to use
}
```

## How It Works

### Automatic Registration

When you call `initializeStatefulColorFunctions()`, the system automatically:

1. **Creates all stateful color function instances** (fire effects, audio, VU meter)
2. **Registers them at the correct indices** (8-12)
3. **Sets up all palettes and parameters** with sensible defaults
4. **Manages memory properly** - instances persist for the lifetime of the program

### What Gets Created Automatically

| Index | Name | Type | Description |
|-------|------|------|-------------|
| 8 | `fire2012` | Fire2012 | Standard red/orange fire effect |
| 9 | `fire2012_blue` | Fire2012 | Blue fire effect |
| 10 | `fire2012_green` | Fire2012 | Green fire effect |
| 11 | `audio` | Audio Reactive | Dynamic audio-responsive colors |
| 12 | `vumeter` | VU Meter | Classic VU meter visualization |

### Default Parameters

The auto-initialization uses these sensible defaults:

**Fire2012 Standard (Red/Orange)**
- Cooling: 55
- Sparking: 120
- Palette: HeatColors_p

**Fire2012 Blue**
- Cooling: 45 (cooler for blue effect)
- Sparking: 100
- Palette: Black → Blue → Aqua → White

**Fire2012 Green**
- Cooling: 50
- Sparking: 110
- Palette: Black → Green → LimeGreen → Yellow

**Audio Reactive**
- Pin: A0
- Sensitivity: 100
- Decay: 0.95

**VU Meter**
- Pin: A0
- Sensitivity: 100

## Usage in Your Code

### In setup()

```cpp
void setup() {
  // ... your other initialization code ...
  
  initedgedata();
  
  // AUTO-INITIALIZE - Just one line!
  initializeStatefulColorFunctions();
  
  initializemodels();
  
  // ... rest of setup ...
}
```

### Using in Sequences

You can immediately use these functions by name in your sequences:

```cpp
void initializeSequences(modelsequence* seq) {
  seq->startNewSequence("Fire Show", 60, true);
  seq->addStepByName("flowoctahedron", {"dark", "fire2012", "fire2012", "fire2012"}, 10, FADE, 2);
  seq->addStepByName("cycle", {"dark", "fire2012_blue", "audio", "vumeter"}, 10, FADE, 2);
  // etc...
}
```

## Advanced: Adding Custom Stateful Functions

If you want to add your own stateful color functions, you have two options:

### Option 1: Modify the Auto-Init Function (Recommended)

Add your custom function to `colorfunctions.cpp` in the `initializeStatefulColorFunctions()` function:

```cpp
void initializeStatefulColorFunctions() {
  // ... existing code ...
  
  // Add your custom function
  MyCustomFunction* myFunc = new MyCustomFunction("mycustom", /* params */);
  registerStatefulColorFunction(13, myFunc);  // Use next available index
}
```

### Option 2: Manual Registration (Advanced)

If you need runtime configuration, you can still manually register after auto-init:

```cpp
void setup() {
  initializeStatefulColorFunctions();  // Get the standard ones
  
  // Add a custom one with special parameters
  Fire2012ColorFunction* purpleFire = new Fire2012ColorFunction(
    "purple_fire", 60, 100, false, myPurplePalette
  );
  registerStatefulColorFunction(15, purpleFire);
}
```

## Benefits

1. **No Boilerplate**: Eliminates ~50 lines of repetitive setup code
2. **No Memory Management**: The system handles allocation properly
3. **Consistent Defaults**: Everyone gets the same well-tuned parameters
4. **Easy to Maintain**: All stateful functions defined in one place
5. **Still Flexible**: You can add custom functions if needed
6. **Lazy Evaluation**: Functions still only update when actually used (CPU efficient!)

## Lazy Evaluation (CPU Optimization)

The automated system preserves the lazy evaluation optimization:

- **Stateful functions only update when used** in the current frame
- If a sequence doesn't use `fire2012`, it won't update (saves CPU)
- If a sequence uses `audio` and `fire2012_blue`, only those two update
- Optimization is automatic - you don't need to do anything!

## Files Modified

- `colorfunctions.h` - Added `initializeStatefulColorFunctions()` declaration
- `colorfunctions.cpp` - Added auto-initialization implementation
- `hdlo_controller.ino` - Removed manual instantiation, replaced with single function call

## Migration Guide

If you have existing code:

1. **Remove** all global pointer declarations (Fire2012ColorFunction*, etc.)
2. **Remove** all manual instantiation code in setup()
3. **Add** one line: `initializeStatefulColorFunctions();`
4. **Done!** Everything else works exactly the same

Your sequences, model definitions, and all other code remain unchanged!

## Questions?

The auto-initialization happens in `colorfunctions.cpp` in the `initializeStatefulColorFunctions()` function. Check there if you need to see exactly what's being created or modify the defaults.
