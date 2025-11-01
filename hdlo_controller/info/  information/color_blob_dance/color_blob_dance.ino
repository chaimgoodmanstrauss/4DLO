#include <FastLED.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// ============================================================================
// CONFIGURATION
// ============================================================================
#define LED_PIN     24
#define NUM_LEDS    260
#define BRIGHTNESS  128
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

const int myInput = AUDIO_INPUT_MIC;

// Create the Audio components
AudioInputI2S          audioInput;         // audio shield: mic or line-in
AudioAnalyzeFFT1024    myFFT;
AudioOutputI2S         audioOutput;        // audio shield: headphones & line-out

AudioConnection patchCord1(audioInput, 0, myFFT, 0);
AudioControlSGTL5000 audioShield;

CRGB leds[NUM_LEDS];

// ============================================================================
// COLOR BLOB DANCE STATE
// ============================================================================
struct Blob {
  float position;      // Current position (float for smooth movement)
  float velocity;      // Speed and direction
  float size;          // Blob radius
  uint8_t paletteIndex; // Starting color in palette
  float hueShift;      // How much hue shifts per frame
};

#define NUM_BLOBS 4
Blob blobs[NUM_BLOBS];

// Audio analysis variables
float audioLevel = 0;
float audioPitch = 0;
float audioSmoothed = 0;
const float AUDIO_SMOOTH = 0.7;

// Palette selection
CRGBPalette16 currentPalette = RainbowColors_p;
uint8_t paletteMode = 0;

// Time tracking
unsigned long lastUpdate = 0;
const unsigned int UPDATE_INTERVAL = 20; // ~50 fps
unsigned long lastSerialUpdate = 0;
const unsigned int SERIAL_UPDATE_INTERVAL = 2000; // Every 2 seconds

// ============================================================================
// SETUP
// ============================================================================
void setup() {
  Serial.begin(115200);
  
  // Initialize Audio Shield
  AudioMemory(12);
  audioShield.enable();
  audioShield.inputSelect(myInput);
  audioShield.volume(0.5);
  audioShield.micGain(36);  // Set mic gain (0-63, default is 0) - increase if audio is too quiet
  
  // Initialize FastLED
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)
    .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  
  // Initialize blobs with random properties
  randomSeed(analogRead(A5));
  for (int b = 0; b < NUM_BLOBS; b++) {
    blobs[b].position = random(0, NUM_LEDS * 256) / 256.0;
    blobs[b].velocity = random(-200, 200) / 100.0;
    blobs[b].size = random(3, 8);
    blobs[b].paletteIndex = random(0, 256);
    blobs[b].hueShift = random(-50, 50) / 100.0;
  }
  
  // Set initial palette
  setPalette(0);
  
  Serial.println("Color Blob Dance Initialized!");
  Serial.println("Audio Shield with FFT ready");
  Serial.println("Mic gain set to 36 (adjust if needed)");
  Serial.println("Watch the serial output for audio levels...");
}

// ============================================================================
// MAIN LOOP
// ============================================================================
void loop() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastUpdate >= UPDATE_INTERVAL) {
    lastUpdate = currentTime;
    
    // Read and process audio
    processAudio();
    
    // Update blob physics
    updateBlobs();
    
    // Render each pixel
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = colorBlobDance(i);
    }
    
    FastLED.show();
  }
  
  // Periodic status updates to serial monitor
  if (currentTime - lastSerialUpdate >= SERIAL_UPDATE_INTERVAL) {
    lastSerialUpdate = currentTime;
    
    Serial.println("=== Color Blob Dance Status ===");
    Serial.print("Audio Level: ");
    Serial.print(audioLevel * 100, 1);
    Serial.println("%");
    
    Serial.print("Pitch: ");
    Serial.print(audioPitch * 100, 1);
    Serial.println("%");
    
    Serial.print("Active Blobs: ");
    Serial.println(NUM_BLOBS);
    
    for (int b = 0; b < NUM_BLOBS; b++) {
      Serial.print("  Blob ");
      Serial.print(b);
      Serial.print(" - Pos: ");
      Serial.print(blobs[b].position, 1);
      Serial.print(" Vel: ");
      Serial.print(blobs[b].velocity, 2);
      Serial.print(" Size: ");
      Serial.println(blobs[b].size, 1);
    }
    
    Serial.print("FPS: ~");
    Serial.println(1000 / UPDATE_INTERVAL);
    Serial.println();
  }
  
  // Handle serial commands for palette changes
  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd >= '0' && cmd <= '9') {
      setPalette(cmd - '0');
    }
  }
}

// ============================================================================
// CORE FUNCTION: COLOR BLOB DANCE
// ============================================================================
CRGB colorBlobDance(int pixelIndex) {
  CRGB color = CRGB::Black;
  
  // Accumulate contribution from each blob
  for (int b = 0; b < NUM_BLOBS; b++) {
    float distance = abs(pixelIndex - blobs[b].position);
    
    // Handle wraparound distance (for circular strips)
    float wrapDistance = NUM_LEDS - distance;
    if (wrapDistance < distance) {
      distance = wrapDistance;
    }
    
    // Calculate influence based on distance and blob size
    // Audio pitch affects the falloff curve much more dramatically
    float pitchFactor = 1.0 + audioPitch * 2.0;  // Increased from 0.5 to 2.0
    float influence = max(0.0, 1.0 - (distance / (blobs[b].size * pitchFactor)));
    
    if (influence > 0) {
      // Sample from palette with audio-modulated offset
      uint8_t colorIndex = blobs[b].paletteIndex + 
                          (uint8_t)(distance * 8.0 / blobs[b].size);
      
      // Get color from current palette
      CRGB blobColor = ColorFromPalette(currentPalette, colorIndex, 255, LINEARBLEND);
      
      // Apply influence with audio level affecting intensity DRAMATICALLY
      // When quiet: dim (minimum 0.2), when loud: full brightness
      float baseIntensity = 0.2 + audioLevel * 0.8;  // Changed to vary from 0.2 to 1.0
      float intensity = influence * baseIntensity;
      
      color.r = qadd8(color.r, blobColor.r * intensity);
      color.g = qadd8(color.g, blobColor.g * intensity);
      color.b = qadd8(color.b, blobColor.b * intensity);
    }
  }
  
  return color;
}

// ============================================================================
// BLOB PHYSICS UPDATE
// ============================================================================
void updateBlobs() {
  for (int b = 0; b < NUM_BLOBS; b++) {
    // Audio pitch affects velocity MUCH more dramatically
    float pitchEffect = 1.0 + (audioPitch - 0.5) * 2.0;  // Increased from 0.3 to 2.0
    
    // Audio level affects size pulsing dramatically
    float sizeVariation = audioLevel * 5.0;  // Increased from 2.0 to 5.0
    float currentSize = blobs[b].size + sizeVariation;
    
    // Update position - speed increases significantly with audio
    float speedMultiplier = 1.0 + audioLevel * 3.0;  // Speed boost from audio
    blobs[b].position += blobs[b].velocity * pitchEffect * speedMultiplier;
    
    // Wrap around
    if (blobs[b].position < 0) {
      blobs[b].position += NUM_LEDS;
    } else if (blobs[b].position >= NUM_LEDS) {
      blobs[b].position -= NUM_LEDS;
    }
    
    // Update palette index (color rotation) - much faster with audio
    blobs[b].paletteIndex += blobs[b].hueShift * (1.0 + audioLevel * 5.0);  // Increased from 1.0 to 5.0
    
    // Store the audio-modulated size for rendering
    blobs[b].size = blobs[b].size * 0.95 + currentSize * 0.05;  // Smooth size changes
  }
}

// ============================================================================
// AUDIO PROCESSING
// ============================================================================
void processAudio() {
  if (myFFT.available()) {
    // Calculate overall audio level from multiple frequency bins
    float level = 0;
    for (int i = 0; i < 40; i++) {  // First 40 bins cover most musical content
      level += myFFT.read(i);
    }
    level = level / 40.0;  // Average
    
    // Amplify and smooth the audio level - make it more sensitive
    audioSmoothed = audioSmoothed * AUDIO_SMOOTH + level * (1.0 - AUDIO_SMOOTH);
    audioLevel = constrain(audioSmoothed * 10.0, 0.0, 1.0);  // Increased from 2.0 to 10.0 for more sensitivity
    
    // Pitch estimation based on FFT bin energy distribution
    // Find the bin with highest energy in musical range
    float maxBinLevel = 0;
    int maxBin = 0;
    
    // Search bins 2-100 (roughly 86Hz to 4.3kHz - musical range)
    for (int i = 2; i < 100; i++) {
      float binLevel = myFFT.read(i);
      if (binLevel > maxBinLevel) {
        maxBinLevel = binLevel;
        maxBin = i;
      }
    }
    
    // Convert bin number to pitch estimate (0-1 normalized)
    // Lower bins = lower pitch, higher bins = higher pitch
    audioPitch = constrain(maxBin / 100.0, 0.0, 1.0);
    
    // Debug output - enable to see raw values
    static unsigned long lastDebug = 0;
    if (millis() - lastDebug > 500) {
      lastDebug = millis();
      Serial.print("Raw level: "); Serial.print(level, 4);
      Serial.print(" | Scaled level: "); Serial.print(audioLevel, 3);
      Serial.print(" | Max bin: "); Serial.print(maxBin);
      Serial.print(" | Pitch: "); Serial.println(audioPitch, 3);
    }
  }
}

// ============================================================================
// PALETTE SELECTION
// ============================================================================
void setPalette(uint8_t mode) {
  paletteMode = mode % 10;
  
  switch(paletteMode) {
    case 0:
      currentPalette = RainbowColors_p;
      Serial.println("Palette: Rainbow");
      break;
    case 1:
      currentPalette = RainbowStripeColors_p;
      Serial.println("Palette: Rainbow Stripe");
      break;
    case 2:
      currentPalette = CloudColors_p;
      Serial.println("Palette: Cloud");
      break;
    case 3:
      currentPalette = PartyColors_p;
      Serial.println("Palette: Party");
      break;
    case 4:
      currentPalette = OceanColors_p;
      Serial.println("Palette: Ocean");
      break;
    case 5:
      currentPalette = LavaColors_p;
      Serial.println("Palette: Lava");
      break;
    case 6:
      currentPalette = ForestColors_p;
      Serial.println("Palette: Forest");
      break;
    case 7:
      currentPalette = HeatColors_p;
      Serial.println("Palette: Heat");
      break;
    case 8:
      // Custom palette: Purple and teal
      currentPalette = CRGBPalette16(
        CRGB::Purple, CRGB::MediumPurple, CRGB::Teal, CRGB::Cyan,
        CRGB::Purple, CRGB::MediumPurple, CRGB::Teal, CRGB::Cyan,
        CRGB::Purple, CRGB::MediumPurple, CRGB::Teal, CRGB::Cyan,
        CRGB::Purple, CRGB::MediumPurple, CRGB::Teal, CRGB::Cyan
      );
      Serial.println("Palette: Purple Teal");
      break;
    case 9:
      // Custom palette: Fire and ice
      currentPalette = CRGBPalette16(
        CRGB::Blue, CRGB::Cyan, CRGB::White, CRGB::Orange,
        CRGB::Red, CRGB::Orange, CRGB::Yellow, CRGB::White,
        CRGB::Blue, CRGB::Cyan, CRGB::White, CRGB::Orange,
        CRGB::Red, CRGB::Orange, CRGB::Yellow, CRGB::White
      );
      Serial.println("Palette: Fire & Ice");
      break;
  }
}
