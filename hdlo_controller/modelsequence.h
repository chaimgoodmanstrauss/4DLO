#ifndef MODELSEQUENCE_H
#define MODELSEQUENCE_H

#include "models.h"
#include "colorfunctions.h"  // For numcolorfunctions constant
#include <array>

// Enum for transition types
enum TransitionType {
  INSTANT,      // No transition, immediate switch
  FADE,         // Cross-fade between models
  WIPE          // Sequential wipe across edges
};

// Structure for sequence registry entry
struct SequenceRegistryEntry {
  int startStepIndex;           // Index of first step in this sequence
  int numSteps;                 // Number of steps in this sequence
  String name;                  // Sequence name
  unsigned long duration;       // How long to display this sequence (milliseconds)
  bool enabled;                 // Whether this sequence is active
  
  SequenceRegistryEntry()
    : startStepIndex(0), numSteps(0), name(""), duration(60000), enabled(false) {}
    
  SequenceRegistryEntry(int start, int count, String n, unsigned long dur, bool en = true)
    : startStepIndex(start), numSteps(count), name(n), duration(dur), enabled(en) {}
};

// Structure for a single sequence step
struct SequenceStep {
  colormodel* model;                    // Pointer to the color model
  std::array<int, numcolorfunctions> colorFunctionIndices; // Indices for color functions to use
  unsigned long duration;               // Duration in milliseconds
  TransitionType transitionType;        // Type of transition to next step
  unsigned long transitionDuration;     // Transition duration in milliseconds
  
  SequenceStep() 
    : model(nullptr), 
      colorFunctionIndices{},  // Empty braces = zero-initialize all elements
      duration(5000), 
      transitionType(INSTANT),
      transitionDuration(0) {}
      
  SequenceStep(colormodel* m, std::array<int, numcolorfunctions> funcIndices, unsigned long dur, 
               TransitionType trans = INSTANT, unsigned long transDur = 0)
    : model(m), 
      colorFunctionIndices(funcIndices),
      duration(dur),
      transitionType(trans),
      transitionDuration(transDur) {}
};

class modelsequence {
private:
  static const int MAX_STEPS = 20;
  static const int MAX_REGISTRY_ENTRIES = 10;
  
  SequenceStep steps[MAX_STEPS];
  int numSteps;
  int currentStepIndex;
  unsigned long stepStartTime;
  unsigned long transitionStartTime;
  bool inTransition;
  
  // Color function arrays for current and next steps
  ColorFunction currentFunctions[numcolorfunctions];
  ColorFunction nextFunctions[numcolorfunctions];
  
  // Storage for name lookups
  colormodel** modelArray;
  int numModels;
  String* modelNames;
  
  ColorFunction* colorFunctionArray;
  int numColorFunctions;
  String* colorFunctionNames;
  
  // Sequence registry
  SequenceRegistryEntry registry[MAX_REGISTRY_ENTRIES];
  int numRegistryEntries;
  int currentRegistryIndex;
  unsigned long registryStartTime;
  int currentSequenceStartStep;  // Track where current sequence starts
  int currentSequenceNumSteps;   // Track how many steps in current sequence
  
  // Helper methods for name lookup
  int findModelByName(String name);
  int findColorFunctionByName(String name);
  
  // Internal methods that work with milliseconds
  bool addStepInternal(colormodel* model, std::array<int, numcolorfunctions> colorFuncIndices, 
                       unsigned long durationMs, TransitionType trans, 
                       unsigned long transDurMs);
  bool startNewSequenceInternal(String name, unsigned long durationMs, bool enabled);
  
public:
  modelsequence();
  
  // Check if models are registered
  bool hasRegisteredModels() const { return modelArray != nullptr && numModels > 0; }
  
  // Register model and color function arrays for name lookup
  void registerModels(colormodel** models, int count, String* names);
  void registerColorFunctions(ColorFunction* functions, int count, String* names);
  
  // Add a step to the sequence (by name) - accepts SECONDS
  bool addStepByName(String modelName, std::array<String, numcolorfunctions> colorFuncNames,
                     float durationSeconds, TransitionType trans = INSTANT,
                     float transDurSeconds = 0);
  
  // Load predefined sequences (using registered models)
  void loadSequence(int index);
  
  // Load sequence by index (alias for consistency)
  void loadSequenceByIndex(int index) { loadSequence(index); }
  
  // Sequence registry management - accepts SECONDS
  bool startNewSequence(String name, float durationSeconds, bool enabled = true);
  
  void clearRegistry();
  int getRegistrySize() const { return numRegistryEntries; }
  SequenceRegistryEntry getRegistryEntry(int index) const;
  
  // Registry-based sequence cycling
  void beginRegistry(); // Initialize registry cycling
  bool updateRegistry(unsigned long currentTime); // Returns true if sequence changed
  int getCurrentRegistryIndex() const { return currentRegistryIndex; }
  String getCurrentRegistryName() const;
  
  // Initialize the sequence (call before first use)
  void begin(ColorFunction* colorFunctionArray);
  
  // Update the sequence (call in loop)
  void update(unsigned long currentTime, ColorFunction* colorFunctionArray);
  
  // Get color for a specific edge and position
  CRGB getColor(int edgeindex, float position);
  
  // Get current model name for debugging
  String getCurrentModelName();
  
  // NEW METHOD: Get direct access to current model
  colormodel* getCurrentModel() {
    if(currentSequenceNumSteps == 0 || currentStepIndex >= numSteps) {
      return nullptr;
    }
    return steps[currentStepIndex].model;
  }
  
  // NEW METHOD: Get next model (useful during transitions)
  colormodel* getNextModel() {
    if(currentSequenceNumSteps == 0) return nullptr;
    
    int nextStepIndex = currentStepIndex + 1;
    if(nextStepIndex >= currentSequenceStartStep + currentSequenceNumSteps) {
      nextStepIndex = currentSequenceStartStep;
    }
    
    return steps[nextStepIndex].model;
  }
  
  // Reset sequence to beginning
  void reset();
  
  // Get sequence info
  int getCurrentStep() const { return currentStepIndex - currentSequenceStartStep; }
  int getTotalSteps() const { return currentSequenceNumSteps; }
  bool isInTransition() const { return inTransition; }
  float getTransitionProgress() const;
};

#endif // MODELSEQUENCE_H