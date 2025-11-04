////////////////////////////////////////////
//
//   modelsequence.cpp (ENHANCED)
//
// Implementation with inline palette specification support
//

#include "modelsequence.h"
#include "models.h"

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

// Helper to safely convert seconds to milliseconds with overflow protection
static unsigned long safeSecondsToMillis(float seconds, const char* context) {
  const float MAX_DURATION_SECONDS = 4294967.0f; // Max unsigned long / 1000
  
  if(seconds > MAX_DURATION_SECONDS) {
    Serial.print("Warning: ");
    Serial.print(context);
    Serial.print(" duration ");
    Serial.print(seconds);
    Serial.print(" seconds exceeds max, capping at ");
    Serial.print(MAX_DURATION_SECONDS);
    Serial.println(" seconds");
    seconds = MAX_DURATION_SECONDS;
  }
  
  return (unsigned long)(seconds * 1000.0f);
}
void modelsequence::applyPaletteOverrides(const std::array<FunctionWithPalette, numcolorfunctions>& funcs) {
  // Clear any previous edge palette settings
  for(int edge = 0; edge < 120; edge++) {
    setEdgePalette(edge, "", "");
  }
  
  // CORRECT: Look at what function each edge actually uses in the model
  if(currentModel != nullptr) {
    for(int edge = 0; edge < 120; edge++) {
      // Get the function index this edge uses from the model
      int funcIndex = currentModel->getEdgeFunctionIndex(edge);
      
      // Check if this function has a palette override
      if(funcIndex >= 0 && funcIndex < numcolorfunctions) {
        if(funcs[funcIndex].functionName != "" && funcs[funcIndex].paletteName != "") {
          // Store the original palette before overriding (only once per function)
          if(originalPalettes[funcIndex] == "") {
            originalPalettes[funcIndex] = getCurrentPaletteName(funcs[funcIndex].functionName);
          }
          
          // Set the palette for THIS edge based on what function it actually uses
          setEdgePalette(edge, funcs[funcIndex].functionName, funcs[funcIndex].paletteName);
        }
      }
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
  
  // Convert seconds to milliseconds with overflow protection
  unsigned long durationMs = safeSecondsToMillis(durationSeconds, "Step");
  unsigned long transDurMs = safeSecondsToMillis(transDurSeconds, "Transition");
  
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
  
  // Convert seconds to milliseconds with overflow protection
  unsigned long durationMs = safeSecondsToMillis(durationSeconds, "Step");
  unsigned long transDurMs = safeSecondsToMillis(transDurSeconds, "Transition");
  
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
    // Apply the function to the model's prototype
    if(currentFunctions[i] != nullptr) {
      currentModel->setColorFunction(i, firstStep.functions[i].functionName, currentFunctions[i]);
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
      // Apply the function to the model's prototype
      if(currentFunctions[i] != nullptr) {
        currentModel->setColorFunction(i, newStep.functions[i].functionName, currentFunctions[i]);
      }
    }
    
    Serial.println("Step " + String(currentStepIndex - currentSequenceStartStep + 1) + "/" + 
                   String(currentSequenceNumSteps) + ": " + currentModel->getModelName() + 
                   " for " + String(newStep.duration/1000.0f) + " seconds");
  }
}

CRGB modelsequence::getColor(int edgeindex, float position) {
  if(currentSequenceNumSteps == 0 || currentModel == nullptr) {
    return CRGB::Black;
  }
  
  if(!inTransition) {
    return currentModel->getcolorfunction(edgeindex, position);
  }
  
  // In transition - need both models to be valid
  if(nextModel == nullptr) {
    return currentModel->getcolorfunction(edgeindex, position);
  }
  
  float progress = getTransitionProgress();
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
      AudioSystem::useMicrophone();
      break;
      
    case AUDIO_SD_CARD:
      Serial.println("Playing from SD: " + config.filename + 
                     (config.looping ? " (looping)" : ""));
      AudioSystem::setLooping(config.looping);
      AudioSystem::useSDCard(config.filename.c_str(), 1.0);
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
  
  // Get current audio level from AudioSystem
  float audioLevel = AudioSystem::getLevel();
  
  if(audioLevel > currentAudioConfig.silenceThreshold) {
    // Audio detected, reset timer and switch back to mic if needed
    lastAudioActivityTime = currentTime;
    
    if(isUsingFallback) {
      Serial.println("Audio detected, switching back to microphone");
      AudioSystem::useMicrophone();
      isUsingFallback = false;
    }
  } else {
    // Check if we've been silent long enough to switch to fallback
    // Protect against unsigned overflow by checking currentTime >= lastAudioActivityTime
    if(!isUsingFallback && 
       currentTime >= lastAudioActivityTime &&
       (currentTime - lastAudioActivityTime) > currentAudioConfig.silenceTimeout) {
      Serial.println("Silence timeout, switching to fallback: " + currentAudioConfig.fallbackFile + " (looping)");
      AudioSystem::setLooping(true);  // Always loop fallback
      AudioSystem::useSDCard(currentAudioConfig.fallbackFile.c_str(), 1.0);
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
  unsigned long durationMs = safeSecondsToMillis(durationSeconds, "Sequence");
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
  
  // Validate current index
  if(currentRegistryIndex < 0 || currentRegistryIndex >= numRegistryEntries) {
    Serial.println("Error: Invalid registry index, resetting to 0");
    currentRegistryIndex = 0;
    registryStartTime = currentTime;
    return false;
  }
  
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
