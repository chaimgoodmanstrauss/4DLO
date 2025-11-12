////////////////////////////////////
//
//   modelsequence.cpp
//
// Sequence implementation
//

#include "modelsequence.h"
#include "audiosystem.h"
#include <FastLED.h>  // For blend() function

modelsequence::modelsequence()
    : numSteps(0),
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
      audioFadeStartTime(0),
      fadingToAudio(false) {}

void modelsequence::addStep(const SequenceStep& step) {
    if(numSteps < MAX_STEPS) {
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
    }
}

void modelsequence::clearSteps() {
    numSteps = 0;
    currentStep = 0;
    stepStartTime = millis();
}

void modelsequence::beginRegistry(String name, float durationSeconds, bool enabled) {
    if(numRegistryEntries < MAX_REGISTRY_ENTRIES) {
        // Convert seconds to milliseconds
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

void modelsequence::updateAudioFade(const SequenceStep& step) {
    if(!step.acceptAudio) {
        // This step doesn't respond to audio
        audioActive = false;
        audioFading = false;
        audioFadeProgress = 0.0;
        return;
    }
    
    // Check if audio system is initialized
    if(!AudioSystem::isInitialized()) {
        return;
    }
    
    unsigned long currentTime = millis();
    float audioLevel = AudioSystem::getMaxBand();  // Use max band instead of average
    
    // Debug output every 2 seconds
    static unsigned long lastDebugTime = 0;
    if(currentTime - lastDebugTime > 2000) {
        Serial.print("Audio: maxBand=");
        Serial.print(audioLevel, 4);
        Serial.print(", avgLevel=");
        Serial.print(AudioSystem::getLevel(), 4);
        Serial.print(", threshold=");
        Serial.print(step.audioThreshold, 4);
        Serial.print(", fadeProgress=");
        Serial.print(audioFadeProgress, 3);
        Serial.print(", active=");
        Serial.println(audioActive ? "YES" : "NO");
        lastDebugTime = currentTime;
    }
    
    // Check if audio is above threshold
    if(audioLevel > step.audioThreshold) {
        audioLastActiveTime = currentTime;
        
        if(!audioActive) {
            // Start fading to audio palette
            audioActive = true;
            audioFading = true;
            fadingToAudio = true;
            audioFadeStartTime = currentTime;
            Serial.println(">>> AUDIO DETECTED - fading to audio palette");
        }
    }
    
    // Check if we should fade back to background
    if(audioActive && (currentTime - audioLastActiveTime) > (unsigned long)(step.audioTimeout * 1000.0f)) {
        if(!audioFading || fadingToAudio) {
            // Start fading back to background
            audioFading = true;
            fadingToAudio = false;
            audioFadeStartTime = currentTime;
            Serial.println("<<< AUDIO TIMEOUT - fading to background palette");
        }
    }
    
    // Update fade progress
    if(audioFading) {
        float fadeDuration = fadingToAudio ? AUDIO_FADE_IN_TIME : step.audioTimeout;
        unsigned long fadeElapsed = currentTime - audioFadeStartTime;
        float fadeProgress = min(1.0f, fadeElapsed / (fadeDuration * 1000.0f));
        
        if(fadingToAudio) {
            audioFadeProgress = fadeProgress;
        } else {
            audioFadeProgress = 1.0f - fadeProgress;
        }
        
        if(fadeProgress >= 1.0f) {
            audioFading = false;
            if(!fadingToAudio) {
                audioActive = false;
                audioFadeProgress = 0.0f;
            }
        }
    }
}


void modelsequence::applyFunctionsToModel() {
    if(currentStep < 0 || currentStep >= numSteps || !steps[currentStep].model) {
        Serial.println("applyFunctionsToModel: Invalid step or null model");
        return;
    }
    
    colormodel* model = steps[currentStep].model;
    const auto& step = steps[currentStep];
    
    // Update audio fade state
    updateAudioFade(step);
    
    // Determine which palette to use based on audio state
    bool useAudioPalette = (audioFadeProgress > 0.5f);
    int numFunctions = useAudioPalette ? step.numAudioFunctions : step.numBackgroundFunctions;
    
    Serial.print("Applying functions to model: ");
    Serial.print(model->getModelName());
    Serial.print(" (");
    Serial.print(numFunctions);
    Serial.print(" functions, audio=");
    Serial.print(audioFadeProgress);
    Serial.println(")");
    
    // First, clear ALL edges to ensure no old functions remain
    for(int edge = 0; edge < 120; edge++) {
        model->setColorFunction(edge, "dark", "", {});
    }
    
    int appliedCount = 0;
    int skippedCount = 0;
    
    // Apply each function to its designated edges based on model data
    for(int edge = 0; edge < 120; edge++) {
        int funcIndex = model->getEdgeFunctionIndex(edge);
        
        if(funcIndex >= 0) {
            // Use modulo to ensure we don't go out of bounds
            int safeIndex = funcIndex % numFunctions;
            
            const FunctionWithPalette& func = useAudioPalette ? 
                step.audioPalettes[safeIndex] : step.backgroundPalettes[safeIndex];
            
            model->setColorFunction(edge, func.functionName, func.paletteName, func.parameters);
            appliedCount++;
            
            // Debug output for first few edges
            if(edge < 3) {
                Serial.print("  Edge ");
                Serial.print(edge);
                Serial.print(" -> ");
                Serial.print(func.functionName);
                Serial.print(" / ");
                Serial.println(func.paletteName);
            }
        } else {
            skippedCount++;
            // Edge has no function index - already set to dark above
        }
    }
    
    Serial.print("Applied functions to ");
    Serial.print(appliedCount);
    Serial.print(" edges, skipped ");
    Serial.print(skippedCount);
    Serial.println(" edges");
}

void modelsequence::update() {
    unsigned long currentTime = millis();
    
    // Update audio fade state for current step CONTINUOUSLY
    if(currentStep >= 0 && currentStep < numSteps) {
        updateAudioFade(steps[currentStep]);
    }
    
    // Check if we need to switch registry entries
    if(currentRegistryIndex >= 0 && currentRegistryIndex < numRegistryEntries) {
        if(currentTime - registryStartTime >= registry[currentRegistryIndex].duration) {
            // Find next enabled entry
            int nextIndex = (currentRegistryIndex + 1) % numRegistryEntries;
            while(!registry[nextIndex].enabled && nextIndex != currentRegistryIndex) {
                nextIndex = (nextIndex + 1) % numRegistryEntries;
            }
            
            if(nextIndex != currentRegistryIndex) {
                previousStep = currentStep;  // Track for potential transition
                currentRegistryIndex = nextIndex;
                currentStep = registry[currentRegistryIndex].startStepIndex;
                registryStartTime = currentTime;
                stepStartTime = currentTime;
                inTransition = false;  // No transition between registry entries
                applyFunctionsToModel();
                configureAudioSource(steps[currentStep].audioConfig);
                
                Serial.print("Switched to registry: ");
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
            Serial.println("Transition complete");
        } else {
            transitionProgress = (float)elapsed / steps[currentStep].transitionDuration;
        }
    }
    
    // Check if we need to advance to next step
    if(currentStep >= 0 && currentStep < numSteps) {
        unsigned long elapsed = currentTime - stepStartTime;
        if(elapsed >= steps[currentStep].duration) {
            // Store previous step for transition blending
            previousStep = currentStep;
            
            // Advance to next step within current registry
            if(currentRegistryIndex >= 0) {
                int registryEnd = registry[currentRegistryIndex].startStepIndex + 
                                 registry[currentRegistryIndex].numSteps;
                int nextStep = currentStep + 1;
                
                if(nextStep >= registryEnd) {
                    nextStep = registry[currentRegistryIndex].startStepIndex;
                }
                
                currentStep = nextStep;
            } else {
                // No registry, just loop through all steps
                currentStep = (currentStep + 1) % numSteps;
            }
            
            stepStartTime = currentTime;
            
            // Start transition if specified
            if(steps[currentStep].transitionType != INSTANT && 
               steps[currentStep].transitionDuration > 0) {
                inTransition = true;
                transitionStartTime = currentTime;
                transitionProgress = 0.0;
                Serial.print("Starting transition: type=");
                Serial.print(steps[currentStep].transitionType == FADE ? "FADE" : "WIPE");
                Serial.print(", duration=");
                Serial.print(steps[currentStep].transitionDuration);
                Serial.println("ms");
            } else {
                inTransition = false;
                Serial.println("INSTANT transition");
            }
            
            // Always apply functions immediately (needed for current colors during transition)
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

CRGB modelsequence::getColor(int edgeindex, float position) {
    colormodel* model = getCurrentModel();
    if(!model) {
        return CRGB::Black;
    }
    
    const auto& step = steps[currentStep];
    CRGB currentColor;
    
    // Handle audio palette fading if this step accepts audio
    if(step.acceptAudio && audioFadeProgress > 0.001f && audioFadeProgress < 0.999f) {
        // Need to blend between background and audio palettes
        int funcIndex = model->getEdgeFunctionIndex(edgeindex);
        
        // Get color from background palette
        CRGB bgColor = CRGB::Black;
        if(funcIndex >= 0 && funcIndex < step.numBackgroundFunctions) {
            const FunctionWithPalette& bgFunc = step.backgroundPalettes[funcIndex];
            model->setColorFunction(edgeindex, bgFunc.functionName, bgFunc.paletteName, bgFunc.parameters);
            bgColor = model->getcolorfunction(edgeindex, position);
        }
        
        // Get color from audio palette  
        CRGB audioColor = CRGB::Black;
        if(funcIndex >= 0 && funcIndex < step.numAudioFunctions) {
            const FunctionWithPalette& audioFunc = step.audioPalettes[funcIndex];
            model->setColorFunction(edgeindex, audioFunc.functionName, audioFunc.paletteName, audioFunc.parameters);
            audioColor = model->getcolorfunction(edgeindex, position);
        }
        
        // Blend between the two
        uint8_t blendAmount = (uint8_t)(audioFadeProgress * 255);
        currentColor = blend(bgColor, audioColor, blendAmount);
        
        // Restore the dominant function for consistency with applyFunctionsToModel
        if(audioFadeProgress > 0.5f) {
            if(funcIndex >= 0 && funcIndex < step.numAudioFunctions) {
                const FunctionWithPalette& func = step.audioPalettes[funcIndex];
                model->setColorFunction(edgeindex, func.functionName, func.paletteName, func.parameters);
            }
        } else {
            if(funcIndex >= 0 && funcIndex < step.numBackgroundFunctions) {
                const FunctionWithPalette& func = step.backgroundPalettes[funcIndex];
                model->setColorFunction(edgeindex, func.functionName, func.paletteName, func.parameters);
            }
        }
    } else {
        // Use single palette (either fully audio or fully background)
        currentColor = model->getcolorfunction(edgeindex, position);
    }
    
    // If not in transition, just return current color
    if(!inTransition || previousStep < 0 || previousStep >= numSteps) {
        return currentColor;
    }
    
    // Get previous step's color
    colormodel* prevModel = steps[previousStep].model;
    if(!prevModel) {
        return currentColor;  // Can't blend without previous model
    }
    
    // If same model used for consecutive steps, can't blend (functions already overwritten)
    if(prevModel == model) {
        return currentColor;  // Will appear as instant transition
    }
    
    CRGB previousColor = prevModel->getcolorfunction(edgeindex, position);
    
    // Blend based on transition type
    TransitionType transType = steps[currentStep].transitionType;
    
    if(transType == FADE) {
        // Simple linear fade between old and new
        uint8_t blendAmount = (uint8_t)(transitionProgress * 255.0f);
        return blend(previousColor, currentColor, blendAmount);
        
    } else if(transType == WIPE) {
        // Spatial wipe: edges transition based on their index
        // Progress 0.0 -> 1.0 sweeps through edges 0 -> 119
        float edgeTransitionPoint = edgeindex / 119.0f;  // Normalized edge position
        
        // Calculate blend amount for this edge
        // Each edge has a "window" where it transitions
        const float transitionWidth = 0.05f;  // 5% of edges transitioning at once (~6 edges)
        float edgeProgress = (transitionProgress - edgeTransitionPoint) / transitionWidth;
        
        // Clamp to 0.0 - 1.0
        if(edgeProgress <= 0.0f) {
            return previousColor;  // Not reached this edge yet
        } else if(edgeProgress >= 1.0f) {
            return currentColor;  // Already fully transitioned
        } else {
            uint8_t blendAmount = (uint8_t)(edgeProgress * 255.0f);
            return blend(previousColor, currentColor, blendAmount);
        }
    }
    
    // INSTANT or unknown - shouldn't reach here, but return current
    return currentColor;
}

int modelsequence::getCurrentStep() const {
    if(currentRegistryIndex >= 0) {
        return currentStep - registry[currentRegistryIndex].startStepIndex;
    }
    return currentStep;
}

float modelsequence::getProgress() const {
    if(currentStep < 0 || currentStep >= numSteps) {
        return 0.0;
    }
    
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
    
    // Reset audio fade state
    audioActive = false;
    audioLastActiveTime = 0;
    audioFading = false;
    audioFadeProgress = 0.0;
    audioFadeStartTime = 0;
    fadingToAudio = false;
    
    applyFunctionsToModel();
    if(currentStep < numSteps) {
        configureAudioSource(steps[currentStep].audioConfig);
    }
}

void modelsequence::printSequenceInfo() {
    Serial.println("\n=== Sequence Info ===");
    Serial.print("Total steps: ");
    Serial.println(numSteps);
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
    
    Serial.println("=====================\n");
}

String modelsequence::getCurrentRegistryName() const {
    if(currentRegistryIndex >= 0 && currentRegistryIndex < numRegistryEntries) {
        return registry[currentRegistryIndex].name;
    }
    return "None";
}

void modelsequence::configureAudioSource(const AudioSourceConfig& config) {
    switch(config.type) {
        case AudioSourceConfig::MICROPHONE:
            AudioSystem::useMicrophone();
            Serial.println("Audio: Switched to microphone");
            break;
            
        case AudioSourceConfig::LINE_IN:
            AudioSystem::useMicrophone();  // Line in uses same hardware as mic
            AudioSystem::setLineInLevel(0.8);  // Set appropriate line in level
            Serial.println("Audio: Switched to line in");
            break;
            
        case AudioSourceConfig::SD_CARD:
            if(AudioSystem::useSDCard(config.filename.c_str())) {
                AudioSystem::setLooping(config.loop);
                Serial.print("Audio: Playing ");
                Serial.print(config.filename);
                Serial.println(config.loop ? " (looping)" : "");
            } else {
                Serial.println("Audio: Failed to play SD card file");
            }
            break;
    }
}
