#ifndef MODELSEQUENCE_H
#define MODELSEQUENCE_H

#include "models.h"
#include "colorfunctions.h"  // For numcolorfunctions constant
#include "audiosystem.h"      // For audio source control
#include <array>

// Enum for transition types
enum TransitionType {
  INSTANT,      // No transition, immediate switch
  FADE,         // Cross-fade between models
  WIPE          // Sequential wipe across edges
};

// Enum for audio source types
enum AudioSourceType {
  AUDIO_KEEP_CURRENT,    // Don't change audio source
  AUDIO_MICROPHONE,      // Use microphone input
  AUDIO_SD_CARD          // Use SD card playback
};

// Structure for audio source configuration
struct AudioSourceConfig {
  AudioSourceType type;
  String filename;       // SD card filename (if type == AUDIO_SD_CARD)
  float playbackRate;    // Playback rate (0.01 to 4.0)
  bool looping;          // Loop playback
  
  AudioSourceConfig() 
    : type(AUDIO_KEEP_CURRENT), filename(""), playbackRate(1.0), looping(false) {}
    
  AudioSourceConfig(AudioSourceType t, String file = "", float rate = 1.0, bool loop = false)
    : type(t), filename(file), playbackRate(rate), looping(loop) {}
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

// Structure for a single sequence step - now uses strings instead of indices
struct SequenceStep {
  String modelName;                      // Name of the color model
  std::array<String, numcolorfunctions> colorFunctionNames; // Names of color functions
  unsigned long duration;                // Duration in milliseconds
  TransitionType transitionType;         // Type of transition to next step
  unsigned long transitionDuration;      // Transition duration in milliseconds
  AudioSourceConfig audioConfig;         // Audio source configuration
  
  SequenceStep() 
    : modelName(""), 
      colorFunctionNames{},
      duration(5000), 
      transitionType(INSTANT),
      transitionDuration(0),
      audioConfig() {}
      
  SequenceStep(String model, std::array<String, numcolorfunctions> funcNames, 
               unsigned long dur, TransitionType trans = INSTANT, unsigned long transDur = 0,
               AudioSourceConfig audio = AudioSourceConfig())
    : modelName(model), 
      colorFunctionNames(funcNames),
      duration(dur),
      transitionType(trans),
      transitionDuration(transDur),
      audioConfig(audio) {}
};

class modelsequence {
private:
  static const int MAX_STEPS = 100;  // Increased for more complex sequences
  static const int MAX_REGISTRY_ENTRIES = 20;  // More sequences allowed
  
  SequenceStep steps[MAX_STEPS];
  int numSteps;
  int currentStepIndex;
  unsigned long stepStartTime;
  unsigned long transitionStartTime;
  bool inTransition;
  
  // Cached pointers for current step (resolved at runtime)
  colormodel* currentModel;
  colormodel* nextModel;
  std::array<ColorFunction, numcolorfunctions> currentFunctions;
  std::array<ColorFunction, numcolorfunctions> nextFunctions;
  
  // Sequence registry
  SequenceRegistryEntry registry[MAX_REGISTRY_ENTRIES];
  int numRegistryEntries;
  int currentRegistryIndex;
  unsigned long registryStartTime;
  int currentSequenceStartStep;
  int currentSequenceNumSteps;
  
  // Resolve model and function names to actual pointers
  colormodel* resolveModel(const String& name);
  ColorFunction resolveColorFunction(const String& name);
  
  // Internal methods that work with milliseconds
  bool addStepInternal(String model, std::array<String, numcolorfunctions> funcNames, 
                       unsigned long durationMs, TransitionType trans, 
                       unsigned long transDurMs, AudioSourceConfig audio);
  bool startNewSequenceInternal(String name, unsigned long durationMs, bool enabled);
  
public:
  modelsequence();
  
  // Add a step to the sequence - accepts SECONDS and string names directly
  bool addStep(String modelName, std::array<String, numcolorfunctions> colorFuncNames,
               float durationSeconds, TransitionType trans = INSTANT,
               float transDurSeconds = 0, AudioSourceConfig audio = AudioSourceConfig());
  
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
  void begin();
  
  // Update the sequence (call in loop)
  void update(unsigned long currentTime);
  
  // Get color for a specific edge and position
  CRGB getColor(int edgeindex, float position);
  
  // Get current model name for debugging
  String getCurrentModelName();
  
  // Get direct access to current model
  colormodel* getCurrentModel() { return currentModel; }
  
  // Get next model (useful during transitions)
  colormodel* getNextModel() { return nextModel; }
  
  // Reset sequence to beginning
  void reset();
  
  // Configure audio source from config
  void configureAudioSource(const AudioSourceConfig& config);
  
  // Get sequence info
  int getCurrentStep() const { return currentStepIndex - currentSequenceStartStep; }
  int getTotalSteps() const { return currentSequenceNumSteps; }
  bool isInTransition() const { return inTransition; }
  float getTransitionProgress() const;
};

#endif // MODELSEQUENCE_H
