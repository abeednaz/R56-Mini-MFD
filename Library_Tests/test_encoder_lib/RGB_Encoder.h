/*
  File: RGB_Encoder.h
  Version: 1
  Author: Abeed Nazar
  Date: 2/27/2024

  RGB encoder library header file
  Written for use with Sparkfun illuminated rotary encoder
  (P/N COM-15141)
*/

#ifndef RGB_ENCODER_H
#define RGB_ENCODER_H

#include <Arduino.h>

#define A_PIN 1
#define B_PIN 2

#define LED_R 7
#define LED_G 6
#define LED_B 5

#define ENC_SW 4

#define CW 1
#define CCW 0

typedef enum {
  STATE_00,
  STATE_01,
  STATE_10,
  STATE_11,
} QEI_State;

class RGB_Encoder{
  private:
    struct QEI_data{
      char Direction;
      int Steps;
      int Degrees;
      char ButtonPress;
      QEI_State currState;
    };

    QEI_data data;

    static void IRAM_ATTR RunEncoderSM();

  public:
    // Constructor for RGB Encoder object
    RGB_Encoder();

    // Access functions
    // getDirection()
    // Returns the current direction of rotation (CW or CCW)
    int getDirection();

    // getSteps()
    // Returns the current number of steps (positive or negative)
    int getSteps();

    // getAngle()
    // Returns the current angle in degrees (0-360)
    int getAngle();

    // getButtonState()
    // Returns 1 for button being pressed, 0 if not
    int getButtonState();

    // Modify functions
    // reset()
    // Sets the number of steps and degrees back to 0
    int reset();

    // LED functions
    // setColor
    // Sets the LED color according to input array
    // Capped range from 0-255 for R, G, and B
    int setColor(char RGB[3]);
    
    void setAttributes(char Direction, int Steps, int Degrees, char ButtonPress, QEI_State currState);
};

#endif // RGB_ENCODER_H
