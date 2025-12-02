////////////////////////////////////
//
//   modelsequence.h
//
// Sequence management system for HDLO controller
// Refactored: Fixed-size structures to avoid heap fragmentation
//
#ifndef MODELSEQUENCE_H
#define MODELSEQUENCE_H

#include "models.h"
#include "colorfunctions.h"
#include <array>
#include <vector>
#include <map>

// Limits - tuned for Teensy 4.x memory
// FunctionWithPalette: 20+20+24+4 = 68 bytes
// SequenceStep: ~1600 bytes, 150 steps = ~240KB (fits in 512KB RAM)
static const int SEQ_MAX_STEPS = 150;
static const int SEQ_MAX_REGISTRY = 20;
static const int SEQ_MAX_FUNCTIONS = 11;  // Must match model edge function indices
static const int SEQ_MAX_PARAMS = 6;
static const int SEQ_MAX_NAME_LEN = 20;   // Enough for "ocean_builtin" etc

// Audio switching thresholds (defined in sequences.cpp)
extern const float AUDIO_PALETTE_SWITCH_THRESHOLD;
extern const float AUDIO_TIMEOUT_SECONDS;

// Audio source configuration - fixed size
struct AudioSourceConfig {
    enum SourceType { MICROPHONE, SD_CARD, LINE_IN };
    SourceType type;
    char filename[SEQ_MAX_NAME_LEN];
    bool loop;
    
    AudioSourceConfig() : type(MICROPHONE), loop(false) {
        filename[0] = '\0';
    }
    AudioSourceConfig(SourceType t, const char* fn = "", bool l = false) 
        : type(t), loop(l) {
        strncpy(filename, fn ? fn : "", SEQ_MAX_NAME_LEN - 1);
        filename[SEQ_MAX_NAME_LEN - 1] = '\0';
    }
    AudioSourceConfig(SourceType t, const String& fn, bool l = false) 
        : type(t), loop(l) {
        strncpy(filename, fn.c_str(), SEQ_MAX_NAME_LEN - 1);
        filename[SEQ_MAX_NAME_LEN - 1] = '\0';
    }
};

// Transition types
enum TransitionType {
    INSTANT,
    FADE,
    WIPE
};

// Function with palette - FIXED SIZE, no heap allocation
struct FunctionWithPalette {
    char functionName[SEQ_MAX_NAME_LEN];
    char paletteName[SEQ_MAX_NAME_LEN];
    float params[SEQ_MAX_PARAMS];
    uint8_t numParams;
    
    FunctionWithPalette() : numParams(0) {
        strcpy(functionName, "breathing");
        paletteName[0] = '\0';
        memset(params, 0, sizeof(params));
    }
    
    FunctionWithPalette(const char* fn, const char* pn = "") : numParams(0) {
        strncpy(functionName, fn ? fn : "breathing", SEQ_MAX_NAME_LEN - 1);
        functionName[SEQ_MAX_NAME_LEN - 1] = '\0';
        strncpy(paletteName, pn ? pn : "", SEQ_MAX_NAME_LEN - 1);
        paletteName[SEQ_MAX_NAME_LEN - 1] = '\0';
        memset(params, 0, sizeof(params));
    }
    
    FunctionWithPalette(const String& fn, const String& pn = "") : numParams(0) {
        strncpy(functionName, fn.c_str(), SEQ_MAX_NAME_LEN - 1);
        functionName[SEQ_MAX_NAME_LEN - 1] = '\0';
        strncpy(paletteName, pn.c_str(), SEQ_MAX_NAME_LEN - 1);
        paletteName[SEQ_MAX_NAME_LEN - 1] = '\0';
        memset(params, 0, sizeof(params));
    }
    
    FunctionWithPalette(const String& fn, const String& pn, std::initializer_list<float> p) : numParams(0) {
        strncpy(functionName, fn.c_str(), SEQ_MAX_NAME_LEN - 1);
        functionName[SEQ_MAX_NAME_LEN - 1] = '\0';
        strncpy(paletteName, pn.c_str(), SEQ_MAX_NAME_LEN - 1);
        paletteName[SEQ_MAX_NAME_LEN - 1] = '\0';
        memset(params, 0, sizeof(params));
        for(float v : p) {
            if(numParams < SEQ_MAX_PARAMS) params[numParams++] = v;
        }
    }
    
    // Build FunctionParameter vector on-demand (for API compatibility)
    std::vector<FunctionParameter> getParameters() const {
        std::vector<FunctionParameter> result;
        result.reserve(numParams);
        for(int i = 0; i < numParams; i++) {
            result.push_back(FunctionParameter(params[i]));
        }
        return result;
    }
};

// Sequence step structure - FIXED SIZE
struct SequenceStep {
    static const int MAX_FUNCTIONS = SEQ_MAX_FUNCTIONS;
    
    colormodel* model;
    
    // Dual palette system - fixed arrays
    std::array<FunctionWithPalette, SEQ_MAX_FUNCTIONS> audioPalettes;
    std::array<FunctionWithPalette, SEQ_MAX_FUNCTIONS> backgroundPalettes;
    int numAudioFunctions;
    int numBackgroundFunctions;
    
    // Audio control
    bool acceptAudio;
    float audioThreshold;
    float audioTimeout;
    
    unsigned long duration;
    TransitionType transitionType;
    unsigned long transitionDuration;
    float transitionSpeed;
    AudioSourceConfig audioConfig;
    
    SequenceStep();
    
    // Legacy constructor
    SequenceStep(colormodel* m,
                 const std::vector<FunctionWithPalette>& funcs,
                 unsigned long dur,
                 TransitionType trans = INSTANT,
                 float transSpeed = 1.0f,
                 unsigned long transDur = 0,
                 AudioSourceConfig audio = AudioSourceConfig());
    
    // Dual palette constructor
    SequenceStep(colormodel* m,
                 const std::vector<FunctionWithPalette>& audioFuncs,
                 const std::vector<FunctionWithPalette>& backgroundFuncs,
                 unsigned long dur,
                 bool acceptAud = true,
                 float audThresh = 0.020f,
                 float audTimeout = 2.0f,
                 TransitionType trans = INSTANT,
                 float transSpeed = 1.0f,
                 AudioSourceConfig audio = AudioSourceConfig());
    
private:
    void initializeDefaultPalettes();
    void initializeDefaultAudioPalette();
};

// Sequence registry entry - FIXED SIZE
struct SequenceRegistryEntry {
    int startStepIndex;
    int numSteps;
    char name[SEQ_MAX_NAME_LEN];
    unsigned long duration;
    bool enabled;
    
    SequenceRegistryEntry()
        : startStepIndex(0), numSteps(0), duration(60000), enabled(false) {
        name[0] = '\0';
    }
    
    SequenceRegistryEntry(int start, int count, const String& n, unsigned long dur, bool en = true)
        : startStepIndex(start), numSteps(count), duration(dur), enabled(en) {
        strncpy(name, n.c_str(), SEQ_MAX_NAME_LEN - 1);
        name[SEQ_MAX_NAME_LEN - 1] = '\0';
    }
};

// Main sequence class
class modelsequence {
private:
    // Large arrays stored in EXTMEM (8MB PSRAM on Teensy 4.1)
    // Declare as pointers, allocate in constructor
    SequenceStep* steps;  // Allocated in EXTMEM
    int numSteps;
    int currentStep;
    int previousStep;
    unsigned long stepStartTime;
    
    SequenceRegistryEntry registry[SEQ_MAX_REGISTRY];  // Small, stays in RAM1
    int numRegistryEntries;
    int currentRegistryIndex;
    unsigned long registryStartTime;
    
    bool inTransition;
    unsigned long transitionStartTime;
    float transitionProgress;
    
    // Audio palette fading state
    bool audioActive;
    unsigned long audioLastActiveTime;
    bool audioFading;
    float audioFadeProgress;
    float audioFadeStartProgress;
    unsigned long audioFadeStartTime;
    bool fadingToAudio;
    static constexpr float AUDIO_FADE_IN_TIME = 0.2f;
    
    // Sequence-level function cache (hash-based to avoid String allocation on lookup)
    std::map<uint32_t, std::shared_ptr<StatefulColorFunction>> sequenceFunctionCache;
    
    // Cached color functions for audio fade blending
    std::shared_ptr<StatefulColorFunction> cachedBgFunctions[SEQ_MAX_FUNCTIONS];
    std::shared_ptr<StatefulColorFunction> cachedAudioFunctions[SEQ_MAX_FUNCTIONS];
    bool audioCacheValid;
    int cachedStepIndex;
    
    std::shared_ptr<StatefulColorFunction> getCachedFunction(const FunctionWithPalette& func);
    
    void applyFunctionsToModel();
    void configureAudioSource(const AudioSourceConfig& config);
    void updateAudioFade(const SequenceStep& step);
    void updateAudioFadeCache();
    
public:
    modelsequence();
    
    // Step management
    void addStep(const SequenceStep& step);
    void clearSteps();
    
    // Registry management
    void beginRegistry(String name, float durationSeconds = 60.0, bool enabled = true);
    void endRegistry();
    int getNumRegistryEntries() const { return numRegistryEntries; }
    const char* getCurrentRegistryName() const;
    
    // Playback control
    void update();
    void updateCachedFunctions();
    colormodel* getCurrentModel();
    CRGB getColor(int edgeindex, float position);
    
    // Status
    int getCurrentStep() const;
    int getNumSteps() const { return numSteps; }
    float getProgress() const;
    bool isInTransition() const { return inTransition; }
    float getTransitionProgress() const { return transitionProgress; }
    
    void reset();
    void printSequenceInfo();
    void printCacheStats();
    void clearFunctionCache();
};

////////////////////////////////////
// SEQUENCE BUILDER HELPER

struct FunctionDef {
    String functionName;
    String paletteName;
    std::vector<float> params;
    
    FunctionDef(const char* fn) : functionName(fn), paletteName("") {}
    FunctionDef(const String& fn) : functionName(fn), paletteName("") {}
    FunctionDef(const String& fn, const String& pn) : functionName(fn), paletteName(pn) {}
    FunctionDef(const String& fn, const String& pn, float p1) 
        : functionName(fn), paletteName(pn), params{p1} {}
    FunctionDef(const String& fn, const String& pn, float p1, float p2) 
        : functionName(fn), paletteName(pn), params{p1, p2} {}
    FunctionDef(const String& fn, const String& pn, float p1, float p2, float p3) 
        : functionName(fn), paletteName(pn), params{p1, p2, p3} {}
    FunctionDef(const String& fn, const String& pn, float p1, float p2, float p3, float p4) 
        : functionName(fn), paletteName(pn), params{p1, p2, p3, p4} {}
    FunctionDef(const String& fn, const String& pn, float p1, float p2, float p3, float p4, float p5) 
        : functionName(fn), paletteName(pn), params{p1, p2, p3, p4, p5} {}
    FunctionDef(const String& fn, const String& pn, float p1, float p2, float p3, float p4, float p5, float p6) 
        : functionName(fn), paletteName(pn), params{p1, p2, p3, p4, p5, p6} {}
};

class SequenceBuilder {
private:
    modelsequence* seq;
    
    std::vector<FunctionDef> currentAudioPalettes;
    std::vector<FunctionDef> currentBackgroundPalettes;
    std::vector<FunctionDef> currentPalettes;
    bool useDualPalettes;
    float audioTimeoutSeconds;
    AudioSourceConfig currentAudioSource;
    
    std::map<String, colormodel*> permutedModelCache;
    
    FunctionWithPalette buildFunctionWithPalette(const FunctionDef& def);
    
public:
    SequenceBuilder(modelsequence* s);
    
    void setaudiosource(AudioSourceConfig config);
    void setaudiopalette(std::initializer_list<FunctionDef> funcs);
    void setbackgroundpalette(std::initializer_list<FunctionDef> funcs);
    void setaudiotimeout(float seconds);
    
    void addpalette(std::initializer_list<FunctionDef> funcs);
    void clearpalettes();
    
    // duration in SECONDS
    void addstep(String modelName, float durationSeconds, 
                 TransitionType transition = INSTANT, float speed = 1.0,
                 bool acceptAudio = true);
    
    void addstep(String modelName, String permName, float durationSeconds, 
                 TransitionType transition = INSTANT, float speed = 1.0,
                 bool acceptAudio = true);
    
    void add(String modelName, std::initializer_list<FunctionDef> funcDefs, float durationSeconds);
};

#endif // MODELSEQUENCE_H
