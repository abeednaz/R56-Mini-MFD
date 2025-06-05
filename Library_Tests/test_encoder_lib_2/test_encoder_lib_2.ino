/*
  File: RGB_Encoder.h
  Author: Abeed Nazar
  Date: 2/27/2024

  RGB encoder library header file
  Written for use with Sparkfun illuminated rotary encoder
  (P/N COM-15141)
*/

#include "RGB_Encoder.h"
#define RAINBOW_ENCODER_TEST
// #define ALL_COLORS_TEST

void setup() {
  // put your setup code here, to run once:
  RGB_Encoder_Init();
  Serial.begin(115200);
}

#ifdef RAINBOW_ENCODER_TEST

void loop() {
  // put your main code here, to run repeatedly:
  int angle;
  static char RGB [3] = {0, 0, 0};
  static int lastAngle = 0;
  angle = RGB_Encoder_getAngle();
  if (lastAngle != angle){
    Serial.println(angle);
    GetColorFromAngle(angle, RGB);
    RGB_Encoder_setColor(RGB);
  }
  if (RGB_Encoder_getButtonState()){
    Serial.println("RESET");
    RGB_Encoder_reset();
  }
  lastAngle = angle;
}

#endif // RAINBOW_ENCODER_TEST

#ifdef ALL_COLORS_TEST

void loop() {
  // put your main code here, to run repeatedly:
  
  char RGB [3] = {0, 0, 0};

  for (int R = 0; R < 256; R++){
    for (int G = 0; G < 256; G++){
      for (int B = 0; B < 256; B++){
        Serial.print("R: ");
        Serial.print(R);
        Serial.print(" | R: ");
        Serial.print(G);
        Serial.print(" | B: ");
        Serial.print(B);
        Serial.println();

        RGB[0] = R;
        RGB[1] = G;
        RGB[2] = B;

        RGB_Encoder_setColor(RGB);
        // delay(1);

      }
    }
  }
  
}

#endif // ALL_COLORS_TEST

void GetColorFromAngle(int angle, char colors[3]) {
  int redVal, greenVal, blueVal;
  // follows the color wheel
  if (angle <= 60) {
    redVal = 255;
    greenVal = angle * 255 / 60;
    blueVal = 0;
  } else if (angle <= 120) {
    redVal = 255 - ((angle - 60) * 255 / 60);
    greenVal = 255;
    blueVal = 0;
  } else if (angle <= 180) {
    redVal = 0;
    greenVal = 255;
    blueVal = (angle - 120) * 255 / 60;
  } else if (angle <= 240) {
    redVal = 0;
    greenVal = 255 - ((angle - 180) * 255 / 60);
    blueVal = 255;
  } else if (angle <= 300) {
    redVal = (angle - 240) * 255 / 60;
    greenVal = 0;
    blueVal = 255;
  } else {  // 300-360
    redVal = 255;
    greenVal = 0;
    blueVal = 255 - ((angle - 300) * 255 / 60);
  }
  // scale the values up for the PWM library which goes up to 1000
  colors[0] = redVal;
  colors[1] = greenVal;
  colors[2] = blueVal;
}

