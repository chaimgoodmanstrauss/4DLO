////////////////////////////////////////////
//
//   modelsequence.cpp (UPDATED)
//
// Implementation of the cleaned-up modelsequence class
// No more pointer arrays - everything uses string names!
//

#include "modelsequence.h"

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
    currentSequenceNumSteps(0) {
  // Initialize function arrays
  for(int i = 0; i < numcolorfunctions; i++) {
    currentFunctions[i] = nullptr;
    nextFunctions[i] = nullptr;
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

// Internal method - works with milliseconds
bool modelsequence::addStepInternal(String model, std::array<String, numcolorfunctions> funcNames,
                                    unsigned long durationMs, TransitionType trans,
                                    unsigned long transDurMs, AudioSourceConfig audio) {
  if(numSteps >= MAX_STEPS) {
    Serial.println("Error: Sequence is full (max " + String(MAX_STEPS) + " steps)");
    return false;
  }
  
  steps[numSteps] = SequenceStep(model, funcNames, durationMs, trans, transDurMs, audio);
  numSteps++;
  
  // Update the current registry entry's step count
  if(numRegistryEntries > 0) {
    registry[numRegistryEntries - 1].numSteps++;
  }
  
  return true;
}

// Public method - accepts SECONDS, uses string names directly
bool modelsequence::addStep(String modelName, std::array<String, numcolorfunctions> colorFuncNames,
                            float durationSeconds, TransitionType trans,
                            float transDurSeconds, AudioSourceConfig audio) {
  // Convert seconds to milliseconds
  unsigned long durationMs = (unsigned long)(durationSeconds * 1000.0f);
  unsigned long transDurMs = (unsigned long)(transDurSeconds * 1000.0f);
  
  // Add the step using internal method
  return addStepInternal(modelName, colorFuncNames, durationMs, trans, transDurMs, audio);
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
  
  // Load color functions for the first step
  for(int i = 0; i < numcolorfunctions; i++) {
    currentFunctions[i] = resolveColorFunction(firstStep.colorFunctionNames[i]);
    if(currentModel != nullptr && currentFunctions[i] != nullptr) {
      currentModel->setColorFunction(i, firstStep.colorFunctionNames[i], currentFunctions[i]);
    }
  }
  
  // Configure audio source for the first step
  configureAudioSource(firstStep.audioConfig);
  
  Serial.println("Started step: " + firstStep.modelName);
}

void modelsequence::update(unsigned long currentTime) {
  if(currentSequenceNumSteps == 0 || currentModel == nullptr) return;
  
  SequenceStep& currentStep = steps[currentStepIndex];
  unsigned long elapsed = currentTime - stepStartTime;
  
  // Check if we should start transitioning
  if(!inTransition && elapsed >= currentStep.duration) {
    // Time to move to next step (within current sequence)
    int nextStepIndex = currentStepIndex + 1;
    
    // Wrap within current sequence
    if(nextStepIndex >= currentSequenceStartStep + currentSequenceNumSteps) {
      nextStepIndex = currentSequenceStartStep;
    }
    
    SequenceStep& nextStep = steps[nextStepIndex];
    
    if(currentStep.transitionType == INSTANT || currentStep.transitionDuration == 0) {
      // Instant transition
      currentStepIndex = nextStepIndex;
      stepStartTime = currentTime;
      
      // Resolve next model
      currentModel = resolveModel(nextStep.modelName);
      if(currentModel == nullptr) {
        Serial.println("Error: Model '" + nextStep.modelName + "' not found!");
        return;
      }
      
      // Update color functions
      for(int i = 0; i < numcolorfunctions; i++) {
        currentFunctions[i] = resolveColorFunction(nextStep.colorFunctionNames[i]);
        if(currentModel != nullptr && currentFunctions[i] != nullptr) {
          currentModel->setColorFunction(i, nextStep.colorFunctionNames[i], currentFunctions[i]);
        }
      }
      
      // Configure audio source
      configureAudioSource(nextStep.audioConfig);
      
      Serial.println("Instant switch to: " + nextStep.modelName);
    } else {
      // Start transition
      inTransition = true;
      transitionStartTime = currentTime;
      
      // Resolve next model
      nextModel = resolveModel(nextStep.modelName);
      if(nextModel == nullptr) {
        Serial.println("Error: Model '" + nextStep.modelName + "' not found!");
        return;
      }
      
      // Load next functions
      for(int i = 0; i < numcolorfunctions; i++) {
        nextFunctions[i] = resolveColorFunction(nextStep.colorFunctionNames[i]);
        if(nextModel != nullptr && nextFunctions[i] != nullptr) {
          nextModel->setColorFunction(i, nextStep.colorFunctionNames[i], nextFunctions[i]);
        }
      }
      
      Serial.println("Starting transition to: " + nextStep.modelName);
    }
  }
  
  // Handle ongoing transition
  if(inTransition) {
    unsigned long transElapsed = currentTime - transitionStartTime;
    
    if(transElapsed >= currentStep.transitionDuration) {
      // Transition complete
      inTransition = false;
      currentStepIndex++;
      
      // Wrap within current sequence
      if(currentStepIndex >= currentSequenceStartStep + currentSequenceNumSteps) {
        currentStepIndex = currentSequenceStartStep;
      }
      
      stepStartTime = currentTime;
      
      // Swap models and functions
      currentModel = nextModel;
      for(int i = 0; i < numcolorfunctions; i++) {
        currentFunctions[i] = nextFunctions[i];
      }
      
      Serial.println("Transition complete to: " + steps[currentStepIndex].modelName);
    }
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
  if(currentSequenceNumSteps == 0) return "No models";
  return steps[currentStepIndex].modelName;
}

void modelsequence::reset() {
  currentStepIndex = currentSequenceStartStep;
  stepStartTime = millis();
  inTransition = false;
  
  // Re-resolve the first step
  if(currentSequenceNumSteps > 0) {
    SequenceStep& firstStep = steps[currentStepIndex];
    currentModel = resolveModel(firstStep.modelName);
    
    for(int i = 0; i < numcolorfunctions; i++) {
      currentFunctions[i] = resolveColorFunction(firstStep.colorFunctionNames[i]);
      if(currentModel != nullptr && currentFunctions[i] != nullptr) {
        currentModel->setColorFunction(i, firstStep.colorFunctionNames[i], currentFunctions[i]);
      }
    }
  }
}

float modelsequence::getTransitionProgress() const {
  if(!inTransition) return 0.0;
  
  unsigned long elapsed = millis() - transitionStartTime;
  unsigned long duration = steps[currentStepIndex].transitionDuration;
  
  if(duration == 0) return 1.0;
  
  float progress = (float)elapsed / (float)duration;
  return constrain(progress, 0.0, 1.0);
}

// Sequence Registry Methods

// Internal method - works with milliseconds
bool modelsequence::startNewSequenceInternal(String name, unsigned long durationMs, bool enabled) {
  if(numRegistryEntries >= MAX_REGISTRY_ENTRIES) {
    Serial.println("Error: Registry is full (max " + String(MAX_REGISTRY_ENTRIES) + " sequences)");
    return false;
  }
  
  // Record where this sequence starts and how many steps have been added
  int startStep = numSteps;
  
  // Store registry entry (numSteps will be updated when steps are added)
  registry[numRegistryEntries] = SequenceRegistryEntry(startStep, 0, name, durationMs, enabled);
  numRegistryEntries++;
  
  Serial.println("Started new sequence: " + name);
  return true;
}

// Public method - accepts SECONDS, converts to milliseconds
bool modelsequence::startNewSequence(String name, float durationSeconds, bool enabled) {
  unsigned long durationMs = (unsigned long)(durationSeconds * 1000.0f);
  return startNewSequenceInternal(name, durationMs, enabled);
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
    Serial.println("Error: No sequences in registry");
    return;
  }
  
  // Find first enabled sequence
  currentRegistryIndex = 0;
  while(currentRegistryIndex < numRegistryEntries && !registry[currentRegistryIndex].enabled) {
    currentRegistryIndex++;
  }
  
  if(currentRegistryIndex >= numRegistryEntries) {
    Serial.println("Error: No enabled sequences in registry");
    return;
  }
  
  // Set up to play this sequence
  currentSequenceStartStep = registry[currentRegistryIndex].startStepIndex;
  currentSequenceNumSteps = registry[currentRegistryIndex].numSteps;
  
  // Initialize the sequence
  begin();
  registryStartTime = millis();
  
  Serial.println("Registry started with: " + registry[currentRegistryIndex].name + 
                 " (" + String(currentSequenceNumSteps) + " steps)");
}

bool modelsequence::updateRegistry(unsigned long currentTime) {
  if(numRegistryEntries == 0) {
    return false;
  }
  
  // Check if it's time to switch sequences
  unsigned long elapsed = currentTime - registryStartTime;
  if(elapsed >= registry[currentRegistryIndex].duration) {
    // Find next enabled sequence
    int startIndex = currentRegistryIndex;
    do {
      currentRegistryIndex = (currentRegistryIndex + 1) % numRegistryEntries;
    } while(!registry[currentRegistryIndex].enabled && currentRegistryIndex != startIndex);
    
    if(!registry[currentRegistryIndex].enabled) {
      Serial.println("Warning: No enabled sequences found");
      return false;
    }
    
    // Set up to play this sequence
    currentSequenceStartStep = registry[currentRegistryIndex].startStepIndex;
    currentSequenceNumSteps = registry[currentRegistryIndex].numSteps;
    
    // Initialize the new sequence
    begin();
    registryStartTime = currentTime;
    
    Serial.println("Switched to: " + registry[currentRegistryIndex].name + 
                   " (" + String(currentSequenceNumSteps) + " steps)");
    return true;
  }
  
  return false;
}

String modelsequence::getCurrentRegistryName() const {
  if(currentRegistryIndex >= 0 && currentRegistryIndex < numRegistryEntries) {
    return registry[currentRegistryIndex].name;
  }
  return "None";
}

void modelsequence::configureAudioSource(const AudioSourceConfig& config) {
  if(config.type == AUDIO_KEEP_CURRENT) {
    return;  // Don't change audio source
  }
  
  if(config.type == AUDIO_MICROPHONE) {
    AudioSystem::useMicrophone();
    Serial.println("Audio: Switched to microphone");
  }
  else if(config.type == AUDIO_SD_CARD) {
    if(config.filename.length() == 0) {
      Serial.println("Audio: No filename specified for SD card");
      return;
    }
    
    AudioSystem::setLooping(config.looping);
    if(AudioSystem::useSDCard(config.filename.c_str(), config.playbackRate)) {
      Serial.println("Audio: Playing " + config.filename + " at " + 
                    String(config.playbackRate * 100) + "%" + 
                    (config.looping ? " (looping)" : ""));
    } else {
      Serial.println("Audio: Failed to play " + config.filename);
    }
  }
}
