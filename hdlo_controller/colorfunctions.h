////////////////////////////////////////////
//
// colorfunctions.h
//
// ARCHITECTURE:
// - IColorFunction base interface
// - ColorFunctionBase<T> CRTP for auto-registration
// - Each function class includes REGISTER_SELF() for automatic factory registration
// - To add: Define class with REGISTER_SELF()
// - To remove: Delete class definition
//
#ifndef COLORFUNCTIONS_H
#define COLORFUNCTIONS_H

#include <FastLED.h>
#include <Arduino.h>
#include "models.h"
#include "audiosystem.h"
#include "paletteregistry.h"
#include <vector>

const int MAXBRIGHTNESS = 160;

/////////////////////////////////////////
// PARAMETER SYSTEM
//
struct FunctionParameter {
    enum Type { FLOAT, INT, STRING };
    Type type;
    union {
        float floatVal;
        int intVal;
    };
    String stringVal;
    
    FunctionParameter(float f) : type(FLOAT), floatVal(f) {}
    FunctionParameter(int i) : type(INT), intVal(i) {}
    FunctionParameter(String s) : type(STRING), stringVal(s) { intVal = 0; }
};

/////////////////////////////////////////
// BASE INTERFACE
//
class IColorFunction {
protected:
    String name;
    unsigned long lastUpdateTime;
    unsigned int updateInterval;
    unsigned long currentFrameNumber;
    static unsigned long globalFrameNumber;
    String paletteName;
    
public:
    IColorFunction(String functionName, unsigned int updateIntervalMs = 20, String palette = "")
        : name(functionName), lastUpdateTime(0), updateInterval(updateIntervalMs),
          currentFrameNumber(0), paletteName(palette) {}
    
    virtual ~IColorFunction() {}
    
    virtual CRGB getColor(float position) = 0;
    virtual void setParameters(const std::vector<FunctionParameter>& params) {}
    virtual IColorFunction* clone() const = 0;
    
    void updateIfNeeded(unsigned long currentTime) {
        if(updateInterval > 0 && (currentTime - lastUpdateTime >= updateInterval)) {
            update();
            lastUpdateTime = currentTime;
        }
    }
    
    virtual void update() {}
    
    void setPalette(String palette) { paletteName = palette; }
    String getPalette() const { return paletteName; }
    String getName() const { return name; }
    
    static void incrementGlobalFrame() { globalFrameNumber++; }
    static unsigned long getGlobalFrame() { return globalFrameNumber; }
};

/////////////////////////////////////////
// CRTP AUTO-REGISTRATION BASE
//
template<typename Derived>
class ColorFunctionBase : public IColorFunction {
protected:
    struct Registrar {
        Registrar(const String& name) {
            ColorFunctionFactory::getInstance().registerFunction(name,
                []() { return new Derived(); });
        }
    };
    
public:
    ColorFunctionBase(String functionName, unsigned int updateIntervalMs = 20, String palette = "")
        : IColorFunction(functionName, updateIntervalMs, palette) {}
};

#define REGISTER_SELF(ClassName, name) \
    static inline Registrar _registrar{name};

/////////////////////////////////////////
// 0. DARK
//
class DarkColorFunction : public ColorFunctionBase<DarkColorFunction> {
private:
    REGISTER_SELF(DarkColorFunction, "dark");
    
public:
    DarkColorFunction(String functionName = "dark")
        : ColorFunctionBase(functionName, 0, "") {}
    
    CRGB getColor(float position) override {
        return CRGB::Black;
    }
    
    IColorFunction* clone() const override {
        return new DarkColorFunction(*this);
    }
};

/////////////////////////////////////////
// 1. RAINBOW
//
class RainbowColorFunction : public ColorFunctionBase<RainbowColorFunction> {
private:
    float speed;
    float cycleTime;
    REGISTER_SELF(RainbowColorFunction, "rainbow");
    
public:
    RainbowColorFunction(String functionName = "rainbow", float spd = 1.0)
        : ColorFunctionBase(functionName, 0, ""),
          speed(spd),
          cycleTime(5.0) {}
    
    CRGB getColor(float position) override {
        float timeOffset = (millis() / 1000.0) * speed / cycleTime;
        uint8_t hue = (position + timeOffset) * 255;
        return CHSV(hue, 255, 255);
    }
    
    void setParameters(const std::vector<FunctionParameter>& params) override {
        if(params.size() > 0 && params[0].type == FunctionParameter::FLOAT) {
            speed = constrain(params[0].floatVal, 0.1f, 10.0f);
        }
    }
    
    IColorFunction* clone() const override {
        return new RainbowColorFunction(*this);
    }
};

/////////////////////////////////////////
// 2. CYLON
//
class CylonColorFunction : public ColorFunctionBase<CylonColorFunction> {
private:
    float position;
    float direction;
    float speed;
    int tailLength;
    REGISTER_SELF(CylonColorFunction, "cylon");
    
public:
    CylonColorFunction(String functionName = "cylon", float spd = 0.02)
        : ColorFunctionBase(functionName, 20, "fire"),
          position(0.0),
          direction(1.0),
          speed(spd),
          tailLength(10) {}
    
    void update() override {
        position += direction * speed;
        if(position >= 1.0) {
            position = 1.0;
            direction = -1.0;
        } else if(position <= 0.0) {
            position = 0.0;
            direction = 1.0;
        }
    }
    
    CRGB getColor(float pos) override {
        float distance = abs(pos - position);
        if(distance < 0.01) {
            return ColorFromPalette(PaletteRegistry::getPalette(paletteName), 255, 255);
        } else if(distance < tailLength * 0.01) {
            uint8_t brightness = 255 * (1.0 - distance / (tailLength * 0.01));
            return ColorFromPalette(PaletteRegistry::getPalette(paletteName), 
                                   (uint8_t)(pos * 255), brightness);
        }
        return CRGB::Black;
    }
    
    void setParameters(const std::vector<FunctionParameter>& params) override {
        if(params.size() > 0 && params[0].type == FunctionParameter::FLOAT) {
            speed = constrain(params[0].floatVal, 0.001f, 0.1f);
        }
    }
    
    IColorFunction* clone() const override {
        return new CylonColorFunction(*this);
    }
};

/////////////////////////////////////////
// 3. FIRE2012
//
class Fire2012ColorFunction : public ColorFunctionBase<Fire2012ColorFunction> {
private:
    static const int NUM_LEDS = 128;
    byte heat[NUM_LEDS];
    int cooling;
    int sparking;
    bool gReverseDirection;
    REGISTER_SELF(Fire2012ColorFunction, "fire2012");
    
    void updateFire() {
        for(int i = 0; i < NUM_LEDS; i++) {
            heat[i] = qsub8(heat[i], random8(0, ((cooling * 10) / NUM_LEDS) + 2));
        }
        
        for(int k = NUM_LEDS - 1; k >= 2; k--) {
            heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
        }
        
        if(random8() < sparking) {
            int y = random8(7);
            heat[y] = qadd8(heat[y], random8(160, 255));
        }
    }
    
public:
    Fire2012ColorFunction(String functionName = "fire2012", 
                          int coolingValue = 55,
                          int sparkingValue = 120,
                          bool reverseDirection = false,
                          String palette = "fire")
        : ColorFunctionBase(functionName, 20, palette),
          cooling(coolingValue),
          sparking(sparkingValue),
          gReverseDirection(reverseDirection) {
        for(int i = 0; i < NUM_LEDS; i++) {
            heat[i] = 0;
        }
    }
    
    void update() override {
        updateFire();
    }
    
    CRGB getColor(float position) override {
        int index = position * NUM_LEDS;
        if(index >= NUM_LEDS) index = NUM_LEDS - 1;
        if(gReverseDirection) index = NUM_LEDS - 1 - index;
        
        byte temperature = heat[index];
        return ColorFromPalette(PaletteRegistry::getPalette(paletteName), temperature, 255);
    }
    
    void setParameters(const std::vector<FunctionParameter>& params) override {
        if(params.size() > 0 && params[0].type == FunctionParameter::FLOAT) {
            cooling = constrain((int)params[0].floatVal, 20, 100);
        }
        if(params.size() > 1 && params[1].type == FunctionParameter::FLOAT) {
            sparking = constrain((int)params[1].floatVal, 50, 200);
        }
    }
    
    IColorFunction* clone() const override {
        return new Fire2012ColorFunction(*this);
    }
};

/////////////////////////////////////////
// 4. AUDIOCYLON
//
class AudioCylonColorFunction : public ColorFunctionBase<AudioCylonColorFunction> {
private:
    float position;
    float direction;
    float baseSpeed;
    int tailLength;
    static const int NUM_BINS = 16;
    float binSmoothing[NUM_BINS];
    REGISTER_SELF(AudioCylonColorFunction, "audiocylon");
    
public:
    AudioCylonColorFunction(String functionName = "audiocylon")
        : ColorFunctionBase(functionName, 20, "rainbow"),
          position(0.0),
          direction(1.0),
          baseSpeed(0.01),
          tailLength(15) {
        for(int i = 0; i < NUM_BINS; i++) {
            binSmoothing[i] = 0.0;
        }
    }
    
    void update() override {
        // Get audio level for speed
        float audioLevel = AudioSystem::getLevel();
        float speed = baseSpeed * (1.0 + audioLevel * 3.0);
        
        // Smooth frequency bins
        for(int i = 0; i < NUM_BINS; i++) {
            float binVal = AudioSystem::getBin(i);
            binSmoothing[i] = binSmoothing[i] * 0.7 + binVal * 0.3;
        }
        
        position += direction * speed;
        if(position >= 1.0) {
            position = 1.0;
            direction = -1.0;
        } else if(position <= 0.0) {
            position = 0.0;
            direction = 1.0;
        }
    }
    
    CRGB getColor(float pos) override {
        float distance = abs(pos - position);
        
        // Find dominant frequency bin
        int dominantBin = 0;
        float maxEnergy = 0;
        for(int i = 0; i < NUM_BINS; i++) {
            if(binSmoothing[i] > maxEnergy) {
                maxEnergy = binSmoothing[i];
                dominantBin = i;
            }
        }
        
        uint8_t paletteIndex = (dominantBin * 255) / NUM_BINS;
        
        if(distance < 0.01) {
            return ColorFromPalette(PaletteRegistry::getPalette(paletteName), paletteIndex, 255);
        } else if(distance < tailLength * 0.01) {
            uint8_t brightness = 255 * (1.0 - distance / (tailLength * 0.01));
            return ColorFromPalette(PaletteRegistry::getPalette(paletteName), paletteIndex, brightness);
        }
        return CRGB::Black;
    }
    
    IColorFunction* clone() const override {
        return new AudioCylonColorFunction(*this);
    }
};

/////////////////////////////////////////
// 5. AUDIO
//
class AudioColorFunction : public ColorFunctionBase<AudioColorFunction> {
private:
    static const int NUM_BINS = 16;
    float binSmoothing[NUM_BINS];
    REGISTER_SELF(AudioColorFunction, "audio");
    
public:
    AudioColorFunction(String functionName = "audio")
        : ColorFunctionBase(functionName, 20, "rainbow") {
        for(int i = 0; i < NUM_BINS; i++) {
            binSmoothing[i] = 0.0;
        }
    }
    
    void update() override {
        for(int i = 0; i < NUM_BINS; i++) {
            float binVal = AudioSystem::getBin(i);
            binSmoothing[i] = binSmoothing[i] * 0.8 + binVal * 0.2;
        }
    }
    
    CRGB getColor(float position) override {
        int bin = position * NUM_BINS;
        if(bin >= NUM_BINS) bin = NUM_BINS - 1;
        
        float energy = binSmoothing[bin];
        uint8_t brightness = constrain(energy * 4000, 0, 160);
        
        return ColorFromPalette(PaletteRegistry::getPalette(paletteName), 
                               (uint8_t)(position * 255), brightness);
    }
    
    IColorFunction* clone() const override {
        return new AudioColorFunction(*this);
    }
};

/////////////////////////////////////////
// 6. VUMETER
//
class VUMeterColorFunction : public ColorFunctionBase<VUMeterColorFunction> {
private:
    float smoothedLevel;
    REGISTER_SELF(VUMeterColorFunction, "vumeter");
    
public:
    VUMeterColorFunction(String functionName = "vumeter")
        : ColorFunctionBase(functionName, 20, "fire"),
          smoothedLevel(0.0) {}
    
    void update() override {
        float level = AudioSystem::getLevel();
        smoothedLevel = smoothedLevel * 0.7 + level * 0.3;
    }
    
    CRGB getColor(float position) override {
        float threshold = smoothedLevel;
        if(position < threshold) {
            uint8_t brightness = 160;
            return ColorFromPalette(PaletteRegistry::getPalette(paletteName), 
                                   (uint8_t)(position * 255 / threshold), brightness);
        }
        return CRGB::Black;
    }
    
    IColorFunction* clone() const override {
        return new VUMeterColorFunction(*this);
    }
};

/////////////////////////////////////////
// 7. PLASMA
//
class PlasmaColorFunction : public ColorFunctionBase<PlasmaColorFunction> {
private:
    float speed;
    REGISTER_SELF(PlasmaColorFunction, "plasma");
    
public:
    PlasmaColorFunction(String functionName = "plasma", float spd = 1.0)
        : ColorFunctionBase(functionName, 0, "ocean"),
          speed(spd) {}
    
    CRGB getColor(float position) override {
        float t = millis() / 1000.0 * speed;
        float plasma = sin(position * 10.0 + t) + 
                      sin((position * 8.0 + t) * 0.5) +
                      sin((position * 12.0 + t) * 0.3);
        uint8_t paletteIndex = (plasma + 3.0) * 42.5;
        return ColorFromPalette(PaletteRegistry::getPalette(paletteName), paletteIndex, 255);
    }
    
    void setParameters(const std::vector<FunctionParameter>& params) override {
        if(params.size() > 0 && params[0].type == FunctionParameter::FLOAT) {
            speed = constrain(params[0].floatVal, 0.1f, 5.0f);
        }
    }
    
    IColorFunction* clone() const override {
        return new PlasmaColorFunction(*this);
    }
};

/////////////////////////////////////////
// 8. PARTICLES
//
class ParticlesColorFunction : public ColorFunctionBase<ParticlesColorFunction> {
private:
    struct Particle {
        float position;
        float velocity;
        uint8_t hue;
        bool active;
    };
    
    static const int NUM_PARTICLES = 5;
    Particle particles[NUM_PARTICLES];
    REGISTER_SELF(ParticlesColorFunction, "particles");
    
public:
    ParticlesColorFunction(String functionName = "particles")
        : ColorFunctionBase(functionName, 20, "rainbow") {
        for(int i = 0; i < NUM_PARTICLES; i++) {
            particles[i].position = random(100) / 100.0;
            particles[i].velocity = (random(20) - 10) / 1000.0;
            particles[i].hue = random(255);
            particles[i].active = true;
        }
    }
    
    void update() override {
        for(int i = 0; i < NUM_PARTICLES; i++) {
            if(particles[i].active) {
                particles[i].position += particles[i].velocity;
                if(particles[i].position > 1.0 || particles[i].position < 0.0) {
                    particles[i].velocity = -particles[i].velocity;
                    particles[i].position = constrain(particles[i].position, 0.0, 1.0);
                }
            }
        }
    }
    
    CRGB getColor(float position) override {
        CRGB color = CRGB::Black;
        for(int i = 0; i < NUM_PARTICLES; i++) {
            if(particles[i].active) {
                float distance = abs(position - particles[i].position);
                if(distance < 0.05) {
                    uint8_t brightness = 255 * (1.0 - distance / 0.05);
                    CRGB particleColor = ColorFromPalette(PaletteRegistry::getPalette(paletteName), 
                                                         particles[i].hue, brightness);
                    color += particleColor;
                }
            }
        }
        return color;
    }
    
    IColorFunction* clone() const override {
        return new ParticlesColorFunction(*this);
    }
};

/////////////////////////////////////////
// 9. BEATDETECT
//
class BeatDetectColorFunction : public ColorFunctionBase<BeatDetectColorFunction> {
private:
    float beatIntensity;
    unsigned long lastBeatTime;
    REGISTER_SELF(BeatDetectColorFunction, "beatdetect");
    
public:
    BeatDetectColorFunction(String functionName = "beatdetect")
        : ColorFunctionBase(functionName, 20, "fire"),
          beatIntensity(0.0),
          lastBeatTime(0) {}
    
    void update() override {
        float currentLevel = AudioSystem::getLevel();
        float beatThreshold = 0.3;
        
        if(currentLevel > beatThreshold && millis() - lastBeatTime > 200) {
            beatIntensity = 1.0;
            lastBeatTime = millis();
        }
        
        beatIntensity *= 0.95;
    }
    
    CRGB getColor(float position) override {
        uint8_t brightness = beatIntensity * 255;
        return ColorFromPalette(PaletteRegistry::getPalette(paletteName), 
                               (uint8_t)(position * 255), brightness);
    }
    
    IColorFunction* clone() const override {
        return new BeatDetectColorFunction(*this);
    }
};

/////////////////////////////////////////
// 10. VOCALS
//
class VocalsColorFunction : public ColorFunctionBase<VocalsColorFunction> {
private:
    static const int NUM_BINS = 16;
    float binSmoothing[NUM_BINS];
    float widthSmoothing;
    REGISTER_SELF(VocalsColorFunction, "vocals");
    
public:
    VocalsColorFunction(String functionName = "vocals")
        : ColorFunctionBase(functionName, 20, "rainbow"),
          widthSmoothing(0.0) {
        for(int i = 0; i < NUM_BINS; i++) {
            binSmoothing[i] = 0.0;
        }
    }
    
    void update() override {
        float totalLevel = 0;
        for(int i = 0; i < NUM_BINS; i++) {
            float binVal = AudioSystem::getBin(i);
            binSmoothing[i] = binSmoothing[i] * 0.7 + binVal * 0.3;
            totalLevel += binSmoothing[i];
        }
        widthSmoothing = widthSmoothing * 0.8 + totalLevel * 0.2;
    }
    
    CRGB getColor(float position) override {
        // Find dominant frequency
        int dominantBin = 0;
        float maxEnergy = 0;
        for(int i = 0; i < NUM_BINS; i++) {
            if(binSmoothing[i] > maxEnergy) {
                maxEnergy = binSmoothing[i];
                dominantBin = i;
            }
        }
        
        uint8_t paletteIndex = (dominantBin * 255) / NUM_BINS;
        
        // Width based on total level
        float width = widthSmoothing * 2.0;
        if(position < width) {
            uint8_t brightness = 160;
            return ColorFromPalette(PaletteRegistry::getPalette(paletteName), paletteIndex, brightness);
        }
        return CRGB::Black;
    }
    
    IColorFunction* clone() const override {
        return new VocalsColorFunction(*this);
    }
};

/////////////////////////////////////////
// 11. BREATHING
//
class BreathingColorFunction : public ColorFunctionBase<BreathingColorFunction> {
private:
    float speed;
    REGISTER_SELF(BreathingColorFunction, "breathing");
    
public:
    BreathingColorFunction(String functionName = "breathing", float spd = 1.0)
        : ColorFunctionBase(functionName, 0, "white"),
          speed(spd) {}
    
    CRGB getColor(float position) override {
        uint8_t bpm = 12 * speed;
        uint8_t brightness = beatsin8(bpm, 30, 255);
        return ColorFromPalette(PaletteRegistry::getPalette(paletteName), 0, brightness);
    }
    
    void setParameters(const std::vector<FunctionParameter>& params) override {
        if(params.size() > 0 && params[0].type == FunctionParameter::FLOAT) {
            speed = constrain(params[0].floatVal, 0.1f, 5.0f);
        }
    }
    
    IColorFunction* clone() const override {
        return new BreathingColorFunction(*this);
    }
};

/////////////////////////////////////////
// 12. SIMPLECOLOR
//
class SimpleColorFunction : public ColorFunctionBase<SimpleColorFunction> {
private:
    REGISTER_SELF(SimpleColorFunction, "simplecolor");
    
public:
    SimpleColorFunction(String functionName = "simplecolor")
        : ColorFunctionBase(functionName, 0, "rainbow") {}
    
    CRGB getColor(float position) override {
        return ColorFromPalette(PaletteRegistry::getPalette(paletteName), 
                               (uint8_t)(position * 255), 255);
    }
    
    IColorFunction* clone() const override {
        return new SimpleColorFunction(*this);
    }
};

/////////////////////////////////////////
// 13. AUDIO2
//
class Audio2ColorFunction : public ColorFunctionBase<Audio2ColorFunction> {
private:
    static const int NUM_BINS = 16;
    float binSmoothing[NUM_BINS];
    REGISTER_SELF(Audio2ColorFunction, "audio2");
    
public:
    Audio2ColorFunction(String functionName = "audio2")
        : ColorFunctionBase(functionName, 20, "ocean") {
        for(int i = 0; i < NUM_BINS; i++) {
            binSmoothing[i] = 0.0;
        }
    }
    
    void update() override {
        for(int i = 0; i < NUM_BINS; i++) {
            float binVal = AudioSystem::getBin(i);
            binSmoothing[i] = binSmoothing[i] * 0.85 + binVal * 0.15;
        }
    }
    
    CRGB getColor(float position) override {
        int bin = position * NUM_BINS;
        if(bin >= NUM_BINS) bin = NUM_BINS - 1;
        
        float energy = binSmoothing[bin];
        uint8_t brightness = constrain(energy * 5000, 0, 160);
        
        return ColorFromPalette(PaletteRegistry::getPalette(paletteName), 
                               (uint8_t)(bin * 16), brightness);
    }
    
    IColorFunction* clone() const override {
        return new Audio2ColorFunction(*this);
    }
};

/////////////////////////////////////////
// 14. FFTFIRE
//
class FFTFireColorFunction : public ColorFunctionBase<FFTFireColorFunction> {
private:
    static const int NUM_BINS = 16;
    float binSmoothing[NUM_BINS];
    REGISTER_SELF(FFTFireColorFunction, "fftfire");
    
public:
    FFTFireColorFunction(String functionName = "fftfire")
        : ColorFunctionBase(functionName, 20, "fire") {
        for(int i = 0; i < NUM_BINS; i++) {
            binSmoothing[i] = 0.0;
        }
    }
    
    void update() override {
        for(int i = 0; i < NUM_BINS; i++) {
            float binVal = AudioSystem::getBin(i);
            binSmoothing[i] = binSmoothing[i] * 0.75 + binVal * 0.25;
        }
    }
    
    CRGB getColor(float position) override {
        // Map position to frequency bins
        int bin1 = position * NUM_BINS;
        int bin2 = bin1 + 1;
        if(bin2 >= NUM_BINS) bin2 = NUM_BINS - 1;
        
        float frac = (position * NUM_BINS) - bin1;
        float energy = binSmoothing[bin1] * (1.0 - frac) + binSmoothing[bin2] * frac;
        
        uint8_t brightness = constrain(energy * 6000, 0, 255);
        
        // Lower frequencies = red/orange, higher = yellow/white
        uint8_t paletteIndex = position * 255;
        
        return ColorFromPalette(PaletteRegistry::getPalette(paletteName), paletteIndex, brightness);
    }
    
    IColorFunction* clone() const override {
        return new FFTFireColorFunction(*this);
    }
};

/////////////////////////////////////////
// 15. NOISEPERLIN
//
class NoisePerlinColorFunction : public ColorFunctionBase<NoisePerlinColorFunction> {
private:
    static const uint8_t WIDTH = 16;
    static const uint8_t HEIGHT = 16;
    uint8_t noise[WIDTH][HEIGHT];
    uint16_t x, y, z;
    uint16_t speed, scale;
    REGISTER_SELF(NoisePerlinColorFunction, "noiseperlin");
    
public:
    NoisePerlinColorFunction(String functionName = "noiseperlin")
        : ColorFunctionBase(functionName, 20, "ocean"),
          x(0), y(0), z(0),
          speed(25), scale(35) {}
    
    void update() override {
        z += speed;
        for(int i = 0; i < WIDTH; i++) {
            int ioffset = scale * i;
            for(int j = 0; j < HEIGHT; j++) {
                int joffset = scale * j;
                noise[i][j] = inoise8(x + ioffset, y + joffset, z);
            }
        }
        x += speed / 8;
    }
    
    CRGB getColor(float position) override {
        uint8_t i = position * WIDTH;
        if(i >= WIDTH) i = WIDTH - 1;
        
        // Sample brightness and color from different noise patterns
        uint8_t j = (millis() / 100) % HEIGHT;
        uint8_t colorIndex = noise[i][j];
        uint8_t brightness = noise[j][i];
        
        return ColorFromPalette(PaletteRegistry::getPalette(paletteName), colorIndex, brightness);
    }
    
    void setParameters(const std::vector<FunctionParameter>& params) override {
        if(params.size() > 0 && params[0].type == FunctionParameter::FLOAT) {
            speed = constrain((int)params[0].floatVal, 1, 100);
        }
        if(params.size() > 1 && params[1].type == FunctionParameter::FLOAT) {
            scale = constrain((int)params[1].floatVal, 10, 100);
        }
    }
    
    IColorFunction* clone() const override {
        return new NoisePerlinColorFunction(*this);
    }
};

#endif // COLORFUNCTIONS_H
