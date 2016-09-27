/**
 * Arduino code for the box.
 * Thanks to the FastLED community (http://fastled.io)
 */

//////////////////////
// FastLed constants//
//////////////////////
#include <FastLED.h>
FASTLED_USING_NAMESPACE
#define CHIPSET     WS2812B
#define COLOR_ORDER GRB

#define DATA_PIN 11
#define NUM_LEDS 165
#define BRIGHTNESS  96
int ledHeight = 0;
uint8_t gHue = 0;
CRGB leds[NUM_LEDS];
#define FRAMES_PER_SECOND  120


///////////////////
// bike constants//
///////////////////

int revolutions = 0;
int mappedRevolutions = 0;
unsigned long int lastmillis = 0;
long lastRevolutions = 0;
static unsigned long last_interrupt_time = 0;

float rpm = 0;

////////////////////
// game variables //
////////////////////
int idleSeconds = 0;
unsigned long int celebration_time = 0;
bool just_won = false;


////////////
//  setup //
////////////

void setup() {
  delay(2000);
  FastLED.addLeds<WS2812B, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness( BRIGHTNESS );
  pinMode(2, INPUT);
  attachInterrupt(0, countRotation, RISING);
  Serial.begin(9600);
}

/////////////
// helpers //
/////////////

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, juggle, bpm };
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

/////////
// main//
/////////

void loop() {
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);
  revolutions = constrain(revolutions, 0, NUM_LEDS);

  EVERY_N_MILLISECONDS( 20 ) {
    gHue++;  // slowly cycle the "base color" through the rainbow
  }
  EVERY_N_SECONDS(1) {
    idleSeconds ++; // counts the number of idle seconds

  }

  if (revolutions < 1 && idleSeconds > 10) {
    // IDLE
    sinelon();
  } else if (revolutions < NUM_LEDS) {
    // IN GAME
    countdown();
    fill(revolutions);
  } else {
    // YOU WIN

    if (!just_won) {
      // YOU JUST WON
      detachInterrupt(0);
      just_won = true;
      celebration_time = millis();
      nextPattern();
    }

    if (millis() - celebration_time < 15000) {
      gPatterns[gCurrentPatternNumber]();
      idleSeconds = 0; // prevent idle animation
    } else {
      // FastLED.clear();
      revolutions = 0;
      just_won = false;
      attachInterrupt(0, countRotation, RISING);
    }
  }
}

/**
   xxx too much instruction: refactor me
*/
void countRotation() {
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) {
    revolutions += 2;
  }
  last_interrupt_time = interrupt_time;
  idleSeconds = 0; // if interrupt was detected, reset idle count
}

/**
   sinelon;rainbow; rainbowWithGlitter confetti, bpm and juggle
   shamelessly stolen from FASTLed Library examples (many thanks to Mark Kriegsman)
*/
void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter)
{
  if ( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(12, 0, NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);
}

void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for ( int i = 0; i < 8; i++) {
    leds[beatsin16(i + 7, 0, NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for ( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}


void fill(int ledHeight) {
  FastLED.clear();

  for (int i = 0; i < ledHeight; i++) {
    int color_mapped = map(i, 0, NUM_LEDS, 0, 255);
    int blue = 255 - color_mapped;
    leds[i] = CRGB(color_mapped, 0, blue);
  }

  //fill_rainbow(leds, ledHeight, gHue, 7);
  //fill_gradient_RGB(leds, ledHeight, CRGB::Blue, CRGB::Red);
  FastLED.show();
}

void countdown() {
  if (millis() - lastmillis >= 1700 && revolutions > 0) {
    revolutions --;
    lastmillis = millis();
  }
}
