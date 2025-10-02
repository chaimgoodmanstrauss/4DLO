
#include "teensy4controller.h"
#include "ledconstants.h"
#include "edgesetup.h"

// #include "trillcontroller.h" // If we would like to add this.

// TBD include: all of our color functions, etc, ahead of this point
// this file should be as simple as possible.

// load in an array with information about each string of lights


int count = 0;
void setup() {
  

    // Prepare serial output, if we wish to use it for debugging or monitoring data.
    Serial.begin(9600);
    Serial.println("Yo the HDLO LED contoller ");
    Serial.println("We are using "+String(numberofleds)+" on "+String(numberofpins)+" pins.");
/*
    //trillsetup(); // if we wish to use the trill bar, include  "trillcontroller.h" 
  
    octocontroller.begin(); //initialize the octocontroller 
    // create our teensycontroller 
    teensycontroller = new CTeensy4Controller<RGB, WS2811_800kHz>(&octocontroller);

    // Initialize FastLED
    FastLED.addLeds(teensycontroller, rgbarray, numberofpins * ledsperstrip);
    // set various parameters
    FastLED.setBrightness(255);
    


    bool animateinitializationq = true;

    // step through the array representing the LEDs and change the color to 
    // something or another
    for(int i = 0;i<numberofleds;i++){
      rgbarray[i]=0xdbc1dd;//0x223255;//0xA7Efff;
      if(i%2==0){ 
        if(animateinitializationq){FastLED.show();}}
      //this determines a slow initialization or a rapid one
    }
    FastLED.show();
    //this determines a slow initialization or a rapid one
   */ 


  /////////////
  initedgedata();



}


void loop() {
  
}
