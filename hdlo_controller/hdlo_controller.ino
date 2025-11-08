////////////////////////////////////
//
//   hdlo_controller.ino
//
// Main controller file for HDLO LED system
// Updated for new OOP architecture with CRTP auto-registration
//

#include <OctoWS2811.h>
#include <FastLED.h>

#include "teensy4controller.h"
#include "ledconstants.h"
#include "edgesetup.h"
#include "audiosystem.h"
#include "paletteregistry.h"
#include "models.h"
#include "colorfunctions.h"
#include "modelsequence.h"
#include "hdlo_models.h"
#include "edgepermutations.h"
#include "sequences.h"

// Sequence
extern modelsequence mainSequence;

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n=== HDLO Controller Starting ===");
    
    // Step 1: Initialize OctoWS2811
    Serial.println("Initializing OctoWS2811...");
    octocontroller.begin();
    
    // Step 2: Initialize FastLED
    Serial.println("Initializing FastLED...");
    teensycontroller = new CTeensy4Controller<GRB, WS2811_800kHz>(&octocontroller);
    FastLED.addLeds(teensycontroller, rgbarray, numberofleds);
    FastLED.setBrightness(MAXBRIGHTNESS);
    FastLED.clear();
    FastLED.show();
    
    // Step 3: Initialize audio system
    Serial.println("Initializing audio system...");
    AudioSystem::initialize();
    
    // Step 4: Initialize palette registry
    Serial.println("Initializing palette registry...");
    PaletteRegistry::initialize();
    
    // Step 5: Construct strand table
    Serial.println("Constructing strand table...");
    initedgedata();
    
    // Step 6: Color functions auto-register via CRTP
    Serial.println("Color functions registered via CRTP");
    ColorFunctionFactory::getInstance().listFunctions();
    
    // Step 7: Initialize edge permutations
    Serial.println("Initializing edge permutations...");
    EdgePermutation::printRegistry();
    
    // Step 8: Initialize models
    Serial.println("Initializing models...");
    initializemodels();
    colormodel::printRegistry();
    
    // Step 9: Create fancy model variations
    Serial.println("Creating fancy models...");
    initializefancymodels();
    
    // Step 10: Initialize sequences
    Serial.println("Initializing sequences...");
    initializeSequences();
    
    // Step 11: Startup light show
    Serial.println("Startup light show...");
    Serial.print("Number of LEDs: ");
    Serial.println(numberofleds);
    
    // Test 1: All white
    for(int i = 0; i < numberofleds; i++) {
        rgbarray[i] = CRGB(50, 50, 50);
    }
    FastLED.show();
    delay(1000);
    
    // Test 2: Rainbow
    for(int i = 0; i < numberofleds; i++) {
        rgbarray[i] = CHSV(i * 255 / numberofleds, 255, 100);
    }
    FastLED.show();
    delay(1000);
    
    // Clear
    FastLED.clear();
    FastLED.show();
    delay(100);
    
    Serial.println("=== Setup Complete ===\n");
    Serial.println("Commands: 's' = status, 'n' = next step, 'm' = model info");
}

void loop() {
    unsigned long currentTime = millis();
    
    // Update audio system
    AudioSystem::update();
    
    // Update sequence (handles transitions and timing)
    mainSequence.update();
    
    // Increment global frame counter
    IColorFunction::incrementGlobalFrame();
    
    // Render LEDs using strand table
    colormodel* currentModel = mainSequence.getCurrentModel();
    if(currentModel) {
        for(int i = 0; i < numberofleds; i++) {
            int edgeindex = strandtable[i][1];
            int positionInt = strandtable[i][0];
            
            if(edgeindex == 0 && positionInt == 0) {
                rgbarray[i] = CRGB::Black;
                continue;
            }
            
            float position = positionInt / 10000.0;
            rgbarray[i] = currentModel->getcolorfunction(edgeindex, position);
        }
    } else {
        // Fallback: simple rainbow if no model
        static uint8_t hue = 0;
        for(int i = 0; i < numberofleds; i++) {
            rgbarray[i] = CHSV(hue + i, 255, 50);
        }
        hue++;
    }
    
    // Show LEDs
    FastLED.show();
    
    // Handle serial commands
    handleSerialCommands();
    
    // Optional: Print status every 10 seconds
    static unsigned long lastStatusTime = 0;
    if(currentTime - lastStatusTime >= 10000) {
        printStatus();
        lastStatusTime = currentTime;
    }
}

void handleSerialCommands() {
    if(Serial.available() > 0) {
        char cmd = Serial.read();
        
        switch(cmd) {
            case 's':
                printStatus();
                break;
                
            case 'n':
                Serial.println("Skipping to next step...");
                // Force step advance by resetting step start time
                mainSequence.reset();
                break;
                
            case 'm':
                colormodel::printRegistry();
                break;
                
            case 'f':
                ColorFunctionFactory::getInstance().listFunctions();
                break;
                
            case 'p':
                PaletteRegistry::printRegistry();
                break;
                
            case 'e':
                EdgePermutation::printRegistry();
                break;
                
            case 'q':
                mainSequence.printSequenceInfo();
                break;
                
            case 'h':
                printHelp();
                break;
                
            case '\n':
            case '\r':
                // Ignore newlines
                break;
                
            default:
                Serial.print("Unknown command: ");
                Serial.println(cmd);
                Serial.println("Type 'h' for help");
                break;
        }
    }
}

void printStatus() {
    Serial.println("\n=== Status ===");
    Serial.print("Sequence: ");
    Serial.println(mainSequence.getCurrentRegistryName());
    Serial.print("Step: ");
    Serial.print(mainSequence.getCurrentStep());
    Serial.print(" Progress: ");
    Serial.print(mainSequence.getProgress() * 100, 1);
    Serial.println("%");
    
    colormodel* model = mainSequence.getCurrentModel();
    if(model) {
        Serial.print("Model: ");
        Serial.println(model->getModelName());
    }
    
    Serial.print("Audio level: ");
    Serial.println(AudioSystem::getLevel());
    
    Serial.print("Frame: ");
    Serial.println(IColorFunction::getGlobalFrame());
    Serial.println("==============\n");
}

void printHelp() {
    Serial.println("\n=== Commands ===");
    Serial.println("s - Print status");
    Serial.println("n - Next step");
    Serial.println("m - List models");
    Serial.println("f - List color functions");
    Serial.println("p - List palettes");
    Serial.println("e - List edge permutations");
    Serial.println("q - Sequence info");
    Serial.println("h - This help");
    Serial.println("================\n");
}
