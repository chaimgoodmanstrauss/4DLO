////////////////////////////////////////////
//
//   paletteregistry.cpp
//
// Implementation of palette registry system
// Defines custom palettes and registers them
//

#include "paletteregistry.h"

// Static member definitions
PaletteRegistry::PaletteEntry PaletteRegistry::registry[MAX_PALETTES];
int PaletteRegistry::numRegistered = 0;
bool PaletteRegistry::initialized = false;

// Define our custom gradient palettes locally (not exported)
namespace {
    
DEFINE_GRADIENT_PALETTE( CustomFire_gp ) {
    0,   0,   0,   0,   // Black
   32,  128,  0,   0,   // Dark red
   64,  255,  0,   0,   // Red
   96,  255,  128, 0,   // Orange
   128, 255, 255,  0,   // Yellow
   160, 255, 255,  64,  // Light yellow
   192, 255, 255,  128, // Lighter yellow
   224, 255, 255,  192, // Very light yellow
   255, 255, 255,  255  // White
};

DEFINE_GRADIENT_PALETTE( CustomBlueFire_gp ) {
    0,   0,   0,   0,   // Black
   32,   0,   0,  64,  // Very dark blue
   64,   0,   0, 128,  // Dark blue
   96,   0,  64, 192,  // Medium blue
   128,  0, 128, 255,  // Blue
   160, 64, 192, 255,  // Light blue
   192, 128,224, 255,  // Lighter blue
   224, 192,240, 255,  // Very light blue
   255, 255,255, 255   // White
};

DEFINE_GRADIENT_PALETTE( CustomGreenFire_gp ) {
    0,   0,   0,   0,   // Black
   32,   0,  64,   0,  // Very dark green
   64,   0, 128,   0,  // Dark green
   96,  32, 192,   0,  // Medium green
   128, 64, 255,   0,  // Green
   160, 128,255,  32,  // Light green
   192, 192,255,  64,  // Yellow-green
   224, 224,255, 128,  // Light yellow-green
   255, 255,255, 192   // Very light yellow
};

DEFINE_GRADIENT_PALETTE( CustomPlasma_gp ) {
    0,    0,   0,   128, // Dark blue
   64,  128,  0,   255, // Purple
   128, 255,  0,   128, // Magenta
   192, 255,  128,  0,  // Orange
   255, 255,  255,  0   // Yellow
};

DEFINE_GRADIENT_PALETTE( CustomOcean_gp ) {
    0,    0,   0,   32,  // Very dark blue
   64,   0,   32,  64,  // Dark blue
   128,  0,   64,  128, // Medium blue
   192,  32,  128, 192, // Light blue
   255,  128, 255, 255  // Cyan
};

DEFINE_GRADIENT_PALETTE( CustomForest_gp ) {
    0,    0,   32,  0,   // Very dark green
   64,   0,   64,  0,   // Dark green
   128,  32,  96,  0,   // Medium green
   192,  64,  128, 32,  // Light green
   255,  128, 192, 64   // Yellow-green
};

DEFINE_GRADIENT_PALETTE( CustomSunset_gp ) {
    0,   32,   0,   64,  // Deep purple
   64,  128,  0,   32,  // Dark red
   128, 255,  32,  0,   // Orange
   192, 255,  128, 0,   // Yellow-orange
   255, 255,  192, 64   // Light yellow
};

DEFINE_GRADIENT_PALETTE( CustomRainbow_gp ) {
    0,   255,  0,   0,   // Red
   42,  255,  128, 0,   // Orange
   84,  255,  255, 0,   // Yellow
   127, 0,    255, 0,   // Green
   170, 0,    128, 255, // Blue
   212, 128,  0,   255, // Purple
   255, 255,  0,   128  // Pink
};

DEFINE_GRADIENT_PALETTE( CustomLava_gp ) {
    0,   0,   0,   0,    // Black
   46,  18,  0,   0,    // Very dark red
   96,  113, 0,   0,    // Dark red
   108, 142, 3,   1,    // Dark red-orange
   119, 175, 17,  1,    // Orange
   146, 213, 44,  2,    // Bright orange
   174, 255, 82,  4,    // Yellow-orange
   188, 255, 115, 4,    // Yellow
   202, 255, 156, 4,    // Light yellow
   218, 255, 203, 4,    // Very light yellow
   234, 255, 255, 4,    // Almost white
   244, 255, 255, 71,   // White-yellow
   255, 255, 255, 255   // White
};

DEFINE_GRADIENT_PALETTE( CustomIce_gp ) {
    0,   0,   0,   0,    // Black
   64,  16,  16,  32,   // Very dark blue
   96,  32,  32,  64,   // Dark blue
   128, 64,  64,  128,  // Medium blue
   160, 96,  96,  192,  // Blue
   192, 128, 160, 255,  // Light blue
   224, 192, 220, 255,  // Very light blue
   255, 255, 255, 255   // White
};

} // anonymous namespace

// Initialize the registry with default palettes
void PaletteRegistry::initialize() {
    if(initialized) return;
    
    Serial.println("Initializing Palette Registry...");
    
    // Register our custom palettes
    registerPalette("fire", CustomFire_gp);
    registerPalette("bluefire", CustomBlueFire_gp);
    registerPalette("greenfire", CustomGreenFire_gp);
    registerPalette("plasma", CustomPlasma_gp);
    registerPalette("ocean", CustomOcean_gp);
    registerPalette("forest", CustomForest_gp);
    registerPalette("sunset", CustomSunset_gp);
    registerPalette("rainbow", CustomRainbow_gp);
    registerPalette("lava", CustomLava_gp);
    registerPalette("ice", CustomIce_gp);
    
    // Also register FastLED built-in palettes
    registerPalette("heat", HeatColors_p);
    registerPalette("party", PartyColors_p);
    registerPalette("cloud", CloudColors_p);
    registerPalette("ocean_builtin", OceanColors_p);
    registerPalette("forest_builtin", ForestColors_p);
    registerPalette("rainbow_builtin", RainbowColors_p);
    registerPalette("rainbowstripe", RainbowStripeColors_p);
    
    initialized = true;
    
    Serial.println("Palette Registry initialized with " + String(numRegistered) + " palettes");
}
