// includes
#include <Adafruit_TiCoServo.h>
#include <known_16bit_timers.h>
//#include <Adafruit_NeoPixel.h>
#include "FastLED.h"

// pins
#define ESC_PIN 9
#define HRN_PIN 8
#define ESC_PWR 7
#define ESC_PRG 6
#define VLT_REF 5
#define RGB_PIN 3

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
int lightSpeed = 3;
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

int btDelay = 200;
long lastTime = 0;

void setup() {
  // bluetooth
  Serial.begin(9600);
  Serial1.begin(9600);

  pinMode(VLT_REF, OUTPUT);
  digitalWrite(VLT_REF, HIGH);

  // lights
  FastLED.addLeds<NEOPIXEL, RGB_PIN>(leds, NUM_LIGHTS);
  while(!Serial1.available());
  Serial1.read();

  // relay pins
  pinMode(ESC_PWR, OUTPUT);
  pinMode(ESC_PRG, OUTPUT);

  // esc
  digitalWrite(ESC_PWR, HIGH);
  mESC.attach(ESC_PIN);

}

void loop() {
  recieveBtData();
  controlLights();
}

void controlLights() {
  switch (lightType) {
    case LIGHT_OFF:
      for (int i = 0; i < NUM_LIGHTS; i++) {
        leds[i].setRGB(0, 0, 0);
      }
      break;
    case LIGHT_FLASH:
      if ((lightTime / lightSpeed) % 2 == 0) {
        for (int i = 0; i < NUM_LIGHTS; i++) {
          leds[i].setRGB(0, 0, 0);
        }
      } else {
        for (int i = 0; i < NUM_LIGHTS; i++) {
          leds[i].setRGB(lightRed, lightGreen, lightBlue);
        }
      }
      break;
    case LIGHT_CHASE: {
        int spot = lightTime / lightSpeed;
        for (int i = 0; i < NUM_LIGHTS; i++) {
          if (i < 15) {
            leds[(spot + i) % NUM_LIGHTS].setRGB(lightRed, lightGreen, lightBlue);
          } else {
            leds[(spot + i) % NUM_LIGHTS].setRGB(0, 0, 0);
          }
        }
      }
      break;
    case LIGHT_FADE:
        for (int i = 0; i < NUM_LIGHTS; i++) {
          leds[i].setRGB(lightRed / ((lightTime / lightSpeed) % 10),
                        lightGreen / ((lightTime / lightSpeed) % 10),
                        lightBlue / ((lightTime / lightSpeed) % 10));
        }
    case LIGHT_SOLID:
      for (int i = 0; i < NUM_LIGHTS; i++) {
        leds[i].setRGB(lightRed, lightGreen, lightBlue);
      }
      break;
  }

  lightTime++;
  delay(btDelay - millis() % btDelay); // only update on 'btDelay' marks
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
  long a = millis();
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
  Serial.println(millis() - a);
}

char nextChar() {
  while (!Serial1.available());
  return (char) Serial1.read();
}

void sendToBt(const char * data) {
  Serial1.print(data);
  Serial1.print(DELIMITER);  
}
