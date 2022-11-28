"""
  author : Leo Ly
"""

#include <FastLED.h>
#include <string.h>
#define LED_PIN     3
#define NUM_LEDS    16
#define BRIGHTNESS  70
CRGB leds[NUM_LEDS];
//uint32_t inputSerial = 0;         // a String to hold incoming data
uint32_t inputSerial[NUM_LEDS] = {0x832090, 0x862195, 0x862196, 
  0x852297, 0x86239A, 0x87249C, 
  0x87249D, 0x86259F, 0x8726A2, 
  0x8725A3, 0x8725A3, 0x8926A4, 
  0x8926A4, 0x8926A6, 0x8926A6, 0x8926A6}; 

uint8_t r,g,b;
uint8_t led_i = 0;
bool stringComplete = false;  // whether the string is complete
int8_t shiftBits = 0;
uint8_t recvCnt = 0;
// the setup routine runs once when you press reset:
void setup() {
//  FastLED.addLeds<WS2812B, LED_PIN, GBR >(leds, NUM_LEDS);
  FastLED.addLeds<WS2812B, LED_PIN, RBG >(leds, NUM_LEDS);
  FastLED.setBrightness( BRIGHTNESS );
  Serial.begin(115200);
  memset(inputSerial, 0, sizeof(uint32_t)*NUM_LEDS);
}


void serialEvent() {
  uint8_t inChar;
  while (Serial.available()) {
    inChar = Serial.read();

      if (stringComplete == false) {
  
        inputSerial[recvCnt] |= ((inChar & 0xFF) << 8*shiftBits);
        shiftBits++;
        if (shiftBits == 3) {
          shiftBits = 0;
          recvCnt++;
         
          if (recvCnt == NUM_LEDS) {
            stringComplete = true;
            recvCnt = 0;
          } 
        }

    }
  }
}

void loop() {
  if (stringComplete) {
    int brightness = 0;
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(inputSerial[NUM_LEDS - 1 - i]);   
//      brightness += inputSerial[NUM_LEDS - 1 - i];
    }
//    brightness = brightness/3;
//    //normalise
//    brightness = (brightness - 20)/(90 - 20);
//    FastLED.setBrightness(brightness);
    FastLED.show();

    memset(inputSerial, 0, sizeof(uint32_t)*NUM_LEDS);
    stringComplete = false;
  }
}

//void serialEvent() {
//  while (Serial.available()) {
//    uint8_t inChar = (uint8_t) Serial.read();
//    
//    inputSerial[shiftBits] = inChar;
//    shiftBits++;
//    if (shiftBits == NUM_LEDS) {
//      stringComplete = true;
//      shiftBits = 0;
//    }
//  }
//}
