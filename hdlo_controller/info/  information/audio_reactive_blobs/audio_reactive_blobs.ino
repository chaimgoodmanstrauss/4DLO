// Audio-Reactive LED Blobs for Teensy 4.1 + Audio Shield
// FastLED on pin 24 with 260 LEDs

#include <Audio.h>
#include <FastLED.h>

// LED Configuration
#define LED_PIN 24
#define NUM_LEDS 260
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

// Audio Configuration
const int myInput = AUDIO_INPUT_MIC;

AudioInputI2S          audioInput;         // audio shield: mic or line-in
AudioAnalyzeFFT1024    fft;
AudioOutputI2S         audioOutput;        // audio shield: headphones & line-out

AudioConnection patchCord1(audioInput, 0, fft, 0);
AudioControlSGTL5000 audioShield;

// Palette - using a colorful gradient
DEFINE_GRADIENT_PALETTE( blob_palette ) {
    0,   255, 0,   255,   // Purple
   64,   0,   0,   255,   // Blue
  128,   0,   255, 255,   // Cyan
  192,   255, 255, 0,     // Yellow
  255,   255, 0,   128    // Pink
};
CRGBPalette16 currentPalette = blob_palette;

// Blob structure
struct Blob {
  bool active;
  float position;        // Float position along the strip
  float velocity;        // Speed and direction (-1 to 1)
  float size;            // Blob radius
  float wobble;          // Wobble intensity
  float wobblePhase;     // Current wobble animation phase
  uint8_t colorIndex;    // Position in palette
  uint8_t brightness;    // Blob brightness
  unsigned long birthTime;
  float wobbleFrequency; // How fast it wobbles
};

#define MAX_BLOBS 8
Blob blobs[MAX_BLOBS];

// Audio detection
#define FFT_BINS 512
#define NOISE_THRESHOLD 0.001
#define TRIGGER_THRESHOLD 0.003  // Even lower threshold
unsigned long lastTriggerTime = 0;
#define MIN_TRIGGER_INTERVAL 100  // Reduced to 100ms for more responsive triggering

void setup() {
  Serial.begin(115200);
  
  // Initialize FastLED
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(200);
  FastLED.clear();
  FastLED.show();
  
  // Initialize Audio
  AudioMemory(12);
  audioShield.enable();
  audioShield.inputSelect(myInput);
  audioShield.volume(0.5);
  audioShield.micGain(63);  // Maximum gain for better sensitivity
  
  // Initialize blobs
  for (int i = 0; i < MAX_BLOBS; i++) {
    blobs[i].active = false;
  }
  
  Serial.println("Audio-Reactive Blobs Ready!");
}

void loop() {
  static unsigned long lastDebug = 0;
  
  // Check for available FFT data
  if (fft.available()) {
    // Analyze audio
    float totalEnergy = 0;
    float peakFreq = 0;
    float peakMag = 0;
    
    // Scan through frequency bins - skip the very lowest bins which are just noise
    for (int i = 3; i < 150; i++) {  // Start at bin 3, scan more bins
      float mag = fft.read(i);
      totalEnergy += mag;
      
      if (mag > peakMag) {
        peakMag = mag;
        peakFreq = i;
      }
    }
    
    totalEnergy /= 147.0;
    
    // Debug output every second
    if (millis() - lastDebug > 1000) {
      Serial.printf("Energy: %.4f, Peak: %.4f @ freq %.0f | ", 
                    totalEnergy, peakMag, peakFreq);
      
      // Show top 5 frequency bins
      Serial.print("Top bins: ");
      for (int i = 1; i < 20; i++) {
        float mag = fft.read(i);
        if (mag > 0.1) {
          Serial.printf("%d:%.2f ", i, mag);
        }
      }
      
      Serial.print("| Active blobs: ");
      int activeCount = 0;
      for (int i = 0; i < MAX_BLOBS; i++) {
        if (blobs[i].active) activeCount++;
      }
      Serial.println(activeCount);
      lastDebug = millis();
    }
    
    // Trigger new blob on sound event
    if (totalEnergy > TRIGGER_THRESHOLD && 
        (millis() - lastTriggerTime) > MIN_TRIGGER_INTERVAL) {
      createBlob(peakFreq, peakMag, totalEnergy);
      lastTriggerTime = millis();
    }
    
    // Update existing blobs with audio wobble
    updateBlobWobble(totalEnergy);
  }
  
  // Update blob positions and states
  updateBlobs();
  
  // Check for blob interactions
  checkBlobInteractions();
  
  // Render blobs to LED strip
  renderBlobs();
  
  // Display
  FastLED.show();
  
  delay(10);
}

void createBlob(float freqBin, float magnitude, float energy) {
  // Find inactive blob slot
  for (int i = 0; i < MAX_BLOBS; i++) {
    if (!blobs[i].active) {
      blobs[i].active = true;
      
      // Start from random end
      blobs[i].position = random(0, 2) == 0 ? 0 : NUM_LEDS - 1;
      
      // Velocity based on frequency (higher = faster)
      // Map frequency 3-150 to speed 5.0-15.0 LEDs per frame (MUCH faster)
      float speedFactor;
      if (freqBin < 3) {
        speedFactor = 5.0;  // Minimum speed for low frequencies
      } else {
        speedFactor = 5.0 + ((freqBin - 3.0) / 147.0) * 10.0;  // Scale from 5.0 to 15.0
      }
      blobs[i].velocity = (blobs[i].position == 0 ? 1 : -1) * speedFactor;
      
      // Size based on energy - larger blobs for visibility
      blobs[i].size = map(energy * 1000, 0, 500, 15, 35);
      blobs[i].size = constrain(blobs[i].size, 12, 40);
      
      // Wobble intensity based on magnitude
      blobs[i].wobble = map(magnitude * 1000, 0, 500, 2, 8);
      blobs[i].wobblePhase = random(0, 628) / 100.0;  // Random phase
      blobs[i].wobbleFrequency = 0.05 + ((freqBin - 3.0) / 147.0) * 0.15;  // 0.05 to 0.20
      
      // Color based on pitch (frequency)
      if (freqBin < 3) {
        blobs[i].colorIndex = 0;  // Purple for very low
      } else {
        blobs[i].colorIndex = ((freqBin - 3.0) / 147.0) * 255;  // 0-255 across frequency range
      }
      
      // Brightness
      blobs[i].brightness = map(magnitude * 1000, 0, 500, 150, 255);
      blobs[i].brightness = constrain(blobs[i].brightness, 100, 255);
      
      blobs[i].birthTime = millis();
      
      Serial.printf("Blob created: freq=%.1f, vel=%.2f, size=%.1f\n", 
                    freqBin, blobs[i].velocity, blobs[i].size);
      break;
    }
  }
}

void updateBlobWobble(float audioEnergy) {
  for (int i = 0; i < MAX_BLOBS; i++) {
    if (blobs[i].active) {
      // Wobble phase advances
      blobs[i].wobblePhase += blobs[i].wobbleFrequency;
      
      // Add extra wobble when audio is detected
      if (audioEnergy > NOISE_THRESHOLD) {
        blobs[i].wobblePhase += audioEnergy * 0.5;
      }
    }
  }
}

void updateBlobs() {
  for (int i = 0; i < MAX_BLOBS; i++) {
    if (blobs[i].active) {
      // Move blob
      blobs[i].position += blobs[i].velocity;
      
      // Age-based fading (blobs last ~6-8 seconds now that they move faster)
      unsigned long age = millis() - blobs[i].birthTime;
      if (age > 8000) {
        blobs[i].active = false;
        Serial.printf("Blob %d died of age\n", i);
      } else if (age > 6000) {
        // Fade out in last 2 seconds
        int fadeAmount = map(age, 6000, 8000, 0, blobs[i].brightness);
        blobs[i].brightness = blobs[i].brightness - (fadeAmount / 100);
      }
      
      // Deactivate if out of bounds
      if (blobs[i].position < -50 || blobs[i].position > NUM_LEDS + 50) {
        blobs[i].active = false;
        Serial.printf("Blob %d left strip\n", i);
      }
    }
  }
}

void checkBlobInteractions() {
  // Check each pair of active blobs
  for (int i = 0; i < MAX_BLOBS; i++) {
    if (!blobs[i].active) continue;
    
    for (int j = i + 1; j < MAX_BLOBS; j++) {
      if (!blobs[j].active) continue;
      
      // Calculate distance between blob centers
      float distance = abs(blobs[i].position - blobs[j].position);
      float combinedSize = (blobs[i].size + blobs[j].size) / 2;
      
      // If blobs are overlapping
      if (distance < combinedSize) {
        // Blur effect - increase size and reduce brightness
        blobs[i].size += 0.3;
        blobs[j].size += 0.3;
        
        blobs[i].brightness = max(50, blobs[i].brightness - 2);
        blobs[j].brightness = max(50, blobs[j].brightness - 2);
        
        // Slightly slow down
        blobs[i].velocity *= 0.98;
        blobs[j].velocity *= 0.98;
        
        // Color blending tendency
        if (random(0, 10) < 3) {
          blobs[i].colorIndex = (blobs[i].colorIndex + blobs[j].colorIndex) / 2;
        }
      }
    }
  }
}

void renderBlobs() {
  // Clear the strip
  fadeToBlackBy(leds, NUM_LEDS, 25);  // Light trail for fast-moving blobs
  
  static unsigned long lastRenderDebug = 0;
  bool debugNow = (millis() - lastRenderDebug > 2000);
  
  if (debugNow) {
    Serial.print("Blob positions: ");
  }
  
  // Render each active blob
  for (int i = 0; i < MAX_BLOBS; i++) {
    if (!blobs[i].active) continue;
    
    if (debugNow) {
      Serial.printf("%d:%.1f ", i, blobs[i].position);
    }
    
    // Calculate wobble offset
    float wobbleOffset = sin(blobs[i].wobblePhase) * blobs[i].wobble;
    
    // Render blob as Gaussian distribution
    for (int led = 0; led < NUM_LEDS; led++) {
      float distance = abs(led - (blobs[i].position + wobbleOffset));
      
      if (distance < blobs[i].size * 2) {
        // Gaussian falloff
        float intensity = exp(-(distance * distance) / (blobs[i].size * blobs[i].size));
        intensity = constrain(intensity, 0, 1);
        
        // Get color from palette
        uint8_t paletteIndex = blobs[i].colorIndex + (uint8_t)(led * 2);
        CRGB color = ColorFromPalette(currentPalette, paletteIndex, 
                                      blobs[i].brightness * intensity, 
                                      LINEARBLEND);
        
        // Blend with existing LED color
        leds[led] += color;
      }
    }
  }
  
  if (debugNow) {
    Serial.println();
    lastRenderDebug = millis();
  }
}
