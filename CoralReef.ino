#include <Servo.h> 
#include <Adafruit_NeoPixel.h>

#define SENSORPIN   4       // break-beam
#define PIN_NEO_PIXEL  8    // Arduino pin that connects to NeoPixel
#define NUM_PIXELS  44      // The number of LEDs (pixels) on NeoPixel (actually 39, added a few to lign up the center with the clam)

Servo clam; 

Adafruit_NeoPixel NeoPixel(NUM_PIXELS, PIN_NEO_PIXEL, NEO_GRB + NEO_KHZ800);

// using millis() to avoid delays from using delay(), seperate counters to avoid confusion
unsigned long previousMillisLedStrip;
unsigned long previousMillisOpen;
unsigned long previousMillisLoop;
unsigned long previousMillisClose;
unsigned long currentMillisLoop;
unsigned long currentMillisLedStrip;
unsigned long currentMillisGame;

int halfPixels = 22;

int r = 0;                  // led strip red value
int g = 0;                  // led strip green value
int b = 255;                // led strip blue value
int servoPosition = 20;     //the servo will move to this position to close
int loopAdd = 0;            // timing for opening and closing the clam

int sensorState = 0, lastState=0;     // variable for reading the break-beam status

int pixel = 1;       // keep track of how many have been lit
int open = 0;        // tracking opening progress
int close = 0;       // tracking closing progress
int restarts = 0;    // how many times the game has restarted (lasts 1 minute);

boolean currentlyOpen;  // track if clam is open or closed
boolean broken;         // break-beam

void setup() {

  NeoPixel.clear();

  // start counters
  previousMillisLedStrip = 0;
  previousMillisLoop = millis();
  previousMillisOpen = millis();
  previousMillisClose = millis();
  currentMillisLoop = millis();
  currentMillisLedStrip = millis();

  NeoPixel.begin();   // initialize

  pinMode(9, OUTPUT);   // servo is an output
  
  pinMode(SENSORPIN, INPUT);      // initialize the sensor pin as an input   
  digitalWrite(SENSORPIN, HIGH);  // turn on the pullup
  clam.attach(9);                 // tell the servo object that its servo is plugged into pin 9
  
  servoPosition = 20;       // closed
  Serial.begin(115200);
  clam.write(servoPosition);    // start closed

  sensorState = HIGH;       // start as un-triggered  

  currentlyOpen = false;   // clam starts closed
  broken = false;          // break-beam starts untriggered

}

void loop(){

  sensorState = digitalRead(SENSORPIN);

  currentMillisLedStrip = millis();

  lightLedStrip(currentMillisLedStrip);  // turn on LED strip

  NeoPixel.show(); 
  
  // if sensor is triggered, switch broken variable to true --> starts moving clam
  if (sensorState == LOW || !sensorState) {
    broken = true;
  }
  
  if (!broken && servoPosition > 20) { // make sure clam is closed if sensor not triggered
    clam.write(20);
  } 
  
  // trigger clam to open and close
  if (broken == true) {  

    currentMillisLoop = millis();

    if (currentMillisLoop - previousMillisLoop >= 2050 + loopAdd){  // set pace

      if (currentlyOpen == false) {
        openClam();
        loopAdd += 50; // don't want the servo to move too fast
      }
      else if (currentlyOpen == true) {
        closeClam();
        loopAdd += 50; // don't want the servo to move too fast
      }
    }
  }

  // restart game if 1 minute has passed
  if ((millis() - (restarts * 60000)) >= 60000) {
    resetGame();
  }
}



// for opening the clam
void openClam() {

  if (open < 100) {  // do not move more than 100
    clam.write(servoPosition);
    servoPosition += 5;            // keeping track of progress
    open += 5;            // keeping track of progress
  }
  else {
    previousMillisLoop = currentMillisLoop;
    currentlyOpen = true;  // update status
    loopAdd = 0;  // reset
    open = 0;   // reset
  }
  
}

// for closing the clam
void closeClam() {

  if (close < 100) {  // do not move more than 100
    clam.write(servoPosition);
    servoPosition -= 5;            // keeping track of progress
    close += 5;            // keeping track of progress
  }
  else {
    previousMillisLoop = currentMillisLoop;
    currentlyOpen = false;  // update status
    loopAdd = 0;  // reset
    close = 0;  // reset
  }
  
}

// for lighting up the LED strip
void lightLedStrip(unsigned long currentMillisLedStrip) {

  if (currentMillisLedStrip - previousMillisLedStrip >= 100) {
      
    // set colors + show
    NeoPixel.setPixelColor(pixel, NeoPixel.Color(r, g, b));
    NeoPixel.setPixelColor((NUM_PIXELS - pixel), NeoPixel.Color(r, g, b));
    NeoPixel.show(); 
    
    // change color values 
    g += 7;
    b -= 12;
    previousMillisLedStrip = currentMillisLedStrip;
    pixel++; // add to number lit up

  }

  //reset if fully lit
  if (pixel > halfPixels){
    NeoPixel.clear();
    pixel = 0;
    b = 255;
    g = 0;
  }

}

void resetGame() {

  //reseting everything

  restarts++; // add to restart counter

  int pixel = 1;  
  int open = 0;
  int close = 0;

  int r = 0;  
  int g = 0; 
  int b = 255;  
  int servoPosition = 20;  
  int loopAdd = 0;

  int sensorState = 0, lastState = 0;

  NeoPixel.clear();
  
  previousMillisLedStrip = 0;
  previousMillisLoop = millis();
  previousMillisOpen = millis();
  previousMillisClose = millis();
  currentMillisLedStrip = millis();
  currentMillisLoop = millis();

  NeoPixel.begin();  
   
  clam.write(servoPosition); 

  sensorState = HIGH;   

  currentlyOpen = false;  
  broken = false;

}
