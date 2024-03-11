/*
  File: RGB_Encoder.h
  Version: 1
  Author: Abeed Nazar
  Date: 2/28/2024

  RGB encoder library header file
  Written for use with Sparkfun illuminated rotary encoder
  (P/N COM-15141)
*/

#ifndef __RGB_ENCODER_H
#define __RGB_ENCODER_H



#define A_PIN 1
#define B_PIN 2

#define LED_R 7
#define LED_G 6
#define LED_B 5

#define ENC_SW 4

#define CW 1
#define CCW 0

void RGB_Encoder_Init();
// Access functions
// getDirection()
// Returns the current direction of rotation (CW or CCW)
int RGB_Encoder_getDirection();

// getSteps()
// Returns the current number of steps (positive or negative)
int RGB_Encoder_getSteps();

// getAngle()
// Returns the current angle in degrees (0-360)
int RGB_Encoder_getAngle();

// getButtonState()
// Returns 1 for button being pressed, 0 if not
int RGB_Encoder_getButtonState();

// Modify functions
// reset()
// Sets the number of steps and degrees back to 0
void RGB_Encoder_reset();

// LED functions
// setColor
// Sets the LED color according to input array
// Capped range from 0-255 for R, G, and B
void RGB_Encoder_setColor(char RGB[3]);


#endif // __RGB_ENCODER_H
