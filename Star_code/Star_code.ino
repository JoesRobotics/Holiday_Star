#include "FastLED.h"
#include <avr/interrupt.h>
#include <EEPROM.h>

//Fast LED examples combined for use with led art. 


#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    2
//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    32

#define COOLING  55
#define SPARKING 120
#define FRAMES_PER_SECOND 24

bool gReverseDirection = false;
int sensorPin = A0;
int BRIGHTNESS = 175; 
const int buttonPin = 3; 
int buttonState = 0;  
volatile int buttonCount = 0;
int setBright=1;
int hue =255;
uint8_t gHue = 0;



// Pride2015
// Animated, ever-changing rainbows.
// by Mark Kriegsman
CRGBArray<NUM_LEDS> ledes;

CRGB leds[NUM_LEDS];


void setup() {
  
  Serial.begin(9600);
  buttonCount=EEPROM.read(0);
  delay(3000); // 3 second delay for recovery
  Serial.println('Please refer to this github if you would like to modify the functionality, utilize fast_led lib. pin 2 =led, pin 3 = button, A0=pot');
  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonCounter, RISING);
 // pinMode(buttonPin, INPUT);  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS)
    .setCorrection(TypicalLEDStrip)
    .setDither(BRIGHTNESS < 255);
  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}


void loop()
{ 
  //increment if button state change
 
 Serial.println(buttonCount);
// State machine for button presses

  if( buttonCount == 0 )
  {  
    Serial.println("0 Bright White knob for brightness");
    setBright=1;
    //set brightness white
    for(int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1) {
      // Turn our current led on to white, then show the leds
      leds[whiteLed] = CRGB::White;
       //leds[whiteLed] = CHSV(127,255,255);
      // Show the leds (only one of which is set to white, from above)
      FastLED.show();
      //hue = map(analogRead(sensorPin),0,1024,0,255);
    } 
    delay(100);
  }
  

  if(buttonCount == 1)
  {
   //set color
    Serial.println("1 Color select knob for color");
    setBright=0;
    hue = map(analogRead(sensorPin),0,1024,0,255);
    for(int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1) {
    leds[whiteLed] = CHSV(hue,255,255);
    // Show the leds (only one of which is set to white, from above)
    FastLED.show();
    }
    delay(100);
   }
  
  if(buttonCount == 2)
  {
  //pride patterns 
    Serial.println("2 Pride show knob for brightness");
    setBright=1;
    pride();
    FastLED.show();  
   }
  
  if(buttonCount == 3)
  {
    //cyclon
    Serial.println("3 color cycle knob for brightness");
    setBright=1;
    static uint8_t hue = 0;
    // slide the led in one direction
  for(int i = 0; i < NUM_LEDS; i++) {
    // Set the i'th led to red 
    leds[i] = CHSV(hue++, 255, 255);
    FastLED.show(); 
    fadeall();
     delay(60);
     }
   }
  
  if(buttonCount == 4)
    {
       Serial.println("4 Fire knob for brightness");
       setBright=1;
       Fire2012(); // run simulation frame
       FastLED.show(); // display this frame
       FastLED.delay(1000 / FRAMES_PER_SECOND);  
    }
    
  if(buttonCount == 5)
    {
  //set color chasing
    Serial.println("5 trail knob for color select");
    setBright=0;
    int oldLed =0;
    for(int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1) {
      leds[whiteLed] = CHSV(hue,255,255);
      //oldLed=whiteLed+24;
      FastLED.show();
      hue = map(analogRead(sensorPin),0,1024,0,255);
      delay(60);
      fadeall();
    }
    
    }
    
  if(buttonCount == 6)
    {
    Serial.println("6 color changing trail knob for brightness");
    setBright=1;
    static uint8_t hue;
    for(int i = 0; i < NUM_LEDS; i++) {   
      ledes.fadeToBlackBy(200);
      leds[i] = CHSV(hue++,255,255);
      ledes(NUM_LEDS/2,NUM_LEDS-1) = ledes(NUM_LEDS/2 - 1 ,0);
      FastLED.delay(60);
    }
   }

  if(buttonCount == 7)
    {
     Serial.println("7 rainbow glitter knob for brightness");
     rainbowWithGlitter();
     FastLED.show();  
     FastLED.delay(1000/FRAMES_PER_SECOND); 
     gHue++;
     delay(20);
    }
   
  if(buttonCount == 8)
    {
    Serial.println("8 pastel juggle knob for brightness");
     juggle();
     FastLED.show();  
     FastLED.delay(1000/FRAMES_PER_SECOND);
     gHue++;
     delay(20); 
    }
   
  
//set brighness at end of each loop 
  BRIGHTNESS = map(analogRead(sensorPin),0,1024,0,255);
  if (setBright == 1)
   {
    FastLED.setBrightness(BRIGHTNESS);
   }
}


////////////////
void buttonCounter()
{   
    buttonCount++;
    if(buttonCount >= 9)
    {
    buttonCount = 0;  
    }
    EEPROM.update(0,buttonCount);
}


void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } }

void Fire2012()
{
// Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }
  // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      CRGB color = HeatColor( heat[j]);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}


void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

// This function draws rainbows with an ever-changing,
// widely-varying set of parameters.
void pride() 
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;
  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);
  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5,9);
  uint16_t brightnesstheta16 = sPseudotime;
  
  for( uint16_t i = 0 ; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    
    CRGB newcolor = CHSV( hue8, sat8, bri8);
    
    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS-1) - pixelnumber;
    
    nblend( leds[pixelnumber], newcolor, 64);
  }
  BRIGHTNESS = map(analogRead(sensorPin),0,1024,0,255);
}
