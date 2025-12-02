////////////////////////////////////
//
//   modelsequence.cpp
//
// Sequence implementation - simplified fixed-size version
//

#include "modelsequence.h"
#include "audiosystem.h"
#include <FastLED.h>

extern unsigned long _heap_start;
extern unsigned long _heap_end;
extern char *__brkval;

int getFreeRam() {
    if (__brkval == 0) {
        return ((int)&_heap_end - (int)&_heap_start);
    }
    return ((int)&_heap_end - (int)__brkval);
}

////////////////////////////////////
// SequenceStep implementation

SequenceStep::SequenceStep()
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
    initializeDefaultPalettes();
}

SequenceStep::SequenceStep(colormodel* m,
                           const std::vector<FunctionWithPalette>& funcs,
                           unsigned long dur,
                           TransitionType trans,
                           float transSpeed,
                           unsigned long transDur,
                           AudioSourceConfig audio)
    : model(m),
      acceptAudio(false),
      audioThreshold(AUDIO_PALETTE_SWITCH_THRESHOLD),
      audioTimeout(AUDIO_TIMEOUT_SECONDS),
      duration(dur),
      transitionType(trans),
      transitionDuration(transDur),
      transitionSpeed(transSpeed),
      audioConfig(audio) {
    
    numBackgroundFunctions = min((int)funcs.size(), SEQ_MAX_FUNCTIONS);
    for(int i = 0; i < numBackgroundFunctions; i++) {
        backgroundPalettes[i] = funcs[i];
    }
    
    initializeDefaultAudioPalette();
    
    if(trans == FADE || trans == WIPE) {
        this->transitionDuration = (unsigned long)(transSpeed * 3141.0f);
    }
}

SequenceStep::SequenceStep(colormodel* m,
                           const std::vector<FunctionWithPalette>& audioFuncs,
                           const std::vector<FunctionWithPalette>& backgroundFuncs,
                           unsigned long dur,
                           bool acceptAud,
                           float audThresh,
                           float audTimeout,
                           TransitionType trans,
                           float transSpeed,
                           AudioSourceConfig audio)
    : model(m),
      acceptAudio(acceptAud),
      audioThreshold(audThresh),
      audioTimeout(audTimeout),
      duration(dur),
      transitionType(trans),
      transitionSpeed(transSpeed),
      audioConfig(audio) {
    
    numAudioFunctions = min((int)audioFuncs.size(), SEQ_MAX_FUNCTIONS);
    for(int i = 0; i < numAudioFunctions; i++) {
        audioPalettes[i] = audioFuncs[i];
    }
    
    numBackgroundFunctions = min((int)backgroundFuncs.size(), SEQ_MAX_FUNCTIONS);
    for(int i = 0; i < numBackgroundFunctions; i++) {
        backgroundPalettes[i] = backgroundFuncs[i];
    }
    
    if(trans == FADE || trans == WIPE) {
        this->transitionDuration = (unsigned long)(transSpeed * 3141.0f);
    }
}

void SequenceStep::initializeDefaultPalettes() {
    audioPalettes[0] = FunctionWithPalette("dark", "");
    for(int i = 1; i < 7; i++) {
        audioPalettes[i] = FunctionWithPalette("fftfire", "");
    }
    numAudioFunctions = 7;
    
    backgroundPalettes[0] = FunctionWithPalette("breathing", "");
    backgroundPalettes[1] = FunctionWithPalette("perlin", "heat");
    backgroundPalettes[2] = FunctionWithPalette("perlin", "cloud");
    backgroundPalettes[3] = FunctionWithPalette("perlin", "forest");
    backgroundPalettes[4] = FunctionWithPalette("perlin", "rainbow");
    backgroundPalettes[5] = FunctionWithPalette("perlin", "sunset");
    backgroundPalettes[6] = FunctionWithPalette("perlin", "ocean_builtin");
    numBackgroundFunctions = 7;
}

void SequenceStep::initializeDefaultAudioPalette() {
    audioPalettes[0] = FunctionWithPalette("dark", "");
    for(int i = 1; i < 7; i++) {
        audioPalettes[i] = FunctionWithPalette("fftfire", "");
    }
    numAudioFunctions = 7;
}

////////////////////////////////////
// modelsequence implementation

// Steps array in EXTMEM (8MB PSRAM)
EXTMEM SequenceStep extmemSteps[SEQ_MAX_STEPS];

modelsequence::modelsequence()
    : steps(extmemSteps),
      numSteps(0),
      currentStep(0),
      previousStep(-1),
      stepStartTime(0),
      numRegistryEntries(0),
      currentRegistryIndex(-1),
      registryStartTime(0),
      inTransition(false),
      transitionStartTime(0),
      transitionProgress(0.0),
      audioActive(false),
      audioLastActiveTime(0),
      audioFading(false),
      audioFadeProgress(0.0),
      audioFadeStartProgress(0.0),
      audioFadeStartTime(0),
      fadingToAudio(false),
      audioCacheValid(false),
      cachedStepIndex(-1) {
    Serial.print("Steps in EXTMEM at 0x");
    Serial.println((uint32_t)steps, HEX);
}

void modelsequence::addStep(const SequenceStep& step) {
    if(numSteps < SEQ_MAX_STEPS) {
        steps[numSteps] = step;
        Serial.print("Added step ");
        Serial.print(numSteps);
        Serial.print(": model=");
        Serial.print(step.model ? step.model->getModelName() : "NULL");
        Serial.print(", transition=");
        Serial.print(step.transitionType == INSTANT ? "INSTANT" : 
                    (step.transitionType == FADE ? "FADE" : "WIPE"));
        Serial.print(", duration=");
        Serial.print(step.transitionDuration);
        Serial.println("ms");
        numSteps++;
    } else {
        Serial.println("ERROR: MAX_STEPS exceeded!");
    }
}

void modelsequence::clearSteps() {
    numSteps = 0;
    currentStep = 0;
    stepStartTime = millis();
}

void modelsequence::beginRegistry(String name, float durationSeconds, bool enabled) {
    if(numRegistryEntries < SEQ_MAX_REGISTRY) {
        unsigned long durationMs = (unsigned long)(durationSeconds * 1000.0f);
        registry[numRegistryEntries] = SequenceRegistryEntry(
            numSteps, 0, name, durationMs, enabled
        );
        numRegistryEntries++;
    }
}

void modelsequence::endRegistry() {
    if(numRegistryEntries > 0) {
        int lastIndex = numRegistryEntries - 1;
        registry[lastIndex].numSteps = numSteps - registry[lastIndex].startStepIndex;
    }
}

// Hash function for cache key - no allocation
static uint32_t hashFunctionKey(const FunctionWithPalette& func) {
    uint32_t hash = 5381;
    
    const char* p = func.functionName;
    while(*p) hash = ((hash << 5) + hash) + *p++;
    
    hash = ((hash << 5) + hash) + ':';
    
    p = func.paletteName;
    while(*p) hash = ((hash << 5) + hash) + *p++;
    
    for(int i = 0; i < func.numParams; i++) {
        uint32_t* fp = (uint32_t*)&func.params[i];
        hash = ((hash << 5) + hash) + *fp;
    }
    
    return hash;
}

// Pre-allocated buffer for parameter passing
static std::vector<FunctionParameter> staticParamBuffer;
static bool staticParamBufferInit = false;

std::shared_ptr<StatefulColorFunction> modelsequence::getCachedFunction(const FunctionWithPalette& func) {
    if(!staticParamBufferInit) {
        staticParamBuffer.reserve(SEQ_MAX_PARAMS);
        staticParamBufferInit = true;
    }
    
    uint32_t hash = hashFunctionKey(func);
    
    auto it = sequenceFunctionCache.find(hash);
    if(it != sequenceFunctionCache.end()) {
        return it->second;
    }
    
    StatefulColorFunction* rawPtr = ColorFunctionFactory::getInstance().create(func.functionName);
    if(!rawPtr) return nullptr;
    
    if(func.paletteName[0] != '\0') {
        rawPtr->setPalette(func.paletteName);
    }
    if(func.numParams > 0) {
        staticParamBuffer.clear();
        for(int i = 0; i < func.numParams; i++) {
            staticParamBuffer.push_back(FunctionParameter(func.params[i]));
        }
        rawPtr->setParameters(staticParamBuffer);
    }
    
    std::shared_ptr<StatefulColorFunction> sharedPtr(rawPtr);
    sequenceFunctionCache[hash] = sharedPtr;
    
    Serial.print("Cache: ");
    Serial.println(func.functionName);
    
    return sharedPtr;
}

void modelsequence::updateAudioFade(const SequenceStep& step) {
    if(!step.acceptAudio) {
        audioActive = false;
        audioFading = false;
        audioFadeProgress = 0.0;
        return;
    }
    
    if(!AudioSystem::isInitialized()) return;
    
    unsigned long currentTime = millis();
    float audioLevel = AudioSystem::getMaxBand();
    
    static unsigned long lastMemoryReport = 0;
    if(currentTime - lastMemoryReport > 60000) {
        int freeRam = getFreeRam();
        Serial.print("FREE RAM: ");
        Serial.print(freeRam);
        Serial.print(" bytes, uptime: ");
        Serial.print(currentTime / 1000);
        Serial.print("s, steps: ");
        Serial.println(numSteps);
        lastMemoryReport = currentTime;
    }
    
    if(audioLevel > step.audioThreshold) {
        audioLastActiveTime = currentTime;
        
        if(!audioActive) {
            audioActive = true;
            audioFading = true;
            fadingToAudio = true;
            audioFadeStartProgress = audioFadeProgress;
            audioFadeStartTime = currentTime;
            Serial.println(">>> AUDIO ON");
        } else if(audioFading && !fadingToAudio) {
            fadingToAudio = true;
            audioFadeStartProgress = audioFadeProgress;
            audioFadeStartTime = currentTime;
        }
    }
    
    if(audioActive && (currentTime - audioLastActiveTime) > (unsigned long)(step.audioTimeout * 1000.0)) {
        if(!audioFading || fadingToAudio) {
            audioFading = true;
            fadingToAudio = false;
            audioFadeStartProgress = audioFadeProgress;
            audioFadeStartTime = currentTime;
        }
    }
    
    if(audioFading) {
        float fadeDuration = fadingToAudio ? AUDIO_FADE_IN_TIME : step.audioTimeout;
        unsigned long fadeElapsed = currentTime - audioFadeStartTime;
        float rawProgress = min(1.0f, fadeElapsed / (fadeDuration * 1000.0f));
        
        if(fadingToAudio) {
            audioFadeProgress = audioFadeStartProgress + (1.0f - audioFadeStartProgress) * rawProgress;
            if(rawProgress >= 1.0f) {
                audioFading = false;
                audioFadeProgress = 1.0f;
            }
        } else {
            audioFadeProgress = audioFadeStartProgress * (1.0f - rawProgress);
            if(rawProgress >= 1.0f) {
                audioFading = false;
                audioActive = false;
                audioFadeProgress = 0.0f;
            }
        }
    }
}

void modelsequence::updateAudioFadeCache() {
    const auto& step = steps[currentStep];
    
    if(audioCacheValid && cachedStepIndex == currentStep) return;
    
    audioCacheValid = false;
    for(int i = 0; i < SEQ_MAX_FUNCTIONS; i++) {
        cachedBgFunctions[i] = nullptr;
        cachedAudioFunctions[i] = nullptr;
    }
    
    for(int i = 0; i < step.numBackgroundFunctions; i++) {
        cachedBgFunctions[i] = getCachedFunction(step.backgroundPalettes[i]);
    }
    
    for(int i = 0; i < step.numAudioFunctions; i++) {
        cachedAudioFunctions[i] = getCachedFunction(step.audioPalettes[i]);
    }
    
    audioCacheValid = true;
    cachedStepIndex = currentStep;
}

void modelsequence::applyFunctionsToModel() {
    if(currentStep < 0 || currentStep >= numSteps || !steps[currentStep].model) {
        Serial.println("applyFunctionsToModel: Invalid step or null model");
        return;
    }
    
    if(!staticParamBufferInit) {
        staticParamBuffer.reserve(SEQ_MAX_PARAMS);
        staticParamBufferInit = true;
    }
    
    colormodel* model = steps[currentStep].model;
    const auto& step = steps[currentStep];
    
    updateAudioFade(step);
    
    bool useAudioPalette = (audioFadeProgress > 0.5f);
    int numFunctions = useAudioPalette ? step.numAudioFunctions : step.numBackgroundFunctions;
    
    Serial.print("Applying ");
    Serial.print(numFunctions);
    Serial.print(" functions, audio=");
    Serial.println(audioFadeProgress, 2);
    
    int appliedCount = 0;
    
    for(int edge = 0; edge < 120; edge++) {
        int funcIndex = model->getEdgeFunctionIndex(edge);
        
        if(funcIndex >= 0) {
            int safeIndex = funcIndex % numFunctions;
            const FunctionWithPalette& func = useAudioPalette ? 
                step.audioPalettes[safeIndex] : step.backgroundPalettes[safeIndex];
            
            std::shared_ptr<StatefulColorFunction> cachedFunc = getCachedFunction(func);
            if(cachedFunc) {
                staticParamBuffer.clear();
                for(int i = 0; i < func.numParams; i++) {
                    staticParamBuffer.push_back(FunctionParameter(func.params[i]));
                }
                model->setColorFunction(edge, func.functionName, func.paletteName, staticParamBuffer);
                appliedCount++;
            }
        } else {
            staticParamBuffer.clear();
            model->setColorFunction(edge, "dark", "", staticParamBuffer);
        }
    }
    
    Serial.print("Applied: ");
    Serial.println(appliedCount);
}

void modelsequence::update() {
    unsigned long currentTime = millis();
    
    if(currentStep >= 0 && currentStep < numSteps) {
        updateAudioFade(steps[currentStep]);
    }
    
    // Check registry switch
    if(currentRegistryIndex >= 0 && currentRegistryIndex < numRegistryEntries) {
        unsigned long registryElapsed = currentTime - registryStartTime;
        if(registryElapsed >= registry[currentRegistryIndex].duration) {
            int nextIndex = (currentRegistryIndex + 1) % numRegistryEntries;
            while(!registry[nextIndex].enabled && nextIndex != currentRegistryIndex) {
                nextIndex = (nextIndex + 1) % numRegistryEntries;
            }
            
            if(nextIndex != currentRegistryIndex) {
                previousStep = currentStep;
                currentRegistryIndex = nextIndex;
                currentStep = registry[currentRegistryIndex].startStepIndex;
                registryStartTime = currentTime;
                stepStartTime = currentTime;
                inTransition = false;
                
                audioActive = false;
                audioFading = false;
                audioFadeProgress = 0.0;
                audioLastActiveTime = 0;
                
                applyFunctionsToModel();
                configureAudioSource(steps[currentStep].audioConfig);
                
                Serial.print("Registry: ");
                Serial.println(registry[currentRegistryIndex].name);
            }
        }
    }
    
    // Handle transitions
    if(inTransition) {
        unsigned long elapsed = currentTime - transitionStartTime;
        if(elapsed >= steps[currentStep].transitionDuration) {
            inTransition = false;
            transitionProgress = 1.0;
        } else {
            transitionProgress = (float)elapsed / steps[currentStep].transitionDuration;
        }
    }
    
    // Check step advance
    if(currentStep >= 0 && currentStep < numSteps) {
        unsigned long elapsed = currentTime - stepStartTime;
        if(elapsed >= steps[currentStep].duration) {
            previousStep = currentStep;
            
            if(currentRegistryIndex >= 0) {
                int registryEnd = registry[currentRegistryIndex].startStepIndex + 
                                 registry[currentRegistryIndex].numSteps;
                int nextStep = currentStep + 1;
                if(nextStep >= registryEnd) {
                    nextStep = registry[currentRegistryIndex].startStepIndex;
                }
                currentStep = nextStep;
            } else {
                currentStep = (currentStep + 1) % numSteps;
            }
            
            stepStartTime = currentTime;
            
            audioActive = false;
            audioFading = false;
            audioFadeProgress = 0.0;
            audioLastActiveTime = 0;
            
            if(steps[currentStep].transitionType != INSTANT && 
               steps[currentStep].transitionDuration > 0) {
                inTransition = true;
                transitionStartTime = currentTime;
                transitionProgress = 0.0;
            } else {
                inTransition = false;
            }
            
            applyFunctionsToModel();
            configureAudioSource(steps[currentStep].audioConfig);
        }
    }
}

void modelsequence::updateCachedFunctions() {
    if(currentStep < 0 || currentStep >= numSteps) return;
    const auto& step = steps[currentStep];
    
    if(step.acceptAudio && audioFadeProgress > 0.001f) {
        if(currentStep != cachedStepIndex || !audioCacheValid) {
            updateAudioFadeCache();
        }
        
        unsigned long currentTime = millis();
        for(int i = 0; i < SEQ_MAX_FUNCTIONS; i++) {
            if(cachedBgFunctions[i]) {
                cachedBgFunctions[i]->updateIfNeeded(currentTime);
            }
            if(cachedAudioFunctions[i]) {
                cachedAudioFunctions[i]->updateIfNeeded(currentTime);
            }
        }
    }
}

colormodel* modelsequence::getCurrentModel() {
    if(currentStep >= 0 && currentStep < numSteps) {
        return steps[currentStep].model;
    }
    return nullptr;
}

CRGB modelsequence::getColor(int edgeindex, float position) {
    colormodel* model = getCurrentModel();
    if(!model) return CRGB::Black;
    
    const auto& step = steps[currentStep];
    CRGB currentColor;
    
    if(step.acceptAudio && audioFadeProgress > 0.001f) {
        if(currentStep != cachedStepIndex || !audioCacheValid) {
            updateAudioFadeCache();
        }
        
        if(edgeindex < 0 || edgeindex >= 120) return CRGB::Black;
        
        int funcIndex = model->getEdgeFunctionIndex(edgeindex);
        
        const auto& edgeModels = model->getEdgeModels();
        int direction = edgeModels[edgeindex][1];
        float workingPosition = position;
        if(direction < 0) workingPosition = 1.0 - workingPosition;
        if(direction == 0) workingPosition = 2.0 * abs(0.5 - workingPosition);
        float startPos = edgeModels[edgeindex][2] / 10000.0;
        float scale = edgeModels[edgeindex][3] / 10000.0;
        float transformedPosition = startPos + scale * workingPosition;
        
        if(audioFadeProgress < 0.999f) {
            CRGB bgColor = CRGB::Black;
            if(funcIndex >= 0 && funcIndex < step.numBackgroundFunctions && cachedBgFunctions[funcIndex]) {
                bgColor = cachedBgFunctions[funcIndex]->getColor(transformedPosition);
            }
            
            CRGB audioColor = CRGB::Black;
            if(funcIndex >= 0 && funcIndex < step.numAudioFunctions && cachedAudioFunctions[funcIndex]) {
                audioColor = cachedAudioFunctions[funcIndex]->getColor(transformedPosition);
            }
            
            uint8_t blendAmount = (uint8_t)(audioFadeProgress * 255);
            currentColor = blend(bgColor, audioColor, blendAmount);
        } else {
            if(funcIndex >= 0 && funcIndex < step.numAudioFunctions && cachedAudioFunctions[funcIndex]) {
                currentColor = cachedAudioFunctions[funcIndex]->getColor(transformedPosition);
            } else {
                currentColor = CRGB::Black;
            }
        }
    } else {
        currentColor = model->getcolorfunction(edgeindex, position);
    }
    
    if(!inTransition || previousStep < 0 || previousStep >= numSteps) {
        return currentColor;
    }
    
    colormodel* prevModel = steps[previousStep].model;
    if(!prevModel || prevModel == model) {
        return currentColor;
    }
    
    CRGB previousColor = prevModel->getcolorfunction(edgeindex, position);
    TransitionType transType = steps[currentStep].transitionType;
    
    if(transType == FADE) {
        uint8_t blendAmount = (uint8_t)(transitionProgress * 255.0f);
        return blend(previousColor, currentColor, blendAmount);
    } else if(transType == WIPE) {
        float edgeTransitionPoint = edgeindex / 119.0f;
        const float transitionWidth = 0.05f;
        float edgeProgress = (transitionProgress - edgeTransitionPoint) / transitionWidth;
        
        if(edgeProgress <= 0.0f) return previousColor;
        if(edgeProgress >= 1.0f) return currentColor;
        
        uint8_t blendAmount = (uint8_t)(edgeProgress * 255.0f);
        return blend(previousColor, currentColor, blendAmount);
    }
    
    return currentColor;
}

int modelsequence::getCurrentStep() const {
    if(currentRegistryIndex >= 0) {
        return currentStep - registry[currentRegistryIndex].startStepIndex;
    }
    return currentStep;
}

float modelsequence::getProgress() const {
    if(currentStep < 0 || currentStep >= numSteps) return 0.0;
    unsigned long elapsed = millis() - stepStartTime;
    return (float)elapsed / steps[currentStep].duration;
}

void modelsequence::reset() {
    if(numRegistryEntries > 0) {
        currentRegistryIndex = 0;
        currentStep = registry[0].startStepIndex;
        registryStartTime = millis();
    } else if(numSteps > 0) {
        currentStep = 0;
    }
    
    stepStartTime = millis();
    previousStep = -1;
    inTransition = false;
    transitionProgress = 0.0;
    
    audioActive = false;
    audioLastActiveTime = 0;
    audioFading = false;
    audioFadeProgress = 0.0;
    audioFadeStartTime = 0;
    fadingToAudio = false;
    
    applyFunctionsToModel();
    if(currentStep >= 0 && currentStep < numSteps) {
        configureAudioSource(steps[currentStep].audioConfig);
    }
}

void modelsequence::printSequenceInfo() {
    Serial.println("\n=== Sequence Info ===");
    Serial.print("Total steps: ");
    Serial.println(numSteps);
    Serial.print("Max steps: ");
    Serial.println(SEQ_MAX_STEPS);
    Serial.print("Current step: ");
    Serial.println(currentStep);
    Serial.print("Registry entries: ");
    Serial.println(numRegistryEntries);
    
    if(currentRegistryIndex >= 0) {
        Serial.print("Current registry: ");
        Serial.print(registry[currentRegistryIndex].name);
        Serial.print(" (");
        Serial.print(getCurrentStep());
        Serial.print("/");
        Serial.print(registry[currentRegistryIndex].numSteps);
        Serial.println(")");
    }
    
    Serial.print("Step struct size: ");
    Serial.println(sizeof(SequenceStep));
    Serial.print("Total step storage: ");
    Serial.print(sizeof(steps));
    Serial.println(" bytes");
    
    Serial.println("=====================\n");
}

const char* modelsequence::getCurrentRegistryName() const {
    if(currentRegistryIndex >= 0 && currentRegistryIndex < numRegistryEntries) {
        return registry[currentRegistryIndex].name;
    }
    return "None";
}

void modelsequence::configureAudioSource(const AudioSourceConfig& config) {
    switch(config.type) {
        case AudioSourceConfig::MICROPHONE:
            AudioSystem::useMicrophone();
            Serial.println("Audio: Microphone");
            break;
        case AudioSourceConfig::LINE_IN:
            AudioSystem::useLineIn();
            AudioSystem::setLineInLevel(0.8);
            Serial.println("Audio: Line in");
            break;
        case AudioSourceConfig::SD_CARD:
            if(AudioSystem::useSDCard(config.filename)) {
                AudioSystem::setLooping(config.loop);
                Serial.print("Audio: ");
                Serial.println(config.filename);
            }
            break;
    }
}

void modelsequence::printCacheStats() {
    Serial.println("\n=== Function Cache ===");
    Serial.print("Cached: ");
    Serial.println(sequenceFunctionCache.size());
    Serial.println("======================\n");
}

void modelsequence::clearFunctionCache() {
    sequenceFunctionCache.clear();
    Serial.println("Cache cleared");
}

////////////////////////////////////
// SequenceBuilder implementation

SequenceBuilder::SequenceBuilder(modelsequence* s)
    : seq(s), useDualPalettes(false), audioTimeoutSeconds(AUDIO_TIMEOUT_SECONDS) {}

void SequenceBuilder::setaudiosource(AudioSourceConfig config) {
    currentAudioSource = config;
}

void SequenceBuilder::setaudiopalette(std::initializer_list<FunctionDef> funcs) {
    currentAudioPalettes.clear();
    for(const auto& f : funcs) {
        currentAudioPalettes.push_back(f);
    }
    useDualPalettes = true;
}

void SequenceBuilder::setbackgroundpalette(std::initializer_list<FunctionDef> funcs) {
    currentBackgroundPalettes.clear();
    for(const auto& f : funcs) {
        currentBackgroundPalettes.push_back(f);
    }
    useDualPalettes = true;
}

void SequenceBuilder::setaudiotimeout(float seconds) {
    audioTimeoutSeconds = seconds;
}

void SequenceBuilder::addpalette(std::initializer_list<FunctionDef> funcs) {
    currentPalettes.clear();
    for(const auto& f : funcs) {
        currentPalettes.push_back(f);
    }
    useDualPalettes = false;
}

void SequenceBuilder::clearpalettes() {
    currentPalettes.clear();
    currentAudioPalettes.clear();
    currentBackgroundPalettes.clear();
    useDualPalettes = false;
}

FunctionWithPalette SequenceBuilder::buildFunctionWithPalette(const FunctionDef& def) {
    FunctionWithPalette result(def.functionName, def.paletteName);
    result.numParams = min((int)def.params.size(), SEQ_MAX_PARAMS);
    for(int i = 0; i < result.numParams; i++) {
        result.params[i] = def.params[i];
    }
    return result;
}

void SequenceBuilder::addstep(String modelName, float durationSeconds, 
                               TransitionType transition, float speed, bool acceptAudio) {
    colormodel* model = colormodel::findModelByName(modelName);
    if(!model) {
        Serial.println("Error: Model '" + modelName + "' not found");
        return;
    }
    
    unsigned long durationMs = (unsigned long)(durationSeconds * 1000.0);
    
    if(useDualPalettes) {
        std::vector<FunctionWithPalette> audioFunctions;
        std::vector<FunctionWithPalette> backgroundFunctions;
        
        for(const auto& def : currentAudioPalettes) {
            if(audioFunctions.size() >= SEQ_MAX_FUNCTIONS) break;
            audioFunctions.push_back(buildFunctionWithPalette(def));
        }
        
        for(const auto& def : currentBackgroundPalettes) {
            if(backgroundFunctions.size() >= SEQ_MAX_FUNCTIONS) break;
            backgroundFunctions.push_back(buildFunctionWithPalette(def));
        }
        
        seq->addStep(SequenceStep(model, audioFunctions, backgroundFunctions, durationMs, 
                                 acceptAudio, AUDIO_PALETTE_SWITCH_THRESHOLD, audioTimeoutSeconds, 
                                 transition, speed, currentAudioSource));
    } else {
        std::vector<FunctionWithPalette> functions;
        
        for(const auto& def : currentPalettes) {
            if(functions.size() >= SEQ_MAX_FUNCTIONS) break;
            functions.push_back(buildFunctionWithPalette(def));
        }
        
        seq->addStep(SequenceStep(model, functions, durationMs, transition, speed, 0, currentAudioSource));
    }
}

void SequenceBuilder::addstep(String modelName, String permName, float durationSeconds, 
                               TransitionType transition, float speed, bool acceptAudio) {
    String cacheKey = modelName + "_" + permName;
    colormodel* permutedModel = nullptr;
    
    auto it = permutedModelCache.find(cacheKey);
    if(it != permutedModelCache.end()) {
        permutedModel = it->second;
    } else {
        permutedModel = colormodel::applyEdgePermutation(modelName, permName, cacheKey);
        if(!permutedModel) {
            Serial.println("Error: Failed to create '" + cacheKey + "'");
            return;
        }
        permutedModelCache[cacheKey] = permutedModel;
    }
    
    unsigned long durationMs = (unsigned long)(durationSeconds * 1000.0);
    
    if(useDualPalettes) {
        std::vector<FunctionWithPalette> audioFunctions;
        std::vector<FunctionWithPalette> backgroundFunctions;
        
        for(const auto& def : currentAudioPalettes) {
            if(audioFunctions.size() >= SEQ_MAX_FUNCTIONS) break;
            audioFunctions.push_back(buildFunctionWithPalette(def));
        }
        
        for(const auto& def : currentBackgroundPalettes) {
            if(backgroundFunctions.size() >= SEQ_MAX_FUNCTIONS) break;
            backgroundFunctions.push_back(buildFunctionWithPalette(def));
        }
        
        seq->addStep(SequenceStep(permutedModel, audioFunctions, backgroundFunctions, durationMs, 
                                 acceptAudio, AUDIO_PALETTE_SWITCH_THRESHOLD, audioTimeoutSeconds, 
                                 transition, speed, currentAudioSource));
    } else {
        std::vector<FunctionWithPalette> functions;
        
        for(const auto& def : currentPalettes) {
            if(functions.size() >= SEQ_MAX_FUNCTIONS) break;
            functions.push_back(buildFunctionWithPalette(def));
        }
        
        seq->addStep(SequenceStep(permutedModel, functions, durationMs, transition, speed, 0, currentAudioSource));
    }
}

void SequenceBuilder::add(String modelName, std::initializer_list<FunctionDef> funcDefs, float durationSeconds) {
    colormodel* model = colormodel::findModelByName(modelName);
    if(!model) {
        Serial.println("Error: Model '" + modelName + "' not found");
        return;
    }
    
    std::vector<FunctionWithPalette> functions;
    
    for(const auto& def : funcDefs) {
        if(functions.size() >= SEQ_MAX_FUNCTIONS) break;
        functions.push_back(buildFunctionWithPalette(def));
    }
    
    unsigned long durationMs = (unsigned long)(durationSeconds * 1000.0f);
    seq->addStep(SequenceStep(model, functions, durationMs, INSTANT, 1.0f, 0, currentAudioSource));
}
