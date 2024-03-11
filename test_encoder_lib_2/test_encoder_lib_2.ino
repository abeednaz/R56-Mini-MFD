/*
  File: RGB_Encoder.h
  Author: Abeed Nazar
  Date: 2/27/2024

  RGB encoder library header file
  Written for use with Sparkfun illuminated rotary encoder
  (P/N COM-15141)
*/

#include "RGB_Encoder.h"

void setup() {
  // put your setup code here, to run once:
  RGB_Encoder_Init();
  Serial.begin(115200);
}

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
