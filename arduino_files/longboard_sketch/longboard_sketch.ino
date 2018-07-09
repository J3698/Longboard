// includes
#include <Adafruit_TiCoServo.h>
#include <known_16bit_timers.h>
//#include <Adafruit_NeoPixel.h>
#include "FastLED.h"

// pins
#define RGB_PIN 3
#define ESC_PIN 9

// lighting variables
#define NUM_LIGHTS 120
// Adafruit_NeoPixel mStrip;
CRGB leds[NUM_LIGHTS];

const char LIGHT_OFF = 'o';
const char LIGHT_FLASH = 'f';
const char LIGHT_CHASE = 'c';
const char LIGHT_FADE = 'a';
const char LIGHT_SOLID = 's';
char lightType = LIGHT_SOLID;
long lightSpeed = 50L;
int lightRed = 50;
int lightGreen = 50;
int lightBlue = 50;
int lightTime = 0; 

// motor
Adafruit_TiCoServo mESC;

// bluetooth variables
const char DELIMITER = '\r';
const char STOP = '\n';
const char BT_THROTTLE = 't';
const char BT_LIGHT_TYPE = 'l';
const char BT_LIGHT_SPEED = 's';
const char BT_LIGHT_COLOR = 'c';

void setup() {
  // bluetooth
  Serial.begin(9600);
  Serial1.begin(9600);

  // motor
  // mESC.attach(ESC_PIN);
  // arm(mESC);

  // lights
  // mStrip = Adafruit_NeoPixel(NUM_LIGHTS, RGB_PIN);
  // mStrip.begin();
  // mStrip.show();
  FastLED.addLeds<NEOPIXEL, RGB_PIN>(leds, NUM_LIGHTS);

  while(!Serial1.available());
  Serial1.read();
}

void loop() {
  long a = millis();
  recieveBtData();
  Serial.println(millis() - a);
  controlLights();
}

boolean s = true;
void controlLights() {
  switch (lightType) {
    case LIGHT_OFF:
      for (int i = 0; i < NUM_LIGHTS; i++) {
        //mStrip.setPixelColor(i, 0, 0, 0);
        leds[i].setRGB(0, 0, 0);
      }
      break;
    case LIGHT_FLASH:
      if (millis() % (30L * lightSpeed) < (30L * lightSpeed) / 2) {
        if (s) {
          s = false;
          //Serial.println(millis());
        }
        for (int i = 0; i < NUM_LIGHTS; i++) {
          //mStrip.setPixelColor(i, 0, 0, 0);
          leds[i].setRGB(0, 0, 0);
        }
      } else {
        if (!s) {
          s = true;
          //Serial.println(millis());
        }
        for (int i = 0; i < NUM_LIGHTS; i++) {
          //mStrip.setPixelColor(i, lightRed, lightGreen, lightBlue);
          leds[i].setRGB(lightRed, lightGreen, lightBlue);
        }
      }
      break;
    case LIGHT_CHASE:
        for (int i = 0; i < NUM_LIGHTS; i++) {
          //mStrip.setPixelColor(i, 0, 0, 0);
          leds[i].setRGB(0, 0, 0);
        }
        //mStrip.setPixelColor(((millis() / 30) / lightSpeed * 5) % 120,
        //            lightRed, lightGreen, lightBlue);
        leds[((millis() / 30) / lightSpeed * 5) % 120].setRGB(lightRed, lightGreen, lightBlue);
      break;
    case LIGHT_FADE:
      break;
    case LIGHT_SOLID:
      for (int i = 0; i < NUM_LIGHTS; i++) {
        //mStrip.setPixelColor(i, lightRed, lightGreen, lightBlue);
        leds[i].setRGB(lightRed, lightGreen, lightBlue);
      }
      break;
  }
  lightTime = millis();
  // mStrip.show();
  FastLED.show();
}

void process(const char* message) {
  switch (message[0]) {
    case BT_THROTTLE:
      Serial.println("throttle change");
      mESC.write(atoi(message + 1));
      break;
    case BT_LIGHT_TYPE:
      Serial.println("light type change");
      lightType = message[1];
      break;
    case BT_LIGHT_SPEED:
      Serial.println("light speed change");
      lightSpeed = atoi(message + 1);
      break;
    case BT_LIGHT_COLOR:
      Serial.println("light color change");
      int color = atoi(message + 1);
      lightRed = atoi(strtok(message + 1, ","));
      lightGreen = atoi(strtok(NULL, ","));
      lightBlue = atoi(strtok(NULL, ","));
      Serial.print(lightRed); Serial.print(" "); Serial.print(lightGreen);
      Serial.print(" "); Serial.print(lightBlue); Serial.println(" ");
      break;
  }
}

void recieveBtData() {
  sendToBt("OK");

  char fromBt[20] = "";
  int fromBtIndex = 0;
  char lastChar = nextChar();
  while (lastChar != STOP) {
    fromBt[fromBtIndex++] = lastChar;
    if (fromBt[fromBtIndex - 1] == DELIMITER) {
      fromBt[fromBtIndex] = '\0';
      process(fromBt);
      fromBtIndex = 0;
    }

    lastChar = nextChar();
  }
}

char nextChar() {
  while (!Serial1.available());
  return (char) Serial1.read();
}

void sendToBt(const char * data) {
  Serial1.print(data);
  Serial1.print(DELIMITER);  
}

void testServo(Adafruit_TiCoServo esc) {
  for (int i = 0; i < 175; i += 5) {
    Serial.println(i);
    esc.write(i);
    delay(1000);
  }
}

void arm(Adafruit_TiCoServo esc) {
  esc.write(90);
  delay(1000);
}
