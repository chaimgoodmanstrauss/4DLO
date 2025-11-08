////////////////////////////////////
//
//   modelsequence.cpp
//
// Sequence implementation
//

#include "modelsequence.h"
#include "audiosystem.h"

modelsequence::modelsequence()
    : numSteps(0),
      currentStep(0),
      stepStartTime(0),
      numRegistryEntries(0),
      currentRegistryIndex(-1),
      registryStartTime(0),
      inTransition(false),
      transitionStartTime(0),
      transitionProgress(0.0) {}

void modelsequence::addStep(const SequenceStep& step) {
    if(numSteps < MAX_STEPS) {
        steps[numSteps] = step;
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

void modelsequence::applyFunctionsToModel() {
    if(currentStep < 0 || currentStep >= numSteps || !steps[currentStep].model) {
        Serial.println("applyFunctionsToModel: Invalid step or null model");
        return;
    }
    
    colormodel* model = steps[currentStep].model;
    const auto& step = steps[currentStep];
    
    Serial.print("Applying functions to model: ");
    Serial.print(model->getModelName());
    Serial.print(" (");
    Serial.print(step.numFunctions);
    Serial.println(" functions)");
    
    int appliedCount = 0;
    // Apply each function to its designated edges based on model data
    for(int edge = 0; edge < 120; edge++) {
        int funcIndex = model->getEdgeFunctionIndex(edge);
        if(funcIndex >= 0 && funcIndex < step.numFunctions) {
            const FunctionWithPalette& func = step.functions[funcIndex];
            model->setColorFunction(edge, func.functionName, func.paletteName, func.parameters);
            appliedCount++;
        }
    }
    
    Serial.print("Applied functions to ");
    Serial.print(appliedCount);
    Serial.println(" edges");
}

void modelsequence::update() {
    unsigned long currentTime = millis();
    
    // Check if we need to switch registry entries
    if(currentRegistryIndex >= 0 && currentRegistryIndex < numRegistryEntries) {
        if(currentTime - registryStartTime >= registry[currentRegistryIndex].duration) {
            // Find next enabled entry
            int nextIndex = (currentRegistryIndex + 1) % numRegistryEntries;
            while(!registry[nextIndex].enabled && nextIndex != currentRegistryIndex) {
                nextIndex = (nextIndex + 1) % numRegistryEntries;
            }
            
            if(nextIndex != currentRegistryIndex) {
                currentRegistryIndex = nextIndex;
                currentStep = registry[currentRegistryIndex].startStepIndex;
                registryStartTime = currentTime;
                stepStartTime = currentTime;
                inTransition = false;
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
        if(elapsed >= steps[currentStep - 1].transitionDuration) {
            inTransition = false;
            transitionProgress = 1.0;
        } else {
            transitionProgress = (float)elapsed / steps[currentStep - 1].transitionDuration;
        }
    }
    
    // Check if we need to advance to next step
    if(currentStep >= 0 && currentStep < numSteps) {
        unsigned long elapsed = currentTime - stepStartTime;
        if(elapsed >= steps[currentStep].duration) {
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
            }
            
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
    if(model) {
        return model->getcolorfunction(edgeindex, position);
    }
    return CRGB::Black;
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
    inTransition = false;
    transitionProgress = 0.0;
    
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
    // Audio system configuration would go here
    // For now, just log what we would do
    switch(config.type) {
        case AudioSourceConfig::MICROPHONE:
            Serial.println("Audio: Would switch to microphone");
            break;
            
        case AudioSourceConfig::LINE_IN:
            Serial.println("Audio: Would switch to line in");
            break;
            
        case AudioSourceConfig::SD_CARD:
            Serial.print("Audio: Would play ");
            Serial.print(config.filename);
            Serial.println(config.loop ? " (looping)" : "");
            break;
    }
}
