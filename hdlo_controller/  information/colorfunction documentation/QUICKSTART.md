# Quick Start Guide: Stateful Color Functions

## Basic Usage

### 1. Include the Headers

```cpp
#include "colorfunctions.h"
// Optional: for additional effects
#include "advanced_colorfunctions.h"
```

### 2. Setup (in hdlo_controller.ino)

```cpp
void setup() {
    Serial.begin(9600);
    
    // Initialize hardware
    octocontroller.begin();
    teensycontroller = new CTeensy4Controller<GRB, WS2811_800kHz>(&octocontroller);
    FastLED.addLeds(teensycontroller, rgbarray, numberofpins * ledsperstrip);
    FastLED.setBrightness(120);
    
    // Initialize edge data
    initedgedata();
    
    // *** NEW: Register stateful color functions FIRST ***
    Serial.println("Initializing stateful color functions...");
    
    // Fire2012 variants
    fire2012Standard = new Fire2012ColorFunction("fire2012", 55, 120);
    registerStatefulColorFunction(8, fire2012Standard);
    
    fire2012Blue = new Fire2012ColorFunction("fire2012_blue", 45, 100, false,
        CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::White));
    registerStatefulColorFunction(9, fire2012Blue);
    
    fire2012Green = new Fire2012ColorFunction("fire2012_green", 50, 110, false,
        CRGBPalette16(CRGB::Black, CRGB::Green, CRGB::LimeGreen, CRGB::Yellow));
    registerStatefulColorFunction(10, fire2012Green);
    
    // Optional: Register advanced functions
    // registerAdvancedColorFunctions();
    
    Serial.println("Stateful color functions initialized.");
    
    // Initialize models (AFTER stateful functions)
    initializemodels();
    
    // Set color functions on all models
    for(int modelIdx = 0; modelIdx < colormodel::getNumRegisteredModels(); modelIdx++) {
        colormodel* model = colormodel::getModelRegistry()[modelIdx];
        for(int funcIdx = 0; funcIdx < numcolorfunctions; funcIdx++) {
            model->setColorFunction(funcIdx, colorFunctionNames[funcIdx], 
                                   colorFunctionArray[funcIdx]);
        }
    }
    
    // Create merged/permuted models...
    // Register sequences...
    // etc.
}
```

### 3. Main Loop

```cpp
void loop() {
    unsigned long currentTime = millis();
    
    // *** CRITICAL: Update stateful functions FIRST ***
    updateAllStatefulColorFunctions(currentTime);
    
    // Then update sequences
    mainSequence.updateRegistry(currentTime);
    mainSequence.update(currentTime, colorFunctionArray);
    
    // Render LEDs
    colormodel* currentModel = mainSequence.getCurrentModel();
    if(currentModel != nullptr) {
        const std::array<std::array<int, 6>, 120>& edgeModelData = 
            currentModel->getEdgeModels();
        
        for(int i = 0; i < numberofleds; i++) {
            // ... your rendering code ...
            rgbarray[i] = mainSequence.getColor(edgeType, position);
        }
    }
    
    FastLED.show();
    delay(10);
}
```

## Using Fire2012 in Sequences

In `sequences.cpp`:

```cpp
void initializeSequences(modelsequence* seq) {
    seq->clearRegistry();
    
    // Fire showcase sequence
    seq->startNewSequence("Fire Effects", 60, true);
    
    // All fire
    seq->addStepByName("flowoctahedron", 
        {"dark", "fire2012", "fire2012", "fire2012"}, 
        10, FADE, 2);
    
    // Blue fire
    seq->addStepByName("cycle", 
        {"dark", "fire2012_blue", "fire2012_blue", "rainbow"}, 
        10, FADE, 2);
    
    // Mixed colors
    seq->addStepByName("octachain", 
        {"dark", "fire2012", "fire2012_blue", "fire2012_green"}, 
        10, WIPE, 2);
    
    // More sequences...
}
```

## Adjusting Parameters at Runtime

### Fire2012 Parameters

```cpp
// In setup() or during runtime:
fire2012Standard->setCooling(40);    // Less cooling = taller flames
fire2012Standard->setSparking(150);  // More sparks = more active fire

// Change direction
fire2012Standard->setDirection(true); // Reverse

// Change palette
CRGBPalette16 purpleFire = CRGBPalette16(
    CRGB::Black, CRGB::Purple, CRGB::Magenta, CRGB::Pink
);
fire2012Standard->setPalette(purpleFire);
```

### Advanced Effects (if using advanced_colorfunctions.h)

```cpp
// Plasma
plasmaWave->setSpeeds(0.1, 0.05, 0.08);  // Change wave speeds

// Sparkle
sparkleBlue->setSparkleRate(10);  // More sparkles
sparkleBlue->setFadeSpeed(20);    // Faster fade
sparkleBlue->setBaseColor(CHSV(200, 255, 160));  // Change color

// Larson Scanner
larsonRed->setSpeed(3.0);         // Faster scanning
larsonRed->setTailLength(12);     // Longer tail

// Ripple
rippleEffect->setSpawnRate(50);   // More frequent ripples
```

## Creating Custom Effects

### Simple Example: Breathing Effect

```cpp
class BreathingColorFunction : public StatefulColorFunction {
private:
    float phase;
    float speed;
    CHSV color;
    
public:
    BreathingColorFunction(String name, CHSV c, float s = 0.02)
        : StatefulColorFunction(name, 20), color(c), speed(s) {
        reset();
    }
    
    void reset() override {
        phase = 0;
    }
    
    void updateState() override {
        phase += speed;
        if(phase > TWO_PI) phase -= TWO_PI;
    }
    
    CRGB getColor(float position) override {
        // Breathing brightness
        float breath = (sin(phase) + 1.0) / 2.0;
        
        CHSV c = color;
        c.v = breath * MAXBRIGHTNESS;
        return c;
    }
};

// In setup():
BreathingColorFunction* breathingBlue = 
    new BreathingColorFunction("breathing_blue", CHSV(160, 255, 160));
registerStatefulColorFunction(20, breathingBlue);
colorFunctionArray[20] = [](float pos) { return callStatefulColorFunction(20, pos); };
colorFunctionNames[20] = "breathing_blue";
```

## Memory Usage

Each stateful function uses memory:
- Fire2012: ~128 bytes (heat array)
- Plasma: ~128 bytes (brightness array)
- Sparkle: ~128 bytes (brightness array)
- Larson: ~128 bytes (brightness array)
- Ripple: ~168 bytes (brightness array + ripple structs)

With 10 stateful functions: ~1.5KB total

Teensy 4.1 has 512KB RAM, so you have plenty of room!

## Troubleshooting

### "Effect not updating"
✓ Check `updateAllStatefulColorFunctions()` is called in loop()
✓ Verify function is registered with `registerStatefulColorFunction()`
✓ Check Serial output for registration messages

### "Colors are wrong"
✓ Ensure index in `registerStatefulColorFunction()` matches wrapper index
✓ Verify `colorFunctionArray` and `colorFunctionNames` are updated
✓ Check palette/color settings

### "Effect is jerky"
✓ Reduce `updateInterval` (e.g., from 50ms to 20ms)
✓ Check other code isn't delaying too long
✓ Monitor with Serial: `Serial.println(millis() - lastTime)`

### "Out of memory"
✓ Reduce `NUM_LEDS` in your effect (128 → 64)
✓ Limit number of simultaneous stateful effects
✓ Use `extern "C" char* sbrk(int incr);` to check free memory

## Performance Tips

1. **Update Rate**: Most effects look smooth at 20-50ms intervals
2. **Array Size**: 128 LEDs provides good resolution for edges
3. **Algorithm Efficiency**: Use integer math where possible
4. **Multiple Instances**: Fire2012 uses very little CPU per instance

## Next Steps

1. Try the existing Fire2012 effects in your sequences
2. Experiment with the advanced effects (plasma, sparkle, etc.)
3. Create your own stateful functions for custom effects
4. Combine stateful and stateless functions in sequences
5. Adjust parameters in real-time to find your perfect look

## Resources

- `ARCHITECTURE.md`: Detailed architecture documentation
- `colorfunctions.h`: Core stateful function class
- `advanced_colorfunctions.h`: Additional effect examples
- FastLED documentation: http://fastled.io/

Happy coding! 🎨✨
