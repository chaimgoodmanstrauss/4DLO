////////////////////////////////////
//
//   modelsequence.h
//
// Sequence management system for HDLO controller
//
#ifndef MODELSEQUENCE_H
#define MODELSEQUENCE_H

#include "models.h"
#include "colorfunctions.h"
#include <array>
#include <vector>
#include <map>



// Audio switching thresholds (defined in sequences.cpp)
extern const float AUDIO_PALETTE_SWITCH_THRESHOLD;
extern const float AUDIO_TIMEOUT_SECONDS;

// Audio source configuration
struct AudioSourceConfig {
    enum SourceType { MICROPHONE, SD_CARD, LINE_IN };
    SourceType type;
    String filename;  // For SD_CARD type
    bool loop;        // For SD_CARD type
    
    AudioSourceConfig() : type(MICROPHONE), filename(""), loop(false) {}
    AudioSourceConfig(SourceType t, String fn = "", bool l = false) 
        : type(t), filename(fn), loop(l) {}
};

// Transition types
// INSTANT: immediate switch, no blending
// FADE: linear crossfade between all edges simultaneously
// WIPE: spatial sweep from edge 0 to 119, with 20% overlap window
enum TransitionType {
    INSTANT,
    FADE,
    WIPE
};

// Function with palette specification
struct FunctionWithPalette {
    String functionName;
    String paletteName;
    std::vector<FunctionParameter> parameters;
    
    FunctionWithPalette() : functionName("breathing"), paletteName("") {}
    
    FunctionWithPalette(String fn, String pn = "")
        : functionName(fn), paletteName(pn), parameters() {}
    
    FunctionWithPalette(String fn, String pn, std::initializer_list<float> params)
        : functionName(fn), paletteName(pn) {
        for(float p : params) {
            parameters.push_back(FunctionParameter(p));
        }
    }
};

// Sequence step structure
struct SequenceStep {
    static const int MAX_FUNCTIONS = 11;  // Matches current sequence design
    
    colormodel* model;
    
    // Dual palette system
    std::array<FunctionWithPalette, MAX_FUNCTIONS> audioPalettes;
    std::array<FunctionWithPalette, MAX_FUNCTIONS> backgroundPalettes;
    int numAudioFunctions;
    int numBackgroundFunctions;
    
    // Audio control
    bool acceptAudio;           // Whether this step responds to audio
    float audioThreshold;       // Audio level threshold to trigger audio palette
    float audioTimeout;         // Seconds to wait before fading back to background
    
    unsigned long duration;
    TransitionType transitionType;
    unsigned long transitionDuration;
    float transitionSpeed;  // For FADE/WIPE: 1.0 = 3.141 seconds
    AudioSourceConfig audioConfig;
    
    SequenceStep()
        : model(nullptr),
          numAudioFunctions(0),
          numBackgroundFunctions(0),
          acceptAudio(false),
          audioThreshold(AUDIO_PALETTE_SWITCH_THRESHOLD),
          audioTimeout(AUDIO_TIMEOUT_SECONDS),
          duration(5000),
          transitionType(INSTANT),
          transitionDuration(0),
          transitionSpeed(1.0f) {
        // Initialize with default palettes
        initializeDefaultPalettes();
    }
    
    // Legacy constructor - converts single palette to dual system
    SequenceStep(colormodel* m,
                 const std::vector<FunctionWithPalette>& funcs,
                 unsigned long dur,
                 TransitionType trans = INSTANT,
                 float transSpeed = 1.0f,
                 unsigned long transDur = 0,
                 AudioSourceConfig audio = AudioSourceConfig())
        : model(m),
          acceptAudio(false),
          audioThreshold(AUDIO_PALETTE_SWITCH_THRESHOLD),
          audioTimeout(AUDIO_TIMEOUT_SECONDS),
          duration(dur),
          transitionType(trans),
          transitionDuration(transDur),
          transitionSpeed(transSpeed),
          audioConfig(audio) {
        
        // Use provided functions as background palette
        numBackgroundFunctions = funcs.size();
        for(int i = 0; i < numBackgroundFunctions && i < MAX_FUNCTIONS; i++) {
            backgroundPalettes[i] = funcs[i];
        }
        if(numBackgroundFunctions > MAX_FUNCTIONS) {
            numBackgroundFunctions = MAX_FUNCTIONS;
        }
        
        // Initialize default audio palette
        initializeDefaultAudioPalette();
        
        // Convert transitionSpeed to transitionDuration if FADE or WIPE
        if(trans == FADE || trans == WIPE) {
            this->transitionDuration = (unsigned long)(transSpeed * 3141.0f);
        }
    }
    
    // New constructor for dual palettes
    SequenceStep(colormodel* m,
                 const std::vector<FunctionWithPalette>& audioFuncs,
                 const std::vector<FunctionWithPalette>& backgroundFuncs,
                 unsigned long dur,
                 bool acceptAud = true,
                 float audThresh = AUDIO_PALETTE_SWITCH_THRESHOLD,
                 float audTimeout = AUDIO_TIMEOUT_SECONDS,
                 TransitionType trans = INSTANT,
                 float transSpeed = 1.0f,
                 AudioSourceConfig audio = AudioSourceConfig())
        : model(m),
          acceptAudio(acceptAud),
          audioThreshold(audThresh),
          audioTimeout(audTimeout),
          duration(dur),
          transitionType(trans),
          transitionSpeed(transSpeed),
          audioConfig(audio) {
        
        // Set audio palette
        numAudioFunctions = audioFuncs.size();
        for(int i = 0; i < numAudioFunctions && i < MAX_FUNCTIONS; i++) {
            audioPalettes[i] = audioFuncs[i];
        }
        if(numAudioFunctions > MAX_FUNCTIONS) {
            numAudioFunctions = MAX_FUNCTIONS;
        }
        
        // Set background palette
        numBackgroundFunctions = backgroundFuncs.size();
        for(int i = 0; i < numBackgroundFunctions && i < MAX_FUNCTIONS; i++) {
            backgroundPalettes[i] = backgroundFuncs[i];
        }
        if(numBackgroundFunctions > MAX_FUNCTIONS) {
            numBackgroundFunctions = MAX_FUNCTIONS;
        }
        
        // Convert transitionSpeed to transitionDuration if FADE or WIPE
        if(trans == FADE || trans == WIPE) {
            this->transitionDuration = (unsigned long)(transSpeed * 3141.0f);
        }
    }
    
private:
    void initializeDefaultPalettes() {
        // Default audio palette
        audioPalettes[0] = FunctionWithPalette("dark", "");
        audioPalettes[1] = FunctionWithPalette("fftfire", "");
        audioPalettes[2] = FunctionWithPalette("fftfire", "");
        audioPalettes[3] = FunctionWithPalette("fftfire", "");
        audioPalettes[4] = FunctionWithPalette("fftfire", "");
        audioPalettes[5] = FunctionWithPalette("fftfire", "");
        audioPalettes[6] = FunctionWithPalette("fftfire", "");
        numAudioFunctions = 7;
        
        // Default background palette
        backgroundPalettes[0] = FunctionWithPalette("breathing", "");
        backgroundPalettes[1] = FunctionWithPalette("perlin", "heat");
        backgroundPalettes[2] = FunctionWithPalette("perlin", "cloud");
        backgroundPalettes[3] = FunctionWithPalette("perlin", "forest");
        backgroundPalettes[4] = FunctionWithPalette("perlin", "rainbow");
        backgroundPalettes[5] = FunctionWithPalette("perlin", "sunset");
        backgroundPalettes[6] = FunctionWithPalette("perlin", "ocean_builtin");
        numBackgroundFunctions = 7;
    }
    
    void initializeDefaultAudioPalette() {
        // Default audio palette
        audioPalettes[0] = FunctionWithPalette("dark", "");
        audioPalettes[1] = FunctionWithPalette("fftfire", "");
        audioPalettes[2] = FunctionWithPalette("fftfire", "");
        audioPalettes[3] = FunctionWithPalette("fftfire", "");
        audioPalettes[4] = FunctionWithPalette("fftfire", "");
        audioPalettes[5] = FunctionWithPalette("fftfire", "");
        audioPalettes[6] = FunctionWithPalette("fftfire", "");
        numAudioFunctions = 7;
    }
};

// Sequence registry entry
struct SequenceRegistryEntry {
    int startStepIndex;
    int numSteps;
    String name;
    unsigned long duration;
    bool enabled;
    
    SequenceRegistryEntry()
        : startStepIndex(0), numSteps(0), name(""), duration(60000), enabled(false) {}
    
    SequenceRegistryEntry(int start, int count, String n, unsigned long dur, bool en = true)
        : startStepIndex(start), numSteps(count), name(n), duration(dur), enabled(en) {}
};

// Main sequence class
class modelsequence {
private:
    static const int MAX_STEPS = 30;  // Reasonable limit for sequences
    static const int MAX_REGISTRY_ENTRIES = 20;
    
    SequenceStep steps[MAX_STEPS];
    int numSteps;
    int currentStep;
    int previousStep;  // For transition blending
    unsigned long stepStartTime;
    
    SequenceRegistryEntry registry[MAX_REGISTRY_ENTRIES];
    int numRegistryEntries;
    int currentRegistryIndex;
    unsigned long registryStartTime;
    
    bool inTransition;
    unsigned long transitionStartTime;
    float transitionProgress;
    
    // Audio palette fading state
    bool audioActive;               // Currently using audio palette
    unsigned long audioLastActiveTime;  // When audio was last above threshold
    bool audioFading;                // Currently fading between palettes
    float audioFadeProgress;         // 0.0 = background, 1.0 = audio
    float audioFadeStartProgress;    // Progress when current fade began
    unsigned long audioFadeStartTime;
    bool fadingToAudio;              // Direction of fade
    static constexpr float AUDIO_FADE_IN_TIME = 0.2f;  // Quick fade to audio (seconds)
    
    // Sequence-level function cache: reuse instances across sequence loops
    // Key format: "functionName:paletteName:param1,param2,..."
    std::map<String, std::shared_ptr<StatefulColorFunction>> sequenceFunctionCache;
    
    // Cached color functions for audio fade blending (now use shared_ptr from cache)
    std::shared_ptr<StatefulColorFunction> cachedBgFunctions[SequenceStep::MAX_FUNCTIONS];
    std::shared_ptr<StatefulColorFunction> cachedAudioFunctions[SequenceStep::MAX_FUNCTIONS];
    bool audioCacheValid;
    int cachedStepIndex;
    
    // Generate cache key from function definition
    String makeCacheKey(const FunctionWithPalette& func);
    
    // Get or create cached function instance
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
    // duration is in SECONDS and will be converted to milliseconds
    void beginRegistry(String name, float durationSeconds = 60.0, bool enabled = true);
    void endRegistry();
    int getNumRegistryEntries() const { return numRegistryEntries; }
    String getCurrentRegistryName() const;
    
    // Playback control
    void update();
    void updateCachedFunctions();  // Update cached audio functions once per frame
    colormodel* getCurrentModel();
    CRGB getColor(int edgeindex, float position);
    
    // Status
    int getCurrentStep() const;
    float getProgress() const;
    bool isInTransition() const { return inTransition; }
    float getTransitionProgress() const { return transitionProgress; }
    
    void reset();
    void printSequenceInfo();
    void printCacheStats();  // Print sequence function cache statistics
    void clearFunctionCache();  // Clear cached function instances (for memory management)
};

/////////////////////////////////////////
// SEQUENCE BUILDER HELPER
//
// Allows flexible syntax with NATIVE TYPES (no string conversion!):
//
// seq.add("test", {
//     "breathing",                      // Just function name
//     {"plasma", "ocean"},              // Function + palette  
//     {"plasma", "fire", 2.0},          // Function + palette + float (native!)
//     {"plasma", "lava", 1.5, 0.8}      // Function + palette + multiple floats
// }, 10);  // Duration in seconds
//
// Can handle 1-120 function definitions per step
// In practice: ~4 typical, 7 for this test (breathing + 6)
//
struct FunctionDef {
    String functionName;
    String paletteName;
    std::vector<float> params;
    
    // Constructor from single string (just function name)
    FunctionDef(const char* fn) : functionName(fn), paletteName("") {}
    FunctionDef(const String& fn) : functionName(fn), paletteName("") {}
    
    // Constructor: function + palette
    FunctionDef(const String& fn, const String& pn) 
        : functionName(fn), paletteName(pn) {}
    
    // Constructors with 1 float parameter
    FunctionDef(const String& fn, const String& pn, float p1) 
        : functionName(fn), paletteName(pn), params{p1} {}
    
    // Constructors with 2 float parameters
    FunctionDef(const String& fn, const String& pn, float p1, float p2) 
        : functionName(fn), paletteName(pn), params{p1, p2} {}
    
    // Constructors with 3 float parameters
    FunctionDef(const String& fn, const String& pn, float p1, float p2, float p3) 
        : functionName(fn), paletteName(pn), params{p1, p2, p3} {}
    
    // Constructors with 4 float parameters
    FunctionDef(const String& fn, const String& pn, float p1, float p2, float p3, float p4) 
        : functionName(fn), paletteName(pn), params{p1, p2, p3, p4} {}
    
    // Constructors with 5 float parameters
    FunctionDef(const String& fn, const String& pn, float p1, float p2, float p3, float p4, float p5) 
        : functionName(fn), paletteName(pn), params{p1, p2, p3, p4, p5} {}
    
    // Constructors with 6 float parameters
    FunctionDef(const String& fn, const String& pn, float p1, float p2, float p3, float p4, float p5, float p6) 
        : functionName(fn), paletteName(pn), params{p1, p2, p3, p4, p5, p6} {}
    
    // Constructor from initializer_list (for backward compatibility with string params)
    FunctionDef(std::initializer_list<String> def) {
        auto it = def.begin();
        functionName = (it != def.end()) ? *it++ : "breathing";
        paletteName = (it != def.end()) ? *it++ : "";
        while(it != def.end()) {
            params.push_back((*it++).toFloat());
        }
    }
};

struct SequenceBuilder {
    modelsequence* seq;
    std::vector<FunctionDef> currentPalettes;
    std::vector<FunctionDef> currentAudioPalettes;
    std::vector<FunctionDef> currentBackgroundPalettes;
    bool useDualPalettes;
    float audioTimeoutSeconds;  // Configurable timeout for audio palette fade
    AudioSourceConfig currentAudioSource;  // Audio source for subsequent steps
    
    // Cache for permuted models: key = "modelName_permName", value = created model pointer
    // Models registered in global registry, so safe to keep pointers
    std::map<String, colormodel*> permutedModelCache;
    
    SequenceBuilder(modelsequence* s) 
        : seq(s), 
          useDualPalettes(false), 
          audioTimeoutSeconds(AUDIO_TIMEOUT_SECONDS),
          currentAudioSource() {}
    
    // Set palette definitions to be used by subsequent addstep() calls (legacy)
    void addpalette(std::initializer_list<FunctionDef> funcDefs) {
        currentPalettes.clear();
        for(const auto& def : funcDefs) {
            currentPalettes.push_back(def);
        }
        useDualPalettes = false;
    }
    
    // Set audio palette for dual palette mode
    void setaudiopalette(std::initializer_list<FunctionDef> funcDefs) {
        currentAudioPalettes.clear();
        for(const auto& def : funcDefs) {
            currentAudioPalettes.push_back(def);
        }
        useDualPalettes = true;
    }
    
    // Set background palette for dual palette mode
    void setbackgroundpalette(std::initializer_list<FunctionDef> funcDefs) {
        currentBackgroundPalettes.clear();
        for(const auto& def : funcDefs) {
            currentBackgroundPalettes.push_back(def);
        }
        useDualPalettes = true;
    }
    
    // Set audio timeout (seconds) - how long to wait before fading back to background
    void setaudiotimeout(float seconds) {
        audioTimeoutSeconds = seconds;
    }
    
    // Set audio source for subsequent addstep() calls
    void setaudiosource(AudioSourceConfig::SourceType type, String filename = "", bool loop = false) {
        currentAudioSource = AudioSourceConfig(type, filename, loop);
    }
    
    // Add step using previously defined palettes
    // duration is in SECONDS, will be converted to milliseconds
    // speed: for FADE/WIPE transitions, 1.0 = 3.141 seconds
    void addstep(String modelName, float durationSeconds, 
                 TransitionType transition = INSTANT, float speed = 1.0,
                 bool acceptAudio = true) {
        
        if(!useDualPalettes && currentPalettes.empty()) {
            Serial.println("Error: No palettes defined. Call addpalette() or setaudiopalette/setbackgroundpalette first.");
            return;
        }
        
        colormodel* model = colormodel::findModelByName(modelName);
        if(!model) {
            Serial.println("Error: Model '" + modelName + "' not found");
            return;
        }
        
        if(useDualPalettes) {
            // Dual palette mode
            std::vector<FunctionWithPalette> audioFunctions;
            std::vector<FunctionWithPalette> backgroundFunctions;
            
            // Build audio functions
            for(const auto& def : currentAudioPalettes) {
                if(audioFunctions.size() >= SequenceStep::MAX_FUNCTIONS) break;
                audioFunctions.push_back(buildFunctionWithPalette(def));
            }
            
            // Build background functions
            for(const auto& def : currentBackgroundPalettes) {
                if(backgroundFunctions.size() >= SequenceStep::MAX_FUNCTIONS) break;
                backgroundFunctions.push_back(buildFunctionWithPalette(def));
            }
            
            unsigned long durationMs = (unsigned long)(durationSeconds * 1000.0);
            seq->addStep(SequenceStep(model, audioFunctions, backgroundFunctions, durationMs, 
                                     acceptAudio, AUDIO_PALETTE_SWITCH_THRESHOLD, audioTimeoutSeconds, 
                                     transition, speed, currentAudioSource));
        } else {
            // Legacy single palette mode
            std::vector<FunctionWithPalette> functions;
            
            for(const auto& def : currentPalettes) {
                if(functions.size() >= SequenceStep::MAX_FUNCTIONS) break;
                functions.push_back(buildFunctionWithPalette(def));
            }
            
            unsigned long durationMs = (unsigned long)(durationSeconds * 1000.0);
            seq->addStep(SequenceStep(model, functions, durationMs, transition, speed, 0, currentAudioSource));
        }
    }
    
private:
    FunctionWithPalette buildFunctionWithPalette(const FunctionDef& def) {
        if(def.params.empty()) {
            return FunctionWithPalette(def.functionName, def.paletteName);
        } else {
            switch(def.params.size()) {
                case 1:
                    return FunctionWithPalette(def.functionName, def.paletteName, 
                        {def.params[0]});
                case 2:
                    return FunctionWithPalette(def.functionName, def.paletteName, 
                        {def.params[0], def.params[1]});
                case 3:
                    return FunctionWithPalette(def.functionName, def.paletteName, 
                        {def.params[0], def.params[1], def.params[2]});
                case 4:
                    return FunctionWithPalette(def.functionName, def.paletteName, 
                        {def.params[0], def.params[1], def.params[2], def.params[3]});
                case 5:
                    return FunctionWithPalette(def.functionName, def.paletteName, 
                        {def.params[0], def.params[1], def.params[2], def.params[3], def.params[4]});
                case 6:
                default:
                    return FunctionWithPalette(def.functionName, def.paletteName, 
                        {def.params[0], def.params[1], def.params[2], def.params[3], def.params[4], def.params[5]});
            }
        }
    }
    
public:
    // Add step using named model + named permutation with caching
    // Automatically creates and caches permuted models: "baseModel_permName"
    // duration is in SECONDS, will be converted to milliseconds
    // speed: for FADE/WIPE transitions, 1.0 = 3.141 seconds
    void addstep(String modelName, String permName, float durationSeconds, 
                 TransitionType transition = INSTANT, float speed = 1.0,
                 bool acceptAudio = true) {
        
        if(!useDualPalettes && currentPalettes.empty()) {
            Serial.println("Error: No palettes defined. Call addpalette() or setaudiopalette/setbackgroundpalette first.");
            return;
        }
        
        // Generate cache key
        String cacheKey = modelName + "_" + permName;
        colormodel* permutedModel = nullptr;
        
        // Check cache first
        auto it = permutedModelCache.find(cacheKey);
        if(it != permutedModelCache.end()) {
            permutedModel = it->second;
        } else {
            // Not in cache - create and register permuted model
            permutedModel = colormodel::applyEdgePermutation(modelName, permName, cacheKey);
            if(!permutedModel) {
                Serial.println("Error: Failed to create permuted model '" + cacheKey + "'");
                Serial.println("  Base model: '" + modelName + "', Permutation: '" + permName + "'");
                return;
            }
            // Add to cache
            permutedModelCache[cacheKey] = permutedModel;
            Serial.println("Created and cached permuted model: " + cacheKey);
        }
        
        if(useDualPalettes) {
            // Dual palette mode
            std::vector<FunctionWithPalette> audioFunctions;
            std::vector<FunctionWithPalette> backgroundFunctions;
            
            // Build audio functions
            for(const auto& def : currentAudioPalettes) {
                if(audioFunctions.size() >= SequenceStep::MAX_FUNCTIONS) break;
                audioFunctions.push_back(buildFunctionWithPalette(def));
            }
            
            // Build background functions
            for(const auto& def : currentBackgroundPalettes) {
                if(backgroundFunctions.size() >= SequenceStep::MAX_FUNCTIONS) break;
                backgroundFunctions.push_back(buildFunctionWithPalette(def));
            }
            
            unsigned long durationMs = (unsigned long)(durationSeconds * 1000.0);
            seq->addStep(SequenceStep(permutedModel, audioFunctions, backgroundFunctions, durationMs, 
                                     acceptAudio, AUDIO_PALETTE_SWITCH_THRESHOLD, audioTimeoutSeconds, 
                                     transition, speed, currentAudioSource));
        } else {
            // Legacy single palette mode
            std::vector<FunctionWithPalette> functions;
            
            for(const auto& def : currentPalettes) {
                if(functions.size() >= SequenceStep::MAX_FUNCTIONS) break;
                functions.push_back(buildFunctionWithPalette(def));
            }
            
            unsigned long durationMs = (unsigned long)(durationSeconds * 1000.0);
            seq->addStep(SequenceStep(permutedModel, functions, durationMs, transition, speed, 0, currentAudioSource));
        }
    }
    
    // Add step with flexible nested syntax (original method, still supported)
    // duration is in SECONDS and will be converted to milliseconds
    // Accepts any number of function definitions (up to 120)
    void add(String modelName,
             std::initializer_list<FunctionDef> funcDefs,
             float durationSeconds) {
        
        colormodel* model = colormodel::findModelByName(modelName);
        if(!model) {
            Serial.println("Error: Model '" + modelName + "' not found");
            return;
        }
        
        std::vector<FunctionWithPalette> functions;
        
        for(const auto& def : funcDefs) {
            if(functions.size() >= SequenceStep::MAX_FUNCTIONS) break;
            
            if(def.params.empty()) {
                functions.push_back(FunctionWithPalette(def.functionName, def.paletteName));
            } else {
                // Build initializer_list by explicitly listing params
                switch(def.params.size()) {
                    case 1:
                        functions.push_back(FunctionWithPalette(def.functionName, def.paletteName, 
                            {def.params[0]}));
                        break;
                    case 2:
                        functions.push_back(FunctionWithPalette(def.functionName, def.paletteName, 
                            {def.params[0], def.params[1]}));
                        break;
                    case 3:
                        functions.push_back(FunctionWithPalette(def.functionName, def.paletteName, 
                            {def.params[0], def.params[1], def.params[2]}));
                        break;
                    case 4:
                        functions.push_back(FunctionWithPalette(def.functionName, def.paletteName, 
                            {def.params[0], def.params[1], def.params[2], def.params[3]}));
                        break;
                    default:
                        // More than 4 params - just use first 4
                        functions.push_back(FunctionWithPalette(def.functionName, def.paletteName, 
                            {def.params[0], def.params[1], def.params[2], def.params[3]}));
                        break;
                }
            }
        }
        
        // Fill remaining slots with breathing if we have less than expected
        // (This is optional - depends on desired behavior)
        // For now, just use what was provided
        
        // Convert seconds to milliseconds
        unsigned long durationMs = (unsigned long)(durationSeconds * 1000.0f);
        seq->addStep(SequenceStep(model, functions, durationMs, INSTANT, 1.0f, 0, currentAudioSource));
    }
};

#endif // MODELSEQUENCE_H
