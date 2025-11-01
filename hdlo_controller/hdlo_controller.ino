////////////////////////////////////////////
//
//   hdlo_controller.ino (UPDATED)
//
// Example main sketch using the new cleaned-up system
// No more pointer registration needed!
//

#include <OctoWS2811.h>
#include <FastLED.h>
#include "teensy4controller.h"
#include "ledconstants.h"
#include "edgesetup.h"
#include "paletteregistry.h"    // NEW: Add this include
#include "audiosystem.h"        // NEW: Centralized audio system
#include "models.h"
#include "hdlo_models.h"
#include "colorfunctions.h"
#include "modelsequence.h"
#include "sequences.h"
#include "edgepermutations.h"
#include "namedpermutations.h"

// Global sequence object
modelsequence* mainSequence;

void setup() {
    Serial.begin(115200);
    // Optional: wait briefly for serial monitor (comment out for production)
    // while (!Serial && millis() < 3000);
    
    Serial.println("=== HDLO Controller Starting ===");
    
    // Step 1: Initialize edge data
    Serial.println("Initializing edge data...");
    initedgedata();
    
    // Step 2: Initialize OctoWS2811
    Serial.println("Initializing OctoWS2811...");
    octocontroller.begin();
    teensycontroller = new CTeensy4Controller<GRB, WS2811_800kHz>(&octocontroller);
    FastLED.setBrightness(MAXBRIGHTNESS);
    FastLED.addLeds(teensycontroller, rgbarray, numberofleds);
    
    // Step 3: Initialize the audio system (BEFORE color functions!)
    Serial.println("Initializing audio system...");
    AudioMemory(12);  // Allocate audio memory blocks FIRST
    AudioSystem::initialize();
    
    // Step 4: Initialize the palette registry 
    Serial.println("Initializing palette registry...");
    PaletteRegistry::initialize();
    //PaletteRegistry::printRegistry();  // Optional: see available palettes
    
    // Step 5: Initialize color functions
    Serial.println("Initializing color functions...");
    initializeStatefulColorFunctions();
    

    // Step 6: Initialize edge permutations
    Serial.println("Initializing edge permutations...");
    //EdgePermutation::printRegistry();  // Optional: see available permutations
    

    // Step 7: Initialize models
    Serial.println("Initializing models...");
    initializemodels();
    initializefancymodels();
    colormodel::printRegistry();  // Optional: see available models
    
    // Step 8: Create and initialize sequences (NEW - SIMPLIFIED!)
    Serial.println("Initializing sequences...");
    mainSequence = new modelsequence();
    initializeSequences(mainSequence);
    
    // Step 9: Start the sequence registry
    Serial.println("Starting sequence registry...");
    mainSequence->beginRegistry();
    
    Serial.println("=== Setup Complete ===");
    Serial.println("Current sequence: " + mainSequence->getCurrentRegistryName());
    Serial.println();
}

void loop() {
    // Safety check
    if(!mainSequence) {
        Serial.println("ERROR: mainSequence is NULL!");
        delay(1000);
        return;
    }
    
    // Update audio system (reads FFT data once per frame)
    AudioSystem::update();
    
    // Update frame counter for lazy evaluation
    StatefulColorFunction::beginFrame();
    
    // Update the sequence registry (handles sequence switching)
    unsigned long currentTime = millis();
    if(mainSequence->updateRegistry(currentTime)) {
        Serial.println("Switched to sequence: " + mainSequence->getCurrentRegistryName());
    }
    
    // Update the current sequence
    mainSequence->update(currentTime);
    
    // Apply the colors to the LED array
    for(int ledindex = 0; ledindex < numberofleds; ledindex++) {
        // Get edge and position from strand table
        int edgeindex = strandtable[ledindex][1];
        float position = strandtable[ledindex][0] / (float)positionresolution;
        
        // Get color from the sequence
        CRGB color = mainSequence->getColor(edgeindex, position);
        
        // Apply to LED array
        rgbarray[ledindex] = color;
    }
    
    // Show the LEDs
    FastLED.show();
    
    // Optional: Print active stateful functions or whatever else every 1000 frames
    static int frameCount = 0;
    if(++frameCount % 1000 == 0) {
      //  printActiveStatefulFunctions();
    }
    
    // Optional: Handle serial commands for runtime control -- pretty cool little feature 
    // Claude added on its own.
    handleSerialCommands();
}

// Optional: Add serial commands for runtime control
void handleSerialCommands() {
    if(Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        
        if(command == "next") {
            // Force switch to next sequence
            Serial.println("Forcing switch to next sequence...");
            // You could add a forceNextSequence() method to modelsequence
        }
        else if(command == "palettes") {
            // Show all palettes
            PaletteRegistry::printRegistry();
        }
        else if(command == "models") {
            // Show all models
            colormodel::printRegistry();
        }
        else if(command == "sequences") {
            // Show current sequence info
            Serial.println("Current sequence: " + mainSequence->getCurrentRegistryName());
            Serial.println("Step: " + String(mainSequence->getCurrentStep()) + 
                          " of " + String(mainSequence->getTotalSteps()));
        }
        else if(command.startsWith("cycle")) {
            // Cycle all palettes
            cycleAllPalettes();
            Serial.println("Cycled all palettes");
        }
        else if(command.startsWith("random")) {
            // Randomize all palettes
            randomizeAllPalettes();
            Serial.println("Randomized all palettes");
        }
        else if(command.startsWith("switch ")) {
            // Switch a specific function to a specific palette
            // Format: "switch fire2012 ocean"
            int spacePos = command.indexOf(' ', 7);
            if(spacePos > 0) {
                String funcName = command.substring(7, spacePos);
                String paletteName = command.substring(spacePos + 1);
                switchPalette(funcName, paletteName);
            }
        }
        else if(command == "help") {
            Serial.println("Commands:");
            Serial.println("  next - Force next sequence");
            Serial.println("  palettes - List all palettes");
            Serial.println("  models - List all models");
            Serial.println("  sequences - Show sequence info");
            Serial.println("  cycle - Cycle all palettes");
            Serial.println("  random - Randomize palettes");
            Serial.println("  switch [function] [palette] - Switch specific function palette");
            Serial.println("  audio - Show audio levels");
            Serial.println("  gain [0.0-1.0] - Set microphone gain");
            Serial.println("  mic - Switch to microphone input");
            Serial.println("  play [filename] - Play SD card file");
            Serial.println("  loop [on/off] - Enable/disable looping");
            Serial.println("  stop - Stop playback");
            Serial.println("  pause - Pause playback");
            Serial.println("  resume - Resume playback");
            Serial.println("  source - Show current audio source");
            Serial.println("  help - Show this help");
        }
        else if(command == "audio") {
            // Show audio levels
            AudioSystem::printLevels();
        }
        else if(command.startsWith("gain ")) {
            // Set microphone gain
            float gain = command.substring(5).toFloat();
            AudioSystem::setMicGain(gain);
        }
        else if(command == "mic") {
            AudioSystem::useMicrophone();
            Serial.println("Switched to microphone");
        }
        else if(command.startsWith("play ")) {
            String filename = command.substring(5);
            
            if(AudioSystem::useSDCard(filename.c_str(), 1.0)) {
                Serial.println("Playing: " + filename);
            } else {
                Serial.println("Failed to play: " + filename);
            }
        }
        else if(command.startsWith("rate ")) {
            Serial.println("Error: Playback rate control not supported");
        }
        else if(command.startsWith("loop ")) {
            String state = command.substring(5);
            if(state == "on") {
                AudioSystem::setLooping(true);
                Serial.println("Looping enabled");
            } else {
                AudioSystem::setLooping(false);
                Serial.println("Looping disabled");
            }
        }
        else if(command == "stop") {
            AudioSystem::stopPlayback();
        }
        else if(command == "pause") {
            AudioSystem::pausePlayback();
        }
        else if(command == "resume") {
            AudioSystem::resumePlayback();
        }
        else if(command == "source") {
            Serial.println("Current source: " + AudioSystem::getCurrentSourceType());
            if(AudioSystem::isPlaying()) {
                Serial.println("Status: Playing");
            }
        }
    }
}
