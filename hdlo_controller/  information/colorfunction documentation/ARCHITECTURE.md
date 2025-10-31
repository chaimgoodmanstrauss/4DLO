# Stateful Color Function Architecture

## Overview

The HDLO LED controller now supports two types of color functions:

1. **Stateless Functions**: Simple inline functions that calculate color based on position (and optionally time via `millis()`)
2. **Stateful Functions**: Class-based functions that maintain internal state and update over time

## Architecture Design

### Simple Stateless Functions

These are the original color functions - simple and efficient for basic effects:

```cpp
inline CRGB rainbow(float position) {
    return CHSV(((int)(position * 255)), 255, MAXBRIGHTNESS);
}

inline CRGB pulsingblue(float position) {
    return CRGB(0, 0, (sin(millis() / 1000.) + 1) / 2 * MAXBRIGHTNESS);
}
```

**Use cases:**
- Static colors
- Simple gradients
- Time-based pulses
- Mathematical patterns

**Advantages:**
- Zero memory overhead
- Maximum performance
- Easy to write and understand

### Stateful Color Functions

For complex effects that need to maintain state between frames:

```cpp
class Fire2012ColorFunction : public StatefulColorFunction {
private:
    static const int NUM_LEDS = 128;  // Virtual LED array
    byte heat[NUM_LEDS];              // State: heat values
    int cooling, sparking;             // Parameters
    CRGBPalette16 palette;            // Color palette
    
public:
    CRGB getColor(float position) override {
        // Map position to virtual LED array
        int ledIndex = (int)(position * (NUM_LEDS - 1));
        byte temperature = heat[ledIndex];
        return ColorFromPalette(palette, temperature);
    }
    
    void updateState() override {
        // Run fire simulation algorithm
        updateFire();
    }
};
```

**Use cases:**
- Fire effects (Fire2012, Fire2018, etc.)
- Particle systems
- Cellular automata (Conway's Life, etc.)
- Wave simulations
- Reaction-diffusion systems
- Any effect requiring memory of previous states

**Advantages:**
- Full control over internal state
- Can implement complex algorithms
- Adjustable parameters at runtime
- Multiple instances with different configurations

## How It Works

### 1. Registration System

Stateful functions register themselves in a global registry:

```cpp
// In setup():
Fire2012ColorFunction* fire2012 = new Fire2012ColorFunction("fire2012", 55, 120);
registerStatefulColorFunction(8, fire2012);  // Register at index 8
```

This makes them available through the standard ColorFunction interface.

### 2. Update Loop

In your main `loop()`, call `updateAllStatefulColorFunctions()` FIRST:

```cpp
void loop() {
    unsigned long currentTime = millis();
    
    // CRITICAL: Update stateful functions first
    updateAllStatefulColorFunctions(currentTime);
    
    // Then update sequences and render
    mainSequence.update(currentTime, colorFunctionArray);
    // ... render LEDs ...
}
```

This ensures all stateful effects update their internal state before colors are queried.

### 3. Transparent Usage

Once registered, stateful functions work exactly like stateless ones:

```cpp
// In sequences.cpp:
seq->addStepByName("flowoctahedron", 
    {"dark", "fire2012", "fire2012", "fire2012"}, 
    10, FADE, 2);
```

The system automatically routes to the correct implementation.

## Creating Your Own Stateful Color Functions

### Template

```cpp
class MyEffectColorFunction : public StatefulColorFunction {
private:
    // Your state variables
    static const int ARRAY_SIZE = 128;
    byte myStateArray[ARRAY_SIZE];
    float myParameter;
    
public:
    MyEffectColorFunction(String name, float param)
        : StatefulColorFunction(name, 20),  // Update every 20ms
          myParameter(param) {
        reset();
    }
    
    void reset() override {
        // Initialize state to defaults
        for(int i = 0; i < ARRAY_SIZE; i++) {
            myStateArray[i] = 0;
        }
    }
    
    void updateState() override {
        // This runs periodically (every updateInterval ms)
        // Update your state arrays here
        for(int i = 0; i < ARRAY_SIZE; i++) {
            myStateArray[i] = /* your algorithm */;
        }
    }
    
    CRGB getColor(float position) override {
        // Map position (0.0-1.0) to your state array
        int index = (int)(position * (ARRAY_SIZE - 1));
        index = constrain(index, 0, ARRAY_SIZE - 1);
        
        // Return color based on state
        byte value = myStateArray[index];
        return CRGB(value, value, value);  // Example: grayscale
    }
    
    // Optional: Add setters for runtime parameter adjustment
    void setParameter(float value) { myParameter = value; }
};
```

### Registration

In `hdlo_controller.ino` setup:

```cpp
MyEffectColorFunction* myEffect = new MyEffectColorFunction("myeffect", 1.5);
registerStatefulColorFunction(11, myEffect);  // Choose an available index

// Update colorFunctionArray to include a wrapper
colorFunctionArray[11] = [](float pos) { 
    return callStatefulColorFunction(11, pos); 
};
colorFunctionNames[11] = "myeffect";
```

## Memory Considerations

### Virtual LED Array Size

The `NUM_LEDS` constant in stateful functions defines a virtual LED array that maps to edge positions:

- **Smaller arrays (64)**: Less memory, lower resolution, faster updates
- **Larger arrays (256)**: More memory, higher resolution, smoother effects
- **Recommended: 128**: Good balance for most effects

Each edge color function gets its own virtual array, so memory usage scales with the number of stateful functions.

### Update Frequency

Control update rate with the `updateInterval` parameter:

```cpp
StatefulColorFunction("name", 20)  // Update every 20ms (50 FPS)
StatefulColorFunction("name", 50)  // Update every 50ms (20 FPS)
StatefulColorFunction("name", 100) // Update every 100ms (10 FPS)
```

Slower update rates save CPU time but may appear less smooth.

## Available Stateful Functions

### Fire2012

Three variants with different color palettes:

1. **fire2012** (index 8): Classic red/orange/yellow fire
2. **fire2012_blue** (index 9): Blue/cyan/white fire
3. **fire2012_green** (index 10): Green/lime/yellow fire

**Parameters:**
- `cooling` (default 55): How quickly fire cools (0-255)
- `sparking` (default 120): How often new sparks ignite (0-255)
- `reverseDirection`: Flip the fire direction

**Runtime adjustment:**
```cpp
fire2012Standard->setCooling(40);    // Slower cooling = taller flames
fire2012Standard->setSparking(150);  // More sparking = more active
```

## Best Practices

### 1. Initialize Before Models

Create and register stateful functions BEFORE initializing models:

```cpp
void setup() {
    // 1. Initialize stateful functions
    Fire2012ColorFunction* fire = new Fire2012ColorFunction(...);
    registerStatefulColorFunction(8, fire);
    
    // 2. Then initialize models
    initializemodels();
    
    // 3. Set color functions on models
    for(int i = 0; i < colormodel::getNumRegisteredModels(); i++) {
        // ...
    }
}
```

### 2. Update Order

Always update stateful functions before rendering:

```cpp
void loop() {
    updateAllStatefulColorFunctions(millis());  // First
    mainSequence.update(millis(), colorFunctionArray);  // Then
    // Render...
}
```

### 3. Memory Management

Stateful functions are created with `new` and should persist for the program lifetime:

```cpp
// Global scope (good)
Fire2012ColorFunction* fire2012Standard = nullptr;

void setup() {
    fire2012Standard = new Fire2012ColorFunction(...);
    // No need to delete - lives for entire program
}
```

### 4. Index Management

Keep track of which indices are used:

```cpp
// colorfunctions.cpp
// 0-7: Original functions
// 8-10: Fire2012 variants
// 11-99: Available for new functions
```

## Future Extensions

The architecture supports many effect types:

### Particle Systems
```cpp
class ParticleColorFunction : public StatefulColorFunction {
    struct Particle { float position, velocity; CRGB color; };
    Particle particles[50];
    // Simulate physics, collisions, etc.
};
```

### Cellular Automata
```cpp
class LifeColorFunction : public StatefulColorFunction {
    bool cells[128];
    // Conway's Game of Life on an edge
};
```

### Wave Equations
```cpp
class WaveColorFunction : public StatefulColorFunction {
    float amplitude[128];
    float velocity[128];
    // Simulate wave physics
};
```

### Audio Reactive
```cpp
class AudioReactiveColorFunction : public StatefulColorFunction {
    byte spectrum[128];
    // FFT analysis, beat detection, etc.
};
```

## Performance Notes

- Stateful functions add minimal overhead per LED
- Most CPU time is in `updateState()`, which runs at a controlled rate
- With 3 Fire2012 instances updating at 50fps, CPU usage is <5%
- You can easily run 10+ stateful effects simultaneously

## Troubleshooting

### Colors not updating
- Check that `updateAllStatefulColorFunctions()` is called in loop()
- Verify the function is registered correctly
- Check `updateInterval` isn't too long

### Memory issues
- Reduce `NUM_LEDS` in your effect class
- Limit number of simultaneous stateful effects
- Monitor free RAM with `Serial.println(freeMemory())`

### Choppy animation
- Reduce `updateInterval` for smoother updates
- Optimize your `updateState()` algorithm
- Consider using fixed-point math instead of floats

## Summary

This architecture provides:

✅ **Flexibility**: Simple functions for basic effects, classes for complex ones  
✅ **Performance**: No overhead for simple functions, controlled updates for complex ones  
✅ **Ease of use**: Both types work identically from the sequence system  
✅ **Extensibility**: Easy to add new effect types  
✅ **Maintainability**: Clear separation of concerns

You now have a robust system for creating both simple and sophisticated LED effects!
