////////////////////////////////////////////
//
//   modelsequence.cpp (ENHANCED)
//
// Implementation with inline palette specification support
//

#include "modelsequence.h"

// External function declarations
extern void switchPalette(String functionName, String paletteName);
extern String getCurrentPaletteName(String functionName);

modelsequence::modelsequence() 
  : numSteps(0),
    currentStepIndex(0),
    stepStartTime(0),
    transitionStartTime(0),
    inTransition(false),
    currentModel(nullptr),
    nextModel(nullptr),
    numRegistryEntries(0),
    currentRegistryIndex(0),
    registryStartTime(0),
    currentSequenceStartStep(0),
    currentSequenceNumSteps(0),
    lastAudioActivityTime(0),
    isUsingFallback(false) {
  // Initialize function arrays
  for(int i = 0; i < numcolorfunctions; i++) {
    currentFunctions[i] = nullptr;
    nextFunctions[i] = nullptr;
    originalPalettes[i] = "";
  }
}

// Resolve model name to pointer using the model registry
colormodel* modelsequence::resolveModel(const String& name) {
  return colormodel::findModelByName(name);
}

// Resolve color function name to function pointer
ColorFunction modelsequence::resolveColorFunction(const String& name) {
  int index = findColorFunctionByName(name);
  if(index >= 0 && index < numcolorfunctions) {
    return colorFunctionArray[index];
  }
  return nullptr;
}

// Parse FunctionSpecInit into FunctionWithPalette
FunctionWithPalette modelsequence::parseFunctionSpec(const FunctionSpecInit& spec) {
  return FunctionWithPalette(spec.functionName, spec.paletteName);
}

// Apply palette overrides for the current step
void modelsequence::applyPaletteOverrides(const std::array<FunctionWithPalette, numcolorfunctions>& funcs) {
  for(int i = 0; i < numcolorfunctions; i++) {
    if(funcs[i].functionName != "" && funcs[i].paletteName != "") {
      // Store the original palette before overriding
      originalPalettes[i] = getCurrentPaletteName(funcs[i].functionName);
      // Apply the palette override
      switchPalette(funcs[i].functionName, funcs[i].paletteName);
    }
  }
}

// Restore original palettes after the step completes
void modelsequence::restoreOriginalPalettes() {
  for(int i = 0; i < numcolorfunctions; i++) {
    if(originalPalettes[i] != "") {
      SequenceStep& currentStep = steps[currentStepIndex];
      if(currentStep.functions[i].functionName != "" && 
         currentStep.functions[i].paletteName != "") {
        switchPalette(currentStep.functions[i].functionName, originalPalettes[i]);
      }
      originalPalettes[i] = "";
    }
  }
}

// Internal method - works with milliseconds and FunctionWithPalette
bool modelsequence::addStepInternal(String model, std::array<FunctionWithPalette, numcolorfunctions> funcs,
                                    unsigned long durationMs, TransitionType trans,
                                    unsigned long transDurMs) {
  if(numSteps >= MAX_STEPS) {
    Serial.println("Error: Sequence is full (max " + String(MAX_STEPS) + " steps)");
    return false;
  }
  
  SequenceStep& step = steps[numSteps];
  step.modelName = model;
  step.functions = funcs;
  step.duration = durationMs;
  step.transitionType = trans;
  step.transitionDuration = transDurMs;
  
  numSteps++;
  
  // Update the current registry entry's step count
  if(numRegistryEntries > 0) {
    registry[numRegistryEntries - 1].numSteps++;
  }
  
  return true;
}

// Original public method - accepts SECONDS and string names (backward compatibility)
bool modelsequence::addStep(String modelName, std::array<String, numcolorfunctions> colorFuncNames,
                            float durationSeconds, TransitionType trans,
                            float transDurSeconds) {
  // Convert string array to FunctionWithPalette array
  std::array<FunctionWithPalette, numcolorfunctions> funcs;
  for(int i = 0; i < numcolorfunctions; i++) {
    funcs[i] = FunctionWithPalette(colorFuncNames[i]);
  }
  
  // Convert seconds to milliseconds
  unsigned long durationMs = (unsigned long)(durationSeconds * 1000.0f);
  unsigned long transDurMs = (unsigned long)(transDurSeconds * 1000.0f);
  
  // Add the step using internal method
  return addStepInternal(modelName, funcs, durationMs, trans, transDurMs);
}

// New public method - accepts mixed strings and {string, string} pairs
bool modelsequence::addStep(String modelName, std::initializer_list<FunctionSpecInit> funcSpecs,
                            float durationSeconds, TransitionType trans,
                            float transDurSeconds) {
  std::array<FunctionWithPalette, numcolorfunctions> funcs;
  
  int i = 0;
  for(const auto& spec : funcSpecs) {
    if(i < numcolorfunctions) {
      funcs[i] = parseFunctionSpec(spec);
      i++;
    }
  }
  
  // Convert seconds to milliseconds
  unsigned long durationMs = (unsigned long)(durationSeconds * 1000.0f);
  unsigned long transDurMs = (unsigned long)(transDurSeconds * 1000.0f);
  
  // Add the step using internal method
  return addStepInternal(modelName, funcs, durationMs, trans, transDurMs);
}

void modelsequence::begin() {
  if(currentSequenceNumSteps == 0) {
    // Use all steps if no specific sequence selected
    currentSequenceStartStep = 0;
    currentSequenceNumSteps = numSteps;
  }
  
  if(currentSequenceNumSteps == 0) {
    Serial.println("Error: No steps in sequence");
    return;
  }
  
  currentStepIndex = currentSequenceStartStep;
  stepStartTime = millis();
  inTransition = false;
  
  // Resolve and cache the model and functions for the first step
  SequenceStep& firstStep = steps[currentStepIndex];
  currentModel = resolveModel(firstStep.modelName);
  
  if(currentModel == nullptr) {
    Serial.println("Error: Model '" + firstStep.modelName + "' not found!");
    return;
  }
  
  // Apply palette overrides for the first step
  applyPaletteOverrides(firstStep.functions);
  
  for(int i = 0; i < numcolorfunctions; i++) {
    currentFunctions[i] = resolveColorFunction(firstStep.functions[i].functionName);
    if(currentFunctions[i] == nullptr && firstStep.functions[i].functionName != "") {
      Serial.println("Warning: Function '" + firstStep.functions[i].functionName + "' not found!");
    }
  }
  
  Serial.println("Sequence started: " + currentModel->getModelName() + " for " + 
                 String(firstStep.duration/1000.0f) + " seconds");
}

void modelsequence::update(unsigned long currentTime) {
  if(currentSequenceNumSteps == 0) return;
  
  // Check audio fallback status
  updateAudioFallback(currentTime);
  
  SequenceStep& currentStep = steps[currentStepIndex];
  
  // Check if we need to start a transition
  if(!inTransition && currentStep.transitionType != INSTANT &&
     (currentTime - stepStartTime) >= (currentStep.duration - currentStep.transitionDuration)) {
    
    // Prepare for transition
    inTransition = true;
    transitionStartTime = currentTime;
    
    // Prepare the next step
    int nextStepIndex = currentStepIndex + 1;
    if(nextStepIndex >= currentSequenceStartStep + currentSequenceNumSteps) {
      nextStepIndex = currentSequenceStartStep; // Loop back to sequence start
    }
    
    SequenceStep& nextStep = steps[nextStepIndex];
    nextModel = resolveModel(nextStep.modelName);
    
    if(nextModel == nullptr) {
      Serial.println("Error: Next model '" + nextStep.modelName + "' not found!");
      inTransition = false;
      return;
    }
    
    for(int i = 0; i < numcolorfunctions; i++) {
      nextFunctions[i] = resolveColorFunction(nextStep.functions[i].functionName);
    }
    
    Serial.println("Starting " + String(currentStep.transitionType == FADE ? "FADE" : "WIPE") + 
                   " transition to " + nextModel->getModelName());
  }
  
  // Check if it's time to move to the next step
  if((currentTime - stepStartTime) >= currentStep.duration) {
    // Restore original palettes before moving to next step
    restoreOriginalPalettes();
    
    // Move to next step
    currentStepIndex++;
    if(currentStepIndex >= currentSequenceStartStep + currentSequenceNumSteps) {
      currentStepIndex = currentSequenceStartStep; // Loop back to sequence start
    }
    
    stepStartTime = currentTime;
    inTransition = false;
    
    // Update cached pointers for the new step
    SequenceStep& newStep = steps[currentStepIndex];
    currentModel = resolveModel(newStep.modelName);
    
    if(currentModel == nullptr) {
      Serial.println("Error: Model '" + newStep.modelName + "' not found!");
      return;
    }
    
    // Apply palette overrides for the new step
    applyPaletteOverrides(newStep.functions);
    
    for(int i = 0; i < numcolorfunctions; i++) {
      currentFunctions[i] = resolveColorFunction(newStep.functions[i].functionName);
      if(currentFunctions[i] == nullptr && newStep.functions[i].functionName != "") {
        Serial.println("Warning: Function '" + newStep.functions[i].functionName + "' not found!");
      }
    }
    
    Serial.println("Step " + String(currentStepIndex - currentSequenceStartStep + 1) + "/" + 
                   String(currentSequenceNumSteps) + ": " + currentModel->getModelName() + 
                   " for " + String(newStep.duration/1000.0f) + " seconds");
  }
}

CRGB modelsequence::getColor(int edgeindex, float position) {
  if(currentSequenceNumSteps == 0 || currentModel == nullptr) return CRGB::Black;
  
  if(!inTransition) {
    // Just return current model color
    return currentModel->getcolorfunction(edgeindex, position);
  }
  
  // We're in transition - blend colors
  float progress = getTransitionProgress();
  
  if(nextModel == nullptr) return currentModel->getcolorfunction(edgeindex, position);
  
  SequenceStep& currentStep = steps[currentStepIndex];
  
  CRGB currentColor = currentModel->getcolorfunction(edgeindex, position);
  CRGB nextColor = nextModel->getcolorfunction(edgeindex, position);
  
  if(currentStep.transitionType == FADE) {
    // Linear blend
    return blend(currentColor, nextColor, progress * 255);
  } else if(currentStep.transitionType == WIPE) {
    // Wipe based on edge index
    float edgeProgress = (float)edgeindex / 120.0;
    if(progress < edgeProgress) {
      return currentColor;
    } else {
      return nextColor;
    }
  }
  
  return currentColor;
}

String modelsequence::getCurrentModelName() {
  if(currentModel != nullptr) {
    return currentModel->getModelName();
  }
  return "None";
}

void modelsequence::reset() {
  currentStepIndex = currentSequenceStartStep;
  stepStartTime = millis();
  inTransition = false;
  begin();
}

float modelsequence::getTransitionProgress() const {
  if(!inTransition) return 0.0f;
  
  const SequenceStep& currentStep = steps[currentStepIndex];
  unsigned long elapsed = millis() - transitionStartTime;
  
  if(elapsed >= currentStep.transitionDuration) {
    return 1.0f;
  }
  
  return (float)elapsed / (float)currentStep.transitionDuration;
}

void modelsequence::configureAudioSource(const AudioSourceConfig& config) {
  currentAudioConfig = config;
  lastAudioActivityTime = millis();
  isUsingFallback = false;
  
  switch(config.type) {
    case AUDIO_MICROPHONE:
      Serial.println("Switching to microphone input");
      if(config.enableFallback) {
        Serial.println("  Fallback enabled: " + config.fallbackFile + 
                      " after " + String(config.silenceTimeout/1000.0f) + "s silence");
      }
      // AudioSystem::setSource(MICROPHONE);
      break;
      
    case AUDIO_SD_CARD:
      Serial.println("Playing from SD: " + config.filename + 
                     " at rate " + String(config.playbackRate) +
                     (config.looping ? " (looping)" : ""));
      // AudioSystem::playFile(config.filename, config.playbackRate, config.looping);
      break;
      
    case AUDIO_KEEP_CURRENT:
    default:
      // Don't change audio source
      break;
  }
}

void modelsequence::updateAudioFallback(unsigned long currentTime) {
  // Only check fallback if we're using microphone with fallback enabled
  if(currentAudioConfig.type != AUDIO_MICROPHONE || !currentAudioConfig.enableFallback) {
    return;
  }
  
  // Get current audio level (this would interface with your audio system)
  // float audioLevel = AudioSystem::getCurrentLevel();
  float audioLevel = 0.02; // Placeholder - replace with actual audio level
  
  if(audioLevel > currentAudioConfig.silenceThreshold) {
    // Audio detected, reset timer and switch back to mic if needed
    lastAudioActivityTime = currentTime;
    
    if(isUsingFallback) {
      Serial.println("Audio detected, switching back to microphone");
      // AudioSystem::setSource(MICROPHONE);
      isUsingFallback = false;
    }
  } else {
    // Check if we've been silent long enough to switch to fallback
    if(!isUsingFallback && 
       (currentTime - lastAudioActivityTime) > currentAudioConfig.silenceTimeout) {
      Serial.println("Silence timeout, switching to fallback: " + currentAudioConfig.fallbackFile + 
                     " (looping at " + String(currentAudioConfig.fallbackRate) + "x)");
      // AudioSystem::playFile(currentAudioConfig.fallbackFile, 
      //                      currentAudioConfig.fallbackRate, true);  // Always loop fallback
      isUsingFallback = true;
    }
  }
}

// Internal method - works with milliseconds
bool modelsequence::startNewSequenceInternal(String name, unsigned long durationMs, bool enabled,
                                             AudioSourceConfig audio) {
  if(numRegistryEntries >= MAX_REGISTRY_ENTRIES) {
    Serial.println("Error: Registry is full (max " + String(MAX_REGISTRY_ENTRIES) + " sequences)");
    return false;
  }
  
  // Record where this sequence starts and how many steps have been added
  int startStep = numSteps;
  
  // Store registry entry (numSteps will be updated when steps are added)
  registry[numRegistryEntries] = SequenceRegistryEntry(startStep, 0, name, durationMs, enabled, audio);
  numRegistryEntries++;
  
  Serial.println("Started new sequence: " + name);
  return true;
}

// Public method - accepts SECONDS, converts to milliseconds
bool modelsequence::startNewSequence(String name, float durationSeconds, bool enabled,
                                     AudioSourceConfig audio) {
  unsigned long durationMs = (unsigned long)(durationSeconds * 1000.0f);
  return startNewSequenceInternal(name, durationMs, enabled, audio);
}

void modelsequence::clearRegistry() {
  numRegistryEntries = 0;
  currentRegistryIndex = 0;
  numSteps = 0;  // Also clear all steps
  Serial.println("Registry cleared");
}

SequenceRegistryEntry modelsequence::getRegistryEntry(int index) const {
  if(index >= 0 && index < numRegistryEntries) {
    return registry[index];
  }
  return SequenceRegistryEntry(); // Return empty entry
}

void modelsequence::beginRegistry() {
  if(numRegistryEntries == 0) {
    Serial.println("Error: No sequences in registry!");
    return;
  }
  
  // Find first enabled sequence
  currentRegistryIndex = 0;
  while(currentRegistryIndex < numRegistryEntries && !registry[currentRegistryIndex].enabled) {
    currentRegistryIndex++;
  }
  
  if(currentRegistryIndex >= numRegistryEntries) {
    Serial.println("Error: No enabled sequences in registry!");
    return;
  }
  
  SequenceRegistryEntry& entry = registry[currentRegistryIndex];
  currentSequenceStartStep = entry.startStepIndex;
  currentSequenceNumSteps = entry.numSteps;
  registryStartTime = millis();
  
  // Configure audio for this sequence
  configureAudioSource(entry.audioConfig);
  
  Serial.println("Starting registry with sequence: " + entry.name);
  begin();
}

bool modelsequence::updateRegistry(unsigned long currentTime) {
  if(numRegistryEntries == 0) return false;
  
  SequenceRegistryEntry& currentEntry = registry[currentRegistryIndex];
  
  // Check if it's time to switch to the next sequence
  if((currentTime - registryStartTime) >= currentEntry.duration) {
    // Find next enabled sequence
    int nextIndex = currentRegistryIndex;
    do {
      nextIndex = (nextIndex + 1) % numRegistryEntries;
    } while(!registry[nextIndex].enabled && nextIndex != currentRegistryIndex);
    
    if(nextIndex == currentRegistryIndex && !registry[nextIndex].enabled) {
      Serial.println("Warning: No enabled sequences found!");
      return false;
    }
    
    currentRegistryIndex = nextIndex;
    SequenceRegistryEntry& nextEntry = registry[currentRegistryIndex];
    currentSequenceStartStep = nextEntry.startStepIndex;
    currentSequenceNumSteps = nextEntry.numSteps;
    registryStartTime = currentTime;
    
    // Configure audio for the new sequence
    configureAudioSource(nextEntry.audioConfig);
    
    Serial.println("Switching to sequence: " + nextEntry.name);
    begin();
    return true;
  }
  
  return false;
}

String modelsequence::getCurrentRegistryName() const {
  if(currentRegistryIndex >= 0 && currentRegistryIndex < numRegistryEntries) {
    return registry[currentRegistryIndex].name;
  }
  return "";
}
