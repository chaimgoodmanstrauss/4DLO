////////////////////////////////////////////
//
//   paletteregistry.h
//
// Centralized palette registry system
// Automatically manages and provides access to all palettes
//

#ifndef PALETTEREGISTRY_H
#define PALETTEREGISTRY_H

#include <FastLED.h>
#include <Arduino.h>

// Maximum number of registered palettes
const int MAX_PALETTES = 50;

/////////////////////////////////////////
// PALETTE REGISTRY CLASS
//

class PaletteRegistry {
private:
    struct PaletteEntry {
        String name;
        CRGBPalette16 palette;
        bool registered;
        
        PaletteEntry() : name(""), registered(false) {}
        PaletteEntry(String n, CRGBPalette16 p) : name(n), palette(p), registered(true) {}
    };
    
    static PaletteEntry registry[MAX_PALETTES];
    static int numRegistered;
    static bool initialized;
    
public:
    // Initialize the registry with default palettes
    static void initialize();
    
    // Register a new palette
    static bool registerPalette(String name, CRGBPalette16 palette) {
        if(numRegistered >= MAX_PALETTES) {
            Serial.println("Error: Palette registry full");
            return false;
        }
        
        // Check for duplicate names
        for(int i = 0; i < numRegistered; i++) {
            if(registry[i].name.equalsIgnoreCase(name)) {
                Serial.println("Warning: Palette '" + name + "' already registered");
                return false;
            }
        }
        
        registry[numRegistered] = PaletteEntry(name, palette);
        numRegistered++;
        
        Serial.println("Registered palette: " + name);
        return true;
    }
    
    // Find palette by name
    static CRGBPalette16* findByName(String name) {
        for(int i = 0; i < numRegistered; i++) {
            if(registry[i].name.equalsIgnoreCase(name)) {
                return &registry[i].palette;
            }
        }
        return nullptr;
    }
    
    // Get palette by index
    static CRGBPalette16* getByIndex(int index) {
        if(index >= 0 && index < numRegistered) {
            return &registry[index].palette;
        }
        return nullptr;
    }
    
    // Get palette name by index
    static String getNameByIndex(int index) {
        if(index >= 0 && index < numRegistered) {
            return registry[index].name;
        }
        return "";
    }
    
    // Get number of registered palettes
    static int getCount() { return numRegistered; }
    
    // Print all registered palettes
    static void printRegistry() {
        Serial.println("=== Palette Registry ===");
        Serial.println("Total palettes: " + String(numRegistered));
        for(int i = 0; i < numRegistered; i++) {
            Serial.println("  [" + String(i) + "] " + registry[i].name);
        }
        Serial.println("========================");
    }
    
    // Get random palette
    static CRGBPalette16* getRandom() {
        if(numRegistered == 0) return nullptr;
        int index = random(numRegistered);
        return &registry[index].palette;
    }
    
    // Cycle through palettes
    static CRGBPalette16* getNext(int& currentIndex) {
        if(numRegistered == 0) return nullptr;
        currentIndex = (currentIndex + 1) % numRegistered;
        return &registry[currentIndex].palette;
    }
};

#endif // PALETTEREGISTRY_H
