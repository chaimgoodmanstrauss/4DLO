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


// DEBUG CODE TO ADD TO hdlo_controller.ino
// Add this function before setup():

void debugColors() {
    Serial.println("=== COLOR DEBUG TEST ===");
    
    // Test 1: Check palette retrieval
    Serial.println("\n1. Testing palette retrieval:");
    CRGBPalette16* firePalette = PaletteRegistry::findByName("fire");
    CRGBPalette16* oceanPalette = PaletteRegistry::findByName("ocean");
    CRGBPalette16* plasmaPalette = PaletteRegistry::findByName("plasma");
    
    Serial.print("Fire palette found: ");
    Serial.println(firePalette != nullptr ? "YES" : "NO");
    Serial.print("Ocean palette found: ");
    Serial.println(oceanPalette != nullptr ? "YES" : "NO");
    Serial.print("Plasma palette found: ");
    Serial.println(plasmaPalette != nullptr ? "YES" : "NO");
    
    // Test 2: Check actual colors from palettes
    if(firePalette != nullptr) {
        Serial.println("\n2. Fire palette colors (at indices 0, 64, 128, 192, 255):");
        for(int i = 0; i <= 255; i += 64) {
            CRGB color = ColorFromPalette(*firePalette, i);
            Serial.print("Index ");
            Serial.print(i);
            Serial.print(": R=");
            Serial.print(color.r);
            Serial.print(" G=");
            Serial.print(color.g);
            Serial.print(" B=");
            Serial.println(color.b);
        }
    }
    
    // Test 3: Test plasma function directly
    Serial.println("\n3. Testing plasma function at position 0.5:");
    PlasmaColorFunction testPlasma("test", 0.02, 0.03, 0.01, 4.0, 3.0, 5.0, "fire");
    testPlasma.reset();
    testPlasma.updateState();
    CRGB plasmaColor = testPlasma.getColor(0.5);
    Serial.print("Plasma with fire palette: R=");
    Serial.print(plasmaColor.r);
    Serial.print(" G=");
    Serial.print(plasmaColor.g);
    Serial.print(" B=");
    Serial.println(plasmaColor.b);
    
    // Test 4: Direct color test
    Serial.println("\n4. Setting LEDs to pure colors:");
    Serial.println("Setting LED 0 to pure RED (255,0,0)");
    Serial.println("Setting LED 1 to pure GREEN (0,255,0)");
    Serial.println("Setting LED 2 to pure BLUE (0,0,255)");
    rgbarray[0] = CRGB(255, 0, 0);  // Pure red
    rgbarray[1] = CRGB(0, 255, 0);  // Pure green
    rgbarray[2] = CRGB(0, 0, 255);  // Pure blue
    FastLED.show();
    Serial.println("Check what colors actually appear on LEDs 0, 1, 2");
    
    // Test 5: Check what's in the strand table
    Serial.println("\n5. First 10 entries in strand table:");
    for(int i = 0; i < 10 && i < numberofleds; i++) {
        Serial.print("LED ");
        Serial.print(i);
        Serial.print(": position=");
        Serial.print(strandtable[i][0]);
        Serial.print(" edge=");
        Serial.println(strandtable[i][1]);
    }
    
    Serial.println("\n=== END COLOR DEBUG ===\n");
}

// Then in setup(), right after initializeSequences(mainSequence), add:
// debugColors();
// delay(5000);  // Give time to see the test colors

// Also add this simpler test you can call from serial commands:
void testPureColors() {
    Serial.println("Testing pure colors on first 3 LEDs:");
    
    // Test pure colors
    rgbarray[0] = CRGB::Red;
    rgbarray[1] = CRGB::Green;
    rgbarray[2] = CRGB::Blue;
    FastLED.show();
    Serial.println("LED 0 should be RED");
    Serial.println("LED 1 should be GREEN");
    Serial.println("LED 2 should be BLUE");
    delay(3000);
    
    // Test other combinations
    rgbarray[0] = CRGB(255, 255, 0);  // Yellow
    rgbarray[1] = CRGB(255, 0, 255);  // Magenta
    rgbarray[2] = CRGB(0, 255, 255);  // Cyan
    FastLED.show();
    Serial.println("LED 0 should be YELLOW");
    Serial.println("LED 1 should be MAGENTA");
    Serial.println("LED 2 should be CYAN");
}

// In handleSerialCommands(), add:
// else if(command == "testcolors") {
//     testPureColors();
// }

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
    initializefancymodels();// this also initializes the color functions for the models
    colormodel::printRegistry();  // Optional: see available models
    
    // Step 8: Create and initialize sequences (NEW - SIMPLIFIED!)
    Serial.println("Initializing sequences...");
    mainSequence = new modelsequence();
    initializeSequences(mainSequence);
    

  debugColors();
  delay(5000);



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
