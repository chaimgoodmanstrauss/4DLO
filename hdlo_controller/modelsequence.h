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
    static const int MAX_FUNCTIONS = 7;  // Matches current sequence design
    
    colormodel* model;
    std::array<FunctionWithPalette, MAX_FUNCTIONS> functions;
    int numFunctions;  // Track actual number used
    unsigned long duration;
    TransitionType transitionType;
    unsigned long transitionDuration;
    float transitionSpeed;  // For FADE/WIPE: 1.0 = 3.141 seconds
    AudioSourceConfig audioConfig;
    
    SequenceStep()
        : model(nullptr),
          numFunctions(0),
          duration(5000),
          transitionType(INSTANT),
          transitionDuration(0),
          transitionSpeed(1.0f) {}
    
    SequenceStep(colormodel* m,
                 const std::vector<FunctionWithPalette>& funcs,
                 unsigned long dur,
                 TransitionType trans = INSTANT,
                 float transSpeed = 1.0f,
                 unsigned long transDur = 0,
                 AudioSourceConfig audio = AudioSourceConfig())
        : model(m),
          numFunctions(funcs.size()),
          duration(dur),
          transitionType(trans),
          transitionDuration(transDur),
          transitionSpeed(transSpeed),
          audioConfig(audio) {
        // Copy functions up to MAX_FUNCTIONS
        for(int i = 0; i < numFunctions && i < MAX_FUNCTIONS; i++) {
            functions[i] = funcs[i];
        }
        if(numFunctions > MAX_FUNCTIONS) {
            numFunctions = MAX_FUNCTIONS;
        }
        
        // Convert transitionSpeed to transitionDuration if FADE or WIPE
        // 1.0 speed = 3.141 seconds = 3141 ms
        if(trans == FADE || trans == WIPE) {
            this->transitionDuration = (unsigned long)(transSpeed * 3141.0f);
        }
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
    unsigned long stepStartTime;
    
    SequenceRegistryEntry registry[MAX_REGISTRY_ENTRIES];
    int numRegistryEntries;
    int currentRegistryIndex;
    unsigned long registryStartTime;
    
    bool inTransition;
    unsigned long transitionStartTime;
    float transitionProgress;
    
    void applyFunctionsToModel();
    void configureAudioSource(const AudioSourceConfig& config);
    
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
    colormodel* getCurrentModel();
    CRGB getColor(int edgeindex, float position);
    
    // Status
    int getCurrentStep() const;
    float getProgress() const;
    bool isInTransition() const { return inTransition; }
    float getTransitionProgress() const { return transitionProgress; }
    
    void reset();
    void printSequenceInfo();
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
    
    // Cache for permuted models: key = "modelName_permName", value = created model pointer
    // Models registered in global registry, so safe to keep pointers
    std::map<String, colormodel*> permutedModelCache;
    
    SequenceBuilder(modelsequence* s) : seq(s) {}
    
    // Set palette definitions to be used by subsequent addstep() calls
    void addpalette(std::initializer_list<FunctionDef> funcDefs) {
        currentPalettes.clear();
        for(const auto& def : funcDefs) {
            currentPalettes.push_back(def);
        }
    }
    
    // Add step using previously defined palettes
    // duration is in SECONDS, will be converted to milliseconds
    // speed: for FADE/WIPE transitions, 1.0 = 3.141 seconds
    void addstep(String modelName, float durationSeconds, TransitionType transition = INSTANT, float speed = 1.0f) {
        if(currentPalettes.empty()) {
            Serial.println("Error: No palettes defined. Call addpalette() first.");
            return;
        }
        
        colormodel* model = colormodel::findModelByName(modelName);
        if(!model) {
            Serial.println("Error: Model '" + modelName + "' not found");
            return;
        }
        
        std::vector<FunctionWithPalette> functions;
        
        for(const auto& def : currentPalettes) {
            if(functions.size() >= SequenceStep::MAX_FUNCTIONS) break;
            
            if(def.params.empty()) {
                functions.push_back(FunctionWithPalette(def.functionName, def.paletteName));
            } else {
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
                        functions.push_back(FunctionWithPalette(def.functionName, def.paletteName, 
                            {def.params[0], def.params[1], def.params[2], def.params[3]}));
                        break;
                }
            }
        }
        
        unsigned long durationMs = (unsigned long)(durationSeconds * 1000.0f);
        seq->addStep(SequenceStep(model, functions, durationMs, transition, speed));
    }
    
    // Add step using named model + named permutation with caching
    // Automatically creates and caches permuted models: "baseModel_permName"
    // duration is in SECONDS, will be converted to milliseconds
    // speed: for FADE/WIPE transitions, 1.0 = 3.141 seconds
    void addstep(String modelName, String permName, float durationSeconds, TransitionType transition = INSTANT, float speed = 1.0f) {
        if(currentPalettes.empty()) {
            Serial.println("Error: No palettes defined. Call addpalette() first.");
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
        
        // Build functions from current palettes
        std::vector<FunctionWithPalette> functions;
        
        for(const auto& def : currentPalettes) {
            if(functions.size() >= SequenceStep::MAX_FUNCTIONS) break;
            
            if(def.params.empty()) {
                functions.push_back(FunctionWithPalette(def.functionName, def.paletteName));
            } else {
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
                        functions.push_back(FunctionWithPalette(def.functionName, def.paletteName, 
                            {def.params[0], def.params[1], def.params[2], def.params[3]}));
                        break;
                }
            }
        }
        
        unsigned long durationMs = (unsigned long)(durationSeconds * 1000.0f);
        seq->addStep(SequenceStep(permutedModel, functions, durationMs, transition, speed));
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
        seq->addStep(SequenceStep(model, functions, durationMs));
    }
};

#endif // MODELSEQUENCE_H
