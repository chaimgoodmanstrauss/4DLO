////////////////////////////////////////////
//
//   paletteregistry.h
//
// Centralized palette registry system
// Uses integer IDs for efficient lookup
//

#ifndef PALETTEREGISTRY_H
#define PALETTEREGISTRY_H

#include <FastLED.h>
#include <Arduino.h>
#include "globalids.h"

const int MAX_PALETTES = 50;

/////////////////////////////////////////
// PALETTE REGISTRY CLASS
//
class PaletteRegistry {
private:
    struct PaletteEntry {
        int id;
        CRGBPalette16 palette;
        bool registered;
        
        PaletteEntry() : id(0), registered(false) {}
        PaletteEntry(int i, CRGBPalette16 p) : id(i), palette(p), registered(true) {}
    };
    
    static PaletteEntry registry[MAX_PALETTES];
    static int numRegistered;
    static bool initialized;
    
public:
    static void initialize();
    
    static bool registerPalette(int id, CRGBPalette16 palette) {
        if(numRegistered >= MAX_PALETTES) {
            Serial.println("Error: Palette registry full");
            return false;
        }
        
        for(int i = 0; i < numRegistered; i++) {
            if(registry[i].id == id) {
                Serial.print("Warning: Palette ID ");
                Serial.print(id);
                Serial.println(" already registered");
                return false;
            }
        }
        
        registry[numRegistered] = PaletteEntry(id, palette);
        numRegistered++;
        return true;
    }
    
    static CRGBPalette16* findById(int id) {
        for(int i = 0; i < numRegistered; i++) {
            if(registry[i].id == id) {
                return &registry[i].palette;
            }
        }
        return nullptr;
    }
    
    static CRGBPalette16 getPalette(int id) {
        CRGBPalette16* pal = findById(id);
        if(pal) {
            return *pal;
        }
        return RainbowColors_p;
    }
    
    static CRGBPalette16* getByIndex(int index) {
        if(index >= 0 && index < numRegistered) {
            return &registry[index].palette;
        }
        return nullptr;
    }
    
    static int getIdByIndex(int index) {
        if(index >= 0 && index < numRegistered) {
            return registry[index].id;
        }
        return 0;
    }
    
    static int getCount() { return numRegistered; }
    
    static void printRegistry() {
        Serial.println("=== Palette Registry ===");
        Serial.print("Total palettes: ");
        Serial.println(numRegistered);
        for(int i = 0; i < numRegistered; i++) {
            Serial.print("  [");
            Serial.print(i);
            Serial.print("] ID=");
            Serial.println(registry[i].id);
        }
        Serial.println("========================");
    }
    
    static CRGBPalette16* getRandom() {
        if(numRegistered == 0) return nullptr;
        int index = random(numRegistered);
        return &registry[index].palette;
    }
    
    static CRGBPalette16* getNext(int& currentIndex) {
        if(numRegistered == 0) return nullptr;
        currentIndex = (currentIndex + 1) % numRegistered;
        return &registry[currentIndex].palette;
    }
};

#endif // PALETTEREGISTRY_H
