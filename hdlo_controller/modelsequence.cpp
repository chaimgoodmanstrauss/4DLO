////////////////////////////////////
//
//   modelsequence.cpp
//
// Sequence implementation with integer ID lookups
//

#include "modelsequence.h"
#include "audiosystem.h"
#include "globalids.h"
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
    audioPalettes[0] = FunctionWithPalette(dark, 0);
    for(int i = 1; i < 7; i++) {
        audioPalettes[i] = FunctionWithPalette(fftfire, 0);
    }
    numAudioFunctions = 7;
    
    backgroundPalettes[0] = FunctionWithPalette(breathing, 0);
    backgroundPalettes[1] = FunctionWithPalette(perlin, heat);
    backgroundPalettes[2] = FunctionWithPalette(perlin, cloud);
    backgroundPalettes[3] = FunctionWithPalette(perlin, forest);
    backgroundPalettes[4] = FunctionWithPalette(perlin, rainbow);
    backgroundPalettes[5] = FunctionWithPalette(perlin, sunset);
    backgroundPalettes[6] = FunctionWithPalette(perlin, ocean);
    numBackgroundFunctions = 7;
}

void SequenceStep::initializeDefaultAudioPalette() {
    audioPalettes[0] = FunctionWithPalette(dark, 0);
    for(int i = 1; i < 7; i++) {
        audioPalettes[i] = FunctionWithPalette(fftfire, 0);
    }
    numAudioFunctions = 7;
}

////////////////////////////////////
// modelsequence implementation

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
      cachedStepIndex(-1),
      sameModelTransition(false) {
    for(int i = 0; i < SEQ_MAX_FUNCTIONS; i++) {
        cachedPrevFunctions[i] = nullptr;
    }
    Serial.print("Steps in EXTMEM at 0x");
    Serial.println((uint32_t)steps, HEX);
}

void modelsequence::addStep(const SequenceStep& step) {
    if(numSteps < SEQ_MAX_STEPS) {
        steps[numSteps] = step;
        Serial.print("Added step ");
        Serial.print(numSteps);
        Serial.print(": model=");
        Serial.print(step.model ? step.model->getModelId() : 0);
        Serial.println();
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

static uint32_t hashFunctionKey(const FunctionWithPalette& func) {
    return func.functionId * 100000 + func.paletteId * 100 + func.numParams;
}

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
    
    StatefulColorFunction* rawPtr = ColorFunctionFactory::getInstance().create(func.functionId);
    if(!rawPtr) return nullptr;
    
    if(func.paletteId != 0) {
        rawPtr->setPalette(func.paletteId);
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
    
    Serial.print("Cache func: ");
    Serial.println(func.functionId);
    
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
    
    if(audioLevel > step.audioThreshold) {
        audioLastActiveTime = currentTime;
        
        if(!audioActive) {
            audioActive = true;
            audioFading = true;
            fadingToAudio = true;
            audioFadeStartProgress = audioFadeProgress;
            audioFadeStartTime = currentTime;
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
    if(currentStep < 0 || currentStep >= numSteps || !steps[currentStep].model) return;
    
    if(!staticParamBufferInit) {
        staticParamBuffer.reserve(SEQ_MAX_PARAMS);
        staticParamBufferInit = true;
    }
    
    colormodel* model = steps[currentStep].model;
    const auto& step = steps[currentStep];
    
    updateAudioFade(step);
    
    bool useAudioPalette = (audioFadeProgress > 0.5f);
    int numFunctions = useAudioPalette ? step.numAudioFunctions : step.numBackgroundFunctions;
    
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
                model->setColorFunction(edge, func.functionId, func.paletteId, staticParamBuffer);
            }
        } else {
            staticParamBuffer.clear();
            model->setColorFunction(edge, dark, 0, staticParamBuffer);
        }
    }
}

void modelsequence::update() {
    unsigned long currentTime = millis();
    
    if(currentStep >= 0 && currentStep < numSteps) {
        updateAudioFade(steps[currentStep]);
    }
    
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
                audioCacheValid = false;
                applyFunctionsToModel();
                configureAudioSource(steps[currentStep].audioConfig);
            }
        }
    }
    
    if(inTransition) {
        unsigned long elapsed = currentTime - transitionStartTime;
        if(elapsed >= steps[currentStep].transitionDuration) {
            inTransition = false;
            transitionProgress = 1.0;
            sameModelTransition = false;
        } else {
            transitionProgress = (float)elapsed / steps[currentStep].transitionDuration;
        }
    }
    
    if(currentStep >= 0 && currentStep < numSteps) {
        unsigned long elapsed = currentTime - stepStartTime;
        if(elapsed >= steps[currentStep].duration) {
            previousStep = currentStep;
            
            int nextStep;
            if(currentRegistryIndex >= 0) {
                int registryEnd = registry[currentRegistryIndex].startStepIndex + 
                                 registry[currentRegistryIndex].numSteps;
                nextStep = currentStep + 1;
                if(nextStep >= registryEnd) {
                    nextStep = registry[currentRegistryIndex].startStepIndex;
                }
            } else {
                nextStep = (currentStep + 1) % numSteps;
            }
            
            sameModelTransition = false;
            if(steps[nextStep].transitionType == FADE && 
               steps[previousStep].model == steps[nextStep].model) {
                sameModelTransition = true;
                for(int i = 0; i < SEQ_MAX_FUNCTIONS; i++) {
                    cachedPrevFunctions[i] = cachedBgFunctions[i];
                }
            }
            
            currentStep = nextStep;
            stepStartTime = currentTime;
            
            if(steps[currentStep].transitionType != INSTANT) {
                inTransition = true;
                transitionStartTime = currentTime;
                transitionProgress = 0.0;
            }
            
            audioCacheValid = false;
            applyFunctionsToModel();
            configureAudioSource(steps[currentStep].audioConfig);
        }
    }
}

colormodel* modelsequence::getCurrentModel() {
    if(currentStep >= 0 && currentStep < numSteps) {
        return steps[currentStep].model;
    }
    return nullptr;
}

int modelsequence::getCurrentStep() const {
    return currentStep;
}

float modelsequence::getProgress() const {
    if(currentStep >= 0 && currentStep < numSteps) {
        unsigned long elapsed = millis() - stepStartTime;
        return (float)elapsed / steps[currentStep].duration;
    }
    return 0.0;
}

void modelsequence::updateCachedFunctions() {
    updateAudioFadeCache();
}

CRGB modelsequence::getColor(int edgeindex, float position) {
    if(currentStep < 0 || currentStep >= numSteps) return CRGB::Black;
    
    colormodel* model = steps[currentStep].model;
    if(!model) return CRGB::Black;
    
    updateAudioFadeCache();
    
    const auto& step = steps[currentStep];
    int funcIndex = model->getEdgeFunctionIndex(edgeindex);
    
    if(funcIndex < 0) return CRGB::Black;
    
    CRGB bgColor = CRGB::Black;
    CRGB audioColor = CRGB::Black;
    
    int bgIndex = funcIndex % step.numBackgroundFunctions;
    if(cachedBgFunctions[bgIndex]) {
        cachedBgFunctions[bgIndex]->updateIfNeeded(millis());
        bgColor = cachedBgFunctions[bgIndex]->getColor(position);
    }
    
    if(audioFadeProgress > 0.0f && step.acceptAudio) {
        int audioIndex = funcIndex % step.numAudioFunctions;
        if(cachedAudioFunctions[audioIndex]) {
            cachedAudioFunctions[audioIndex]->updateIfNeeded(millis());
            audioColor = cachedAudioFunctions[audioIndex]->getColor(position);
        }
        
        return blend(bgColor, audioColor, audioFadeProgress * 255);
    }
    
    if(inTransition && sameModelTransition && transitionProgress < 1.0f) {
        CRGB prevColor = CRGB::Black;
        int prevIndex = funcIndex % step.numBackgroundFunctions;
        if(cachedPrevFunctions[prevIndex]) {
            cachedPrevFunctions[prevIndex]->updateIfNeeded(millis());
            prevColor = cachedPrevFunctions[prevIndex]->getColor(position);
        }
        return blend(prevColor, bgColor, transitionProgress * 255);
    }
    
    return bgColor;
}

void modelsequence::reset() {
    currentStep = 0;
    previousStep = -1;
    stepStartTime = millis();
    inTransition = false;
    transitionProgress = 0.0;
    audioActive = false;
    audioFading = false;
    audioFadeProgress = 0.0;
    audioCacheValid = false;
    
    if(numRegistryEntries > 0) {
        currentRegistryIndex = 0;
        registryStartTime = millis();
        currentStep = registry[0].startStepIndex;
    }
    
    if(numSteps > 0 && steps[currentStep].model) {
        applyFunctionsToModel();
        configureAudioSource(steps[currentStep].audioConfig);
    }
}

void modelsequence::printSequenceInfo() {
    Serial.println("\n=== Sequence Info ===");
    Serial.print("Steps: ");
    Serial.println(numSteps);
    Serial.print("Current: ");
    Serial.println(currentStep);
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
            break;
        case AudioSourceConfig::LINE_IN:
            AudioSystem::useLineIn();
            AudioSystem::setLineInLevel(0.8);
            break;
        case AudioSourceConfig::SD_CARD:
            if(AudioSystem::useSDCard(config.filename)) {
                AudioSystem::setLooping(config.loop);
            }
            break;
    }
}

void modelsequence::printCacheStats() {
    Serial.print("Function cache size: ");
    Serial.println(sequenceFunctionCache.size());
}

void modelsequence::clearFunctionCache() {
    sequenceFunctionCache.clear();
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
    FunctionWithPalette result(def.functionId, def.paletteId);
    result.numParams = min((int)def.params.size(), SEQ_MAX_PARAMS);
    for(int i = 0; i < result.numParams; i++) {
        result.params[i] = def.params[i];
    }
    return result;
}

void SequenceBuilder::addstep(int modelId, float durationSeconds, 
                               TransitionType transition, float speed, bool acceptAudio) {
    colormodel* model = colormodel::findModelById(modelId);
    if(!model) {
        Serial.print("Error: Model ID ");
        Serial.print(modelId);
        Serial.println(" not found");
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

void SequenceBuilder::addstep(int modelId, int permId, float durationSeconds, 
                               TransitionType transition, float speed, bool acceptAudio) {
    uint32_t cacheKey = modelId * 10000 + permId;
    colormodel* permutedModel = nullptr;
    
    auto it = permutedModelCache.find(cacheKey);
    if(it != permutedModelCache.end()) {
        permutedModel = it->second;
    } else {
        // Create new ID for permuted model (dynamic range 5000+)
        static int dynamicModelId = 5000;
        permutedModel = colormodel::applyEdgePermutation(modelId, permId, dynamicModelId++);
        if(!permutedModel) {
            Serial.print("Error: Failed to create permuted model ");
            Serial.print(modelId);
            Serial.print(" + ");
            Serial.println(permId);
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

void SequenceBuilder::add(int modelId, std::initializer_list<FunctionDef> funcDefs, float durationSeconds) {
    colormodel* model = colormodel::findModelById(modelId);
    if(!model) {
        Serial.print("Error: Model ID ");
        Serial.print(modelId);
        Serial.println(" not found");
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
