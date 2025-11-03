# Getting Started with Automated Stateful Functions

## TL;DR - What Changed?

You no longer need to manually create fire effects, audio functions, or any stateful color functions. It's all automatic now!

## The New Way (Super Simple!)

### In your setup() function:

```cpp
void setup() {
  // ... your hardware setup ...
  
  initedgedata();
  
  // ONE LINE - that's it!
  initializeStatefulColorFunctions();
  
  initializemodels();
  
  // ... rest of your setup ...
}
```

### That's literally all you need to do!

This single function call creates and registers:
- ✅ `fire2012` - Red/orange fire
- ✅ `fire2012_blue` - Blue fire  
- ✅ `fire2012_green` - Green fire
- ✅ `audio` - Audio reactive
- ✅ `vumeter` - VU meter

## What to Delete

If you have old code with manual instantiation, delete these:

### ❌ Delete Global Pointers
```cpp
// DELETE THESE LINES
Fire2012ColorFunction* fire2012Standard = nullptr;
Fire2012ColorFunction* fire2012Blue = nullptr;
Fire2012ColorFunction* fire2012Green = nullptr;
AudioReactiveColorFunction* audioReactive = nullptr;
VUMeterColorFunction* vuMeter = nullptr;
```

### ❌ Delete Manual Instantiation
```cpp
// DELETE ALL OF THIS
Serial.println("Initializing stateful color functions...");

fire2012Standard = new Fire2012ColorFunction("fire2012", 55, 120, false, HeatColors_p);
registerStatefulColorFunction(8, fire2012Standard);

CRGBPalette16 bluePalette = CRGBPalette16(
  CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::White
);
fire2012Blue = new Fire2012ColorFunction("fire2012_blue", 45, 100, false, bluePalette);
registerStatefulColorFunction(9, fire2012Blue);

CRGBPalette16 greenPalette = CRGBPalette16(
  CRGB::Black, CRGB::Green, CRGB::LimeGreen, CRGB::Yellow
);
fire2012Green = new Fire2012ColorFunction("fire2012_green", 50, 110, false, greenPalette);
registerStatefulColorFunction(10, fire2012Green);

audioReactive = new AudioReactiveColorFunction("audio", A0, 100, 0.95);
registerStatefulColorFunction(11, audioReactive);

vuMeter = new VUMeterColorFunction("vumeter", A0, 100);
registerStatefulColorFunction(12, vuMeter);

Serial.println("Stateful color functions initialized.");
Serial.println("NOTE: Audio functions require audio input on pin A0");
```

### ✅ Replace with ONE Line
```cpp
initializeStatefulColorFunctions();
```

## Using the Functions

Nothing changes! Use them the same way in your sequences:

```cpp
void initializeSequences(modelsequence* seq) {
  // Fire sequence
  seq->addStepByName("flowoctahedron", 
    {"dark", "fire2012", "fire2012", "fire2012"}, 
    10, FADE, 2);
  
  // Audio sequence
  seq->addStepByName("cycle", 
    {"dark", "audio", "vumeter", "rainbow"}, 
    10, FADE, 2);
  
  // Mixed
  seq->addStepByName("octachain", 
    {"dark", "fire2012_blue", "audio", "fire2012_green"}, 
    10, WIPE, 1.5);
}
```

## What You Get

### Default Configuration

All functions come pre-configured with sensible defaults:

**Fire Effects:**
- Update every 20ms
- Optimized cooling/sparking values
- Custom color palettes per type

**Audio Effects:**
- Pin A0 (change in colorfunctions.cpp if needed)
- Sensitivity: 100
- Update every 10ms for responsiveness

### Automatic Features

- ✅ Memory managed automatically
- ✅ Functions registered at correct indices
- ✅ Palettes configured perfectly
- ✅ Serial output shows status
- ✅ Lazy evaluation (CPU efficient)

## FAQs

### Q: Do I need to change my sequences?
**A: No!** Sequences work exactly the same. Just use the function names.

### Q: What if I want different parameters?
**A: Two options:**
1. Edit `initializeStatefulColorFunctions()` in `colorfunctions.cpp`
2. Create additional functions after auto-init in your setup()

### Q: Does this work with custom stateful functions?
**A: Yes!** You can still add your own:
```cpp
void setup() {
  initializeStatefulColorFunctions();  // Get the standard ones
  
  // Add your custom one
  MyCustomFunction* custom = new MyCustomFunction("custom", params);
  registerStatefulColorFunction(15, custom);
}
```

### Q: What about performance?
**A: Same!** The lazy evaluation optimization is preserved. Functions only update when actually used in the current frame.

### Q: Can I skip the audio functions?
**A: They won't hurt!** If you don't use them in sequences, they won't update (lazy evaluation). Zero CPU cost!

### Q: Where are the functions actually created?
**A: In `colorfunctions.cpp`** in the `initializeStatefulColorFunctions()` function. Check there if you want to see or modify the defaults.

## Next Steps

1. ✅ Replace manual code with `initializeStatefulColorFunctions()`
2. ✅ Upload to your Teensy
3. ✅ Watch the serial output to see functions initialize
4. ✅ Use them in your sequences by name

That's it! Enjoy your cleaner, simpler code!

## Need More Info?

- **Quick comparison:** [QUICK_REFERENCE.md](computer:///mnt/user-data/outputs/QUICK_REFERENCE.md)
- **Full guide:** [AUTOMATED_COLORFUNCTIONS_README.md](computer:///mnt/user-data/outputs/AUTOMATED_COLORFUNCTIONS_README.md)
- **Technical details:** [CHANGES_SUMMARY.md](computer:///mnt/user-data/outputs/CHANGES_SUMMARY.md)
- **All files:** [INDEX.md](computer:///mnt/user-data/outputs/INDEX.md)
