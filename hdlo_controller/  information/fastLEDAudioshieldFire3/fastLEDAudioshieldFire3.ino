// microphone activated FFT Fire with neopixel. Runs on teensy with audio shield.
// This is an adaptation of fire2012 using the frequency spectrum to make the
// fire instead of random sparks. 

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <FastLED.h>

#define LED_PIN     24
#define LED_PIN2    2
#define NUM_LEDS    432
#define NUM_LEDS2    8
#define BRIGHTNESS  255
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define COOLING 40

CRGB leds[NUM_LEDS];
CRGB leds2[NUM_LEDS2];
uint8_t heat[NUM_LEDS*3+6];  // this is the array from fire2012 expanded to HSV color
int avgHue = 128;
//const int myInput = AUDIO_INPUT_LINEIN;
const int myInput = AUDIO_INPUT_MIC;

// Create the Audio components.  These should be created in the
// order data flows, inputs/sources -> processing -> outputs
//
AudioInputI2S          audioInput;         // audio shield: mic or line-in
AudioAnalyzeFFT1024    myFFT;
AudioOutputI2S         audioOutput;        // audio shield: headphones & line-out

AudioConnection patchCord1(audioInput, 0, myFFT, 0);
AudioControlSGTL5000 audioShield;


void setup() {
  AudioMemory(12);
  audioShield.enable();
  audioShield.inputSelect(myInput);
  audioShield.volume(0.5);
  myFFT.windowFunction(AudioWindowHanning1024);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, LED_PIN2, COLOR_ORDER>(leds2, NUM_LEDS2).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );
  for(int i = 0; i<NUM_LEDS; i++) {
    heat[i*3] = 0;      //hue
    heat[i*3+1] = 255;  //saturation
    heat[i*3+2] = 0;    //brightness
  }
}

void loop() {
  float n;
  FastLED.show();
  if (myFFT.available()) {
    for( int i = 0; i < NUM_LEDS-3; i++) {
      heat[i*3+2] = qsub8( heat[i*3+2],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
       heat[k*3+2] = (heat[(k - 1)*3+2] + heat[(k - 2)*3+2] + heat[(k - 2)*3+2] ) / 3;
    }
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k*3] = (heat[(k - 1)*3] + heat[(k - 2)*3] + heat[(k - 2)*3] ) / 3;
    }
    for (int i=0; i<60; i++) {
      n = myFFT.read(i);
      if(n > .01){
        heat[(i/4)*3] = i*6;
        heat[(i/4)*3+2] = min(160, n*4000+heat[(i/4)*3+2]);
        //int g = random(10);
        //heat[g*3+2] = heat[g*3+2]/4;
        heat[2] = heat[2]/3;
      }
    }
    avgHue = 0;
    for(int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV(heat[i*3],heat[i*3+1], heat[i*3+2]);
      avgHue += heat[i*3];
    }
    avgHue /= NUM_LEDS; 
    for(int i = 0; i < NUM_LEDS2; i++) {
      leds2[i] = CHSV(avgHue,128,128);
    }
  }
}
