////////////////////////////////////////////
//
//   paletteregistry.cpp
//
// Implementation of palette registry system
// Uses integer IDs from globalids.h
//

#include "paletteregistry.h"
#include "globalids.h"

// Static member definitions
PaletteRegistry::PaletteEntry PaletteRegistry::registry[MAX_PALETTES];
int PaletteRegistry::numRegistered = 0;
bool PaletteRegistry::initialized = false;

namespace {
    
DEFINE_GRADIENT_PALETTE( CustomFire_gp ) {
    0,   0,   0,   0,
   32,  128,  0,   0,
   64,  255,  0,   0,
   96,  255,  128, 0,
   128, 255, 255,  0,
   160, 255, 255,  64,
   192, 255, 255,  128,
   224, 255, 255,  192,
   255, 255, 255,  255
};

DEFINE_GRADIENT_PALETTE( CustomBlueFire_gp ) {
    0,   0,   0,   0,
   32,   0,   0,  64,
   64,   0,   0, 128,
   96,   0,  64, 192,
   128,  0, 128, 255,
   160, 64, 192, 255,
   192, 128,224, 255,
   224, 192,240, 255,
   255, 255,255, 255
};

DEFINE_GRADIENT_PALETTE( CustomGreenFire_gp ) {
    0,   0,   0,   0,
   32,   0,  64,   0,
   64,   0, 128,   0,
   96,  32, 192,   0,
   128, 64, 255,   0,
   160, 128,255,  32,
   192, 192,255,  64,
   224, 224,255, 128,
   255, 255,255, 192
};

DEFINE_GRADIENT_PALETTE( CustomPlasma_gp ) {
    0,    0,   0,   128,
   64,  128,  0,   255,
   128, 255,  0,   128,
   192, 255,  128,  0,
   255, 255,  255,  0
};

DEFINE_GRADIENT_PALETTE( CustomForest_gp ) {
    0,    0,   40,  0,
    40, 0, 80,0,
   50,   30,  120,  20,
   90, 5, 150, 20,
   100, 140,  150,  0,
  120, 80,  180, 60,
  130, 150,  200, 140,
  150, 20,  40,  10,
  160, 120,  90,  30,
  190, 0,  90,  0,
  230, 30, 60, 20,
  255, 40,  90, 10
};

DEFINE_GRADIENT_PALETTE( CustomSunset_gp ) {
    0,   32,   0,   64,
   64,  128,  0,   32,
   128, 255,  32,  0,
   192, 255,  128, 0,
   255, 255,  192, 64
};

DEFINE_GRADIENT_PALETTE( CustomRainbow_gp ) {
    0,   255,  0,   0,
   42,  255,  128, 0,
   84,  255,  255, 0,
   127, 0,    255, 0,
   170, 0,    128, 255,
   212, 128,  0,   255,
   255, 255,  0,   128
};

DEFINE_GRADIENT_PALETTE( CustomLava_gp ) {
    0,   0,   0,   0,
   46,  18,  0,   0,
   96,  113, 0,   0,
   108, 142, 3,   1,
   119, 175, 17,  1,
   146, 213, 44,  2,
   174, 255, 82,  4,
   188, 255, 115, 4,
   202, 255, 156, 4,
   218, 255, 203, 4,
   234, 255, 255, 4,
   244, 255, 255, 71,
   255, 255, 255, 255
};

DEFINE_GRADIENT_PALETTE( CustomIce_gp ) {
    0,   0,   0,   0,
   64,  16,  16,  32,
   96,  32,  32,  64,
   128, 64,  64,  128,
   160, 96,  96,  192,
   192, 128, 160, 255,
   224, 192, 220, 255,
   255, 255, 255, 255
};

DEFINE_GRADIENT_PALETTE( AudioGreen_gp ) {
    0,    0,   32,   0,
   32,    0,   64,   0,
   64,    0,   96,   0,
   96,    0,  128,   0,
  128,    0,  160,   0,
  160,   32,  192,   0,
  192,   64,  224,   0,
  224,  128,  255,  32,
  255,  192,  255,  64
};

DEFINE_GRADIENT_PALETTE( AudioSpectrum_gp ) {
    0,    0,   0,   128,
   64,    0,  128,  64,
  128,    0,  255,   0,
  192,  128,  255,   0,
  255,  255,  255,   0
};

DEFINE_GRADIENT_PALETTE( WhitePalette_gp ) {
    0,   0, 0, 0,
  255,   255, 255, 255
};

DEFINE_GRADIENT_PALETTE( RedPalette_gp ) {
    0,   0, 0, 0,
  255,   255, 0, 0
};

DEFINE_GRADIENT_PALETTE( GreenPalette_gp ) {
    0,   0, 0, 0,
  255,   0, 255, 0
};

DEFINE_GRADIENT_PALETTE( BluePalette_gp ) {
    0,   0, 0, 0,
  255,   0, 0, 255
};

DEFINE_GRADIENT_PALETTE( CyanPalette_gp ) {
    0,   0, 0, 0,
  255,   0, 255, 255
};

DEFINE_GRADIENT_PALETTE( MagentaPalette_gp ) {
    0,   0, 0, 0,
  255,   255, 0, 255
};

DEFINE_GRADIENT_PALETTE( YellowPalette_gp ) {
    0,   0, 0, 0,
  255,   255, 255, 0
};

} // anonymous namespace

void PaletteRegistry::initialize() {
    if(initialized) return;
    
    Serial.println("Initializing Palette Registry...");
    
    // Register palettes using integer IDs
    registerPalette(fire, CustomFire_gp);
    registerPalette(bluefire, CustomBlueFire_gp);
    registerPalette(greenfire, CustomGreenFire_gp);
    registerPalette(plasma_pal, CustomPlasma_gp);
    registerPalette(ocean, OceanColors_p);
    registerPalette(forest, CustomForest_gp);
    registerPalette(sunset, CustomSunset_gp);
    registerPalette(rainbow, CustomRainbow_gp);
    registerPalette(lava, CustomLava_gp);
    registerPalette(ice, CustomIce_gp);
    registerPalette(audiogreen, AudioGreen_gp);
    registerPalette(audiospectrum, AudioSpectrum_gp);
    registerPalette(white, WhitePalette_gp);
    registerPalette(red, RedPalette_gp);
    registerPalette(green, GreenPalette_gp);
    registerPalette(blue, BluePalette_gp);
    registerPalette(cyan, CyanPalette_gp);
    registerPalette(yellow, YellowPalette_gp);
    registerPalette(magenta, MagentaPalette_gp);
    
    // FastLED built-in palettes
    registerPalette(heat, HeatColors_p);
    registerPalette(party, PartyColors_p);
    registerPalette(cloud, CloudColors_p);
    registerPalette(forest_builtin, ForestColors_p);
    registerPalette(rainbow_builtin, RainbowColors_p);
    registerPalette(rainbowstripe, RainbowStripeColors_p);
    
    initialized = true;
    
    Serial.print("Palette Registry initialized with ");
    Serial.print(numRegistered);
    Serial.println(" palettes");
}
