#ifndef MODELSEQUENCE_H
#define MODELSEQUENCE_H

#include "models.h"
#include "colorfunctions.h"  // For numcolorfunctions constant
#include "audiosystem.h"      // For audio source control
#include <array>
#include <initializer_list>

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
  
  // Auto-fallback settings
  bool enableFallback;   // Enable auto-fallback to SD when mic is silent
  String fallbackFile;   // SD card file to play when mic is silent (always loops)
  float fallbackRate;    // Playback rate for fallback file
  float silenceThreshold; // Audio level threshold for silence detection
  unsigned long silenceTimeout; // Milliseconds of silence before switching (default 5000)
  
  AudioSourceConfig() 
    : type(AUDIO_KEEP_CURRENT), filename(""), playbackRate(1.0), looping(false),
      enableFallback(false), fallbackFile(""), fallbackRate(1.0), 
      silenceThreshold(0.01), silenceTimeout(5000) {}
    
  AudioSourceConfig(AudioSourceType t, String file = "", float rate = 1.0, bool loop = true)
    : type(t), filename(file), playbackRate(rate), looping(loop),
      enableFallback(false), fallbackFile(""), fallbackRate(1.0),
      silenceThreshold(0.01), silenceTimeout(5000) {}
      
  // Constructor for microphone with fallback
  AudioSourceConfig(String fallbackFileName, float fallbackSpeed = 1.0, unsigned long silenceMs = 5000)
    : type(AUDIO_MICROPHONE), filename(""), playbackRate(1.0), looping(false),
      enableFallback(true), fallbackFile(fallbackFileName), fallbackRate(fallbackSpeed),
      silenceThreshold(0.01), silenceTimeout(silenceMs) {}
};

// Structure for sequence registry entry
struct SequenceRegistryEntry {
  int startStepIndex;           // Index of first step in this sequence
  int numSteps;                 // Number of steps in this sequence
  String name;                  // Sequence name
  unsigned long duration;       // How long to display this sequence (milliseconds)
  bool enabled;                 // Whether this sequence is active
  AudioSourceConfig audioConfig; // Audio configuration for entire sequence
  
  SequenceRegistryEntry()
    : startStepIndex(0), numSteps(0), name(""), duration(60000), enabled(false), audioConfig() {}
    
  SequenceRegistryEntry(int start, int count, String n, unsigned long dur, bool en = true, AudioSourceConfig audio = AudioSourceConfig())
    : startStepIndex(start), numSteps(count), name(n), duration(dur), enabled(en), audioConfig(audio) {}
};

// Store function name and optional palette override
struct FunctionWithPalette {
  String functionName;
  String paletteName;  // Empty string means use current/default palette
  
  FunctionWithPalette() : functionName(""), paletteName("") {}
  FunctionWithPalette(String func, String palette = "") : functionName(func), paletteName(palette) {}
};

// Structure for a single sequence step - now uses FunctionWithPalette
struct SequenceStep {
  String modelName;                      // Name of the color model
  std::array<FunctionWithPalette, numcolorfunctions> functions; // Functions with optional palettes
  unsigned long duration;                // Duration in milliseconds
  TransitionType transitionType;         // Type of transition to next step
  unsigned long transitionDuration;      // Transition duration in milliseconds
  
  SequenceStep() 
    : modelName(""), 
      functions{},
      duration(5000), 
      transitionType(INSTANT),
      transitionDuration(0) {}
};

// Helper struct for mixed string/pair initialization
struct FunctionSpecInit {
  String functionName;
  String paletteName;
  
  FunctionSpecInit(const char* func) : functionName(func), paletteName("") {}
  FunctionSpecInit(String func) : functionName(func), paletteName("") {}
  FunctionSpecInit(std::initializer_list<String> init) {
    auto it = init.begin();
    if(init.size() >= 1) functionName = *it;
    if(init.size() >= 2) paletteName = *(++it);
  }
  FunctionSpecInit(std::initializer_list<const char*> init) {
    auto it = init.begin();
    if(init.size() >= 1) functionName = String(*it);
    if(init.size() >= 2) paletteName = String(*(++it));
  }
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
  
  // Store original palettes to restore them after each step
  std::array<String, numcolorfunctions> originalPalettes;
  
  // Audio management
  unsigned long lastAudioActivityTime;
  bool isUsingFallback;
  AudioSourceConfig currentAudioConfig;
  
  // Resolve model and function names to actual pointers
  colormodel* resolveModel(const String& name);
  ColorFunction resolveColorFunction(const String& name);
  void applyPaletteOverrides(const std::array<FunctionWithPalette, numcolorfunctions>& funcs);
  void restoreOriginalPalettes();
  
  // Convert FunctionSpecInit to FunctionWithPalette
  FunctionWithPalette parseFunctionSpec(const FunctionSpecInit& spec);
  
  // Internal methods that work with milliseconds
  bool addStepInternal(String model, std::array<FunctionWithPalette, numcolorfunctions> funcs, 
                       unsigned long durationMs, TransitionType trans, 
                       unsigned long transDurMs);
  bool startNewSequenceInternal(String name, unsigned long durationMs, bool enabled, 
                                AudioSourceConfig audio);
  
public:
  modelsequence();
  
  // Original addStep for backward compatibility - accepts SECONDS and string names directly
  bool addStep(String modelName, std::array<String, numcolorfunctions> colorFuncNames,
               float durationSeconds, TransitionType trans = INSTANT,
               float transDurSeconds = 0);
  
  // New addStep that accepts mixed strings and {string, string} pairs
  bool addStep(String modelName, std::initializer_list<FunctionSpecInit> funcSpecs,
               float durationSeconds, TransitionType trans = INSTANT,
               float transDurSeconds = 0);
  
  // Sequence registry management - accepts SECONDS
  bool startNewSequence(String name, float durationSeconds, bool enabled = true, 
                       AudioSourceConfig audio = AudioSourceConfig());
  
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
  
  // Check audio levels and handle fallback
  void updateAudioFallback(unsigned long currentTime);
  
  // Get sequence info
  int getCurrentStep() const { return currentStepIndex - currentSequenceStartStep; }
  int getTotalSteps() const { return currentSequenceNumSteps; }
  bool isInTransition() const { return inTransition; }
  float getTransitionProgress() const;
};

#endif // MODELSEQUENCE_H
