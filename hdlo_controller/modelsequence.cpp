#include "modelsequence.h"

modelsequence::modelsequence() 
  : numSteps(0),
    currentStepIndex(0),
    stepStartTime(0),
    transitionStartTime(0),
    inTransition(false),
    modelArray(nullptr),
    numModels(0),
    modelNames(nullptr),
    colorFunctionArray(nullptr),
    numColorFunctions(0),
    colorFunctionNames(nullptr),
    numRegistryEntries(0),
    currentRegistryIndex(0),
    registryStartTime(0),
    currentSequenceStartStep(0),
    currentSequenceNumSteps(0) {
  // Initialize function arrays to nullptr
  for(int i = 0; i < 4; i++) {
    currentFunctions[i] = nullptr;
    nextFunctions[i] = nullptr;
  }
}

void modelsequence::registerModels(colormodel** models, int count, String* names) {
  modelArray = models;
  numModels = count;
  modelNames = names;
}

void modelsequence::registerColorFunctions(ColorFunction* functions, int count, String* names) {
  colorFunctionArray = functions;
  numColorFunctions = count;
  colorFunctionNames = names;
}

int modelsequence::findModelByName(String name) {
  if(modelNames == nullptr) return -1;
  
  for(int i = 0; i < numModels; i++) {
    if(modelNames[i].equalsIgnoreCase(name)) {
      return i;
    }
  }
  return -1;
}

int modelsequence::findColorFunctionByName(String name) {
  if(colorFunctionNames == nullptr) return -1;
  
  for(int i = 0; i < numColorFunctions; i++) {
    if(colorFunctionNames[i].equalsIgnoreCase(name)) {
      return i;
    }
  }
  return -1;
}

bool modelsequence::addStep(colormodel* model, std::array<int, 4> colorFuncIndices,
                            unsigned long duration, TransitionType trans,
                            unsigned long transDur) {
  if(numSteps >= MAX_STEPS) {
    return false; // Sequence is full
  }
  
  steps[numSteps] = SequenceStep(model, colorFuncIndices, duration, trans, transDur);
  numSteps++;
  
  // Update the current registry entry's step count
  if(numRegistryEntries > 0) {
    registry[numRegistryEntries - 1].numSteps++;
  }
  
  return true;
}

bool modelsequence::addStepByName(String modelName, std::array<String, 4> colorFuncNames,
                                  unsigned long duration, TransitionType trans,
                                  unsigned long transDur) {
  // Find model by name
  int modelIdx = findModelByName(modelName);
  if(modelIdx < 0) {
    Serial.println("Error: Model '" + modelName + "' not found");
    return false;
  }
  
  // Find color functions by name
  std::array<int, 4> colorFuncIndices;
  for(int i = 0; i < 4; i++) {
    colorFuncIndices[i] = findColorFunctionByName(colorFuncNames[i]);
    if(colorFuncIndices[i] < 0) {
      Serial.println("Error: Color function '" + colorFuncNames[i] + "' not found");
      return false;
    }
  }
  
  // Add the step using indices
  return addStep(modelArray[modelIdx], colorFuncIndices, duration, trans, transDur);
}

void modelsequence::loadSequence(int index) {
  if(!hasRegisteredModels()) {
    Serial.println("Error: Cannot load sequence - models not registered");
    return;
  }
  
  // Clear any existing steps
  numSteps = 0;
  
  // Note: Actual sequence definitions are loaded from hdlo_models.cpp
  // via the initializeSequences() function which calls addStepByName()
  Serial.println("Warning: loadSequence called but sequences should be initialized via initializeSequences()");
}

void modelsequence::begin(ColorFunction* colorFunctionArray) {
  if(currentSequenceNumSteps == 0) {
    // Use all steps
    currentSequenceStartStep = 0;
    currentSequenceNumSteps = numSteps;
  }
  
  if(currentSequenceNumSteps == 0) return;
  
  currentStepIndex = currentSequenceStartStep;
  stepStartTime = millis();
  inTransition = false;
  
  // Load color functions for the first step
  for(int i = 0; i < 4; i++) {
    int funcIdx = steps[currentStepIndex].colorFunctionIndices[i];
    currentFunctions[i] = colorFunctionArray[funcIdx];
    steps[currentStepIndex].model->setColorFunction(i, "", colorFunctionArray[funcIdx]);
  }
}

void modelsequence::update(unsigned long currentTime, ColorFunction* colorFunctionArray) {
  if(currentSequenceNumSteps == 0) return;
  
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
    
    if(currentStep.transitionType == INSTANT || currentStep.transitionDuration == 0) {
      // Instant transition
      currentStepIndex = nextStepIndex;
      stepStartTime = currentTime;
      
      // Update color functions
      for(int i = 0; i < 4; i++) {
        int funcIdx = steps[currentStepIndex].colorFunctionIndices[i];
        currentFunctions[i] = colorFunctionArray[funcIdx];
        steps[currentStepIndex].model->setColorFunction(i, "", colorFunctionArray[funcIdx]);
      }
    } else {
      // Start transition
      inTransition = true;
      transitionStartTime = currentTime;
      
      // Load next functions
      for(int i = 0; i < 4; i++) {
        int funcIdx = steps[nextStepIndex].colorFunctionIndices[i];
        nextFunctions[i] = colorFunctionArray[funcIdx];
      }
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
      
      // Copy next functions to current
      for(int i = 0; i < 4; i++) {
        currentFunctions[i] = nextFunctions[i];
        steps[currentStepIndex].model->setColorFunction(i, "", nextFunctions[i]);
      }
    }
  }
}

CRGB modelsequence::getColor(int edgeindex, float position) {
  if(currentSequenceNumSteps == 0) return CRGB::Black;
  
  SequenceStep& currentStep = steps[currentStepIndex];
  
  if(!inTransition) {
    // Just return current model color
    return currentStep.model->getcolorfunction(edgeindex, position);
  }
  
  // We're in transition - blend colors
  float progress = getTransitionProgress();
  int nextStepIndex = currentStepIndex + 1;
  
  // Wrap within current sequence
  if(nextStepIndex >= currentSequenceStartStep + currentSequenceNumSteps) {
    nextStepIndex = currentSequenceStartStep;
  }
  
  SequenceStep& nextStep = steps[nextStepIndex];
  
  CRGB currentColor = currentStep.model->getcolorfunction(edgeindex, position);
  CRGB nextColor = nextStep.model->getcolorfunction(edgeindex, position);
  
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
  return steps[currentStepIndex].model->getModelName();
}

void modelsequence::reset() {
  currentStepIndex = currentSequenceStartStep;
  stepStartTime = millis();
  inTransition = false;
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

bool modelsequence::startNewSequence(String name, unsigned long duration, bool enabled) {
  if(numRegistryEntries >= MAX_REGISTRY_ENTRIES) {
    Serial.println("Error: Registry is full");
    return false;
  }
  
  // Record where this sequence starts and how many steps have been added
  int startStep = numSteps;
  
  // Store registry entry (numSteps will be updated when steps are added)
  registry[numRegistryEntries] = SequenceRegistryEntry(startStep, 0, name, duration, enabled);
  numRegistryEntries++;
  
  return true;
}

void modelsequence::clearRegistry() {
  numRegistryEntries = 0;
  currentRegistryIndex = 0;
  numSteps = 0;  // Also clear all steps
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
  
  if(!hasRegisteredModels()) {
    Serial.println("Error: Cannot begin registry - models not registered");
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
  begin(colorFunctionArray);
  registryStartTime = millis();
  
  Serial.println("Registry started with: " + registry[currentRegistryIndex].name + 
                 " (" + String(currentSequenceNumSteps) + " steps)");
}

bool modelsequence::updateRegistry(unsigned long currentTime) {
  if(numRegistryEntries == 0 || !hasRegisteredModels()) {
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
    begin(colorFunctionArray);
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