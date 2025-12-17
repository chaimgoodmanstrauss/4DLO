////////////////////////////////////
//
//   modelsequence.h
//
// Sequence management system for HDLO controller
// Uses integer IDs for all named entities
//
#ifndef MODELSEQUENCE_H
#define MODELSEQUENCE_H

#include "models.h"
#include "colorfunctions.h"
#include "globalids.h"
#include <array>
#include <vector>
#include <map>

// Limits
static const int SEQ_MAX_STEPS = 150;
static const int SEQ_MAX_REGISTRY = 20;
static const int SEQ_MAX_FUNCTIONS = 11;
static const int SEQ_MAX_PARAMS = 6;
static const int SEQ_MAX_NAME_LEN = 20;

// Audio switching thresholds
extern const float AUDIO_PALETTE_SWITCH_THRESHOLD;
extern const float AUDIO_TIMEOUT_SECONDS;

// Audio source configuration
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

enum TransitionType {
    INSTANT,
    FADE,
    WIPE
};

// Function with palette - uses integer IDs
struct FunctionWithPalette {
    int functionId;
    int paletteId;
    float params[SEQ_MAX_PARAMS];
    uint8_t numParams;
    
    FunctionWithPalette() : functionId(breathing), paletteId(0), numParams(0) {
        memset(params, 0, sizeof(params));
    }
    
    FunctionWithPalette(int funcId, int palId = 0) 
        : functionId(funcId), paletteId(palId), numParams(0) {
        memset(params, 0, sizeof(params));
    }
    
    FunctionWithPalette(int funcId, int palId, std::initializer_list<float> p) 
        : functionId(funcId), paletteId(palId), numParams(0) {
        memset(params, 0, sizeof(params));
        for(float v : p) {
            if(numParams < SEQ_MAX_PARAMS) params[numParams++] = v;
        }
    }
    
    std::vector<FunctionParameter> getParameters() const {
        std::vector<FunctionParameter> result;
        result.reserve(numParams);
        for(int i = 0; i < numParams; i++) {
            result.push_back(FunctionParameter(params[i]));
        }
        return result;
    }
};

// Sequence step structure
struct SequenceStep {
    static const int MAX_FUNCTIONS = SEQ_MAX_FUNCTIONS;
    
    colormodel* model;
    
    std::array<FunctionWithPalette, SEQ_MAX_FUNCTIONS> audioPalettes;
    std::array<FunctionWithPalette, SEQ_MAX_FUNCTIONS> backgroundPalettes;
    int numAudioFunctions;
    int numBackgroundFunctions;
    
    bool acceptAudio;
    float audioThreshold;
    float audioTimeout;
    
    unsigned long duration;
    TransitionType transitionType;
    unsigned long transitionDuration;
    float transitionSpeed;
    AudioSourceConfig audioConfig;
    
    SequenceStep();
    
    SequenceStep(colormodel* m,
                 const std::vector<FunctionWithPalette>& funcs,
                 unsigned long dur,
                 TransitionType trans = INSTANT,
                 float transSpeed = 1.0f,
                 unsigned long transDur = 0,
                 AudioSourceConfig audio = AudioSourceConfig());
    
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

// Sequence registry entry
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
    SequenceStep* steps;
    int numSteps;
    int currentStep;
    int previousStep;
    unsigned long stepStartTime;
    
    SequenceRegistryEntry registry[SEQ_MAX_REGISTRY];
    int numRegistryEntries;
    int currentRegistryIndex;
    unsigned long registryStartTime;
    
    bool inTransition;
    unsigned long transitionStartTime;
    float transitionProgress;
    
    bool audioActive;
    unsigned long audioLastActiveTime;
    bool audioFading;
    float audioFadeProgress;
    float audioFadeStartProgress;
    unsigned long audioFadeStartTime;
    bool fadingToAudio;
    static constexpr float AUDIO_FADE_IN_TIME = 0.2f;
    
    std::map<uint32_t, std::shared_ptr<StatefulColorFunction>> sequenceFunctionCache;
    
    std::shared_ptr<StatefulColorFunction> cachedBgFunctions[SEQ_MAX_FUNCTIONS];
    std::shared_ptr<StatefulColorFunction> cachedAudioFunctions[SEQ_MAX_FUNCTIONS];
    bool audioCacheValid;
    int cachedStepIndex;
    
    std::shared_ptr<StatefulColorFunction> cachedPrevFunctions[SEQ_MAX_FUNCTIONS];
    bool sameModelTransition;
    
    std::shared_ptr<StatefulColorFunction> getCachedFunction(const FunctionWithPalette& func);
    
    void applyFunctionsToModel();
    void configureAudioSource(const AudioSourceConfig& config);
    void updateAudioFade(const SequenceStep& step);
    void updateAudioFadeCache();
    
public:
    modelsequence();
    
    void addStep(const SequenceStep& step);
    void clearSteps();
    
    void beginRegistry(String name, float durationSeconds = 60.0, bool enabled = true);
    void endRegistry();
    int getNumRegistryEntries() const { return numRegistryEntries; }
    const char* getCurrentRegistryName() const;
    
    void update();
    void updateCachedFunctions();
    colormodel* getCurrentModel();
    CRGB getColor(int edgeindex, float position);
    
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
// Uses integer IDs for functions, palettes, models, permutations

struct FunctionDef {
    int functionId;
    int paletteId;
    std::vector<float> params;
    
    // Single ID (function only, e.g., "dark")
    FunctionDef(int funcId) : functionId(funcId), paletteId(0) {}
    
    // Function + palette
    FunctionDef(int funcId, int palId) : functionId(funcId), paletteId(palId) {}
    
    // Function + palette + params
    FunctionDef(int funcId, int palId, float p1) 
        : functionId(funcId), paletteId(palId), params{p1} {}
    FunctionDef(int funcId, int palId, float p1, float p2) 
        : functionId(funcId), paletteId(palId), params{p1, p2} {}
    FunctionDef(int funcId, int palId, float p1, float p2, float p3) 
        : functionId(funcId), paletteId(palId), params{p1, p2, p3} {}
    FunctionDef(int funcId, int palId, float p1, float p2, float p3, float p4) 
        : functionId(funcId), paletteId(palId), params{p1, p2, p3, p4} {}
    FunctionDef(int funcId, int palId, float p1, float p2, float p3, float p4, float p5) 
        : functionId(funcId), paletteId(palId), params{p1, p2, p3, p4, p5} {}
    FunctionDef(int funcId, int palId, float p1, float p2, float p3, float p4, float p5, float p6) 
        : functionId(funcId), paletteId(palId), params{p1, p2, p3, p4, p5, p6} {}
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
    
    std::map<uint32_t, colormodel*> permutedModelCache;  // Hash-based cache
    
    FunctionWithPalette buildFunctionWithPalette(const FunctionDef& def);
    
public:
    SequenceBuilder(modelsequence* s);
    
    void setaudiosource(AudioSourceConfig config);
    void setaudiopalette(std::initializer_list<FunctionDef> funcs);
    void setbackgroundpalette(std::initializer_list<FunctionDef> funcs);
    void setaudiotimeout(float seconds);
    
    void addpalette(std::initializer_list<FunctionDef> funcs);
    void clearpalettes();
    
    // Model by ID, duration in seconds
    void addstep(int modelId, float durationSeconds, 
                 TransitionType transition = INSTANT, float speed = 1.0,
                 bool acceptAudio = true);
    
    // Model by ID + permutation by ID
    void addstep(int modelId, int permId, float durationSeconds, 
                 TransitionType transition = INSTANT, float speed = 1.0,
                 bool acceptAudio = true);
    
    void add(int modelId, std::initializer_list<FunctionDef> funcDefs, float durationSeconds);
};

#endif // MODELSEQUENCE_H
