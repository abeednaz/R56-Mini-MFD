/*
  File: RGB_Encoder.c
  Version: 1
  Author: Abeed Nazar
  Date: 2/28/2024

  Incorporates functions in RGB_Encoder.h
*/

#include <Arduino.h>
#include "RGB_Encoder.h"

#define MAX_ENCODER 96

typedef enum {
  STATE_00,
  STATE_01,
  STATE_10,
  STATE_11,
} QEI_State;

typedef struct {
  int Direction;
  int Steps;
  int ButtonPress;
  QEI_State currState;
} QEI_t;

static QEI_t QEI_Status;

void IRAM_ATTR RunEncoderSM() {
  static char A_state, B_state, currRead_AB;
  A_state = digitalRead(A_PIN);
  B_state = digitalRead(B_PIN);
  currRead_AB = (QEI_State)((A_state << 1) | B_state);

  /*
     *  encoder combinations table:
     *  last    |   curr
     *  A   B   |   A   B
     * ---------+--------------
     *  1   0   |   0   0   CW
     *  0   0   |   0   1   CW
     *  1   1   |   1   0   CW
     *  0   1   |   1   1   CW  
     *          |
     *  0   1   |   0   0   CCW
     *  1   1   |   0   1   CCW
     *  0   0   |   1   0   CCW
     *  1   0   |   1   1   CCW
     */

  switch (QEI_Status.currState) {
    case STATE_00:
      if (currRead_AB & 0b01) {
        QEI_Status.Direction = CW;
      }
      if (currRead_AB & 0b10) {
        QEI_Status.Direction = CCW;
      }
      break;
    case STATE_01:
      if (currRead_AB & 0b11) {
        QEI_Status.Direction = CW;
      }
      if (currRead_AB & 0b00) {
        QEI_Status.Direction = CCW;
      }
      break;
    case STATE_10:
      if (currRead_AB & 0b00) {
        QEI_Status.Direction = CW;
      }
      if (currRead_AB & 0b11) {
        QEI_Status.Direction = CCW;
      }
      break;
    case STATE_11:
      if (currRead_AB & 0b10) {
        QEI_Status.Direction = CW;
      }
      if (currRead_AB & 0b01) {
        QEI_Status.Direction = CCW;
      }
      break;
  }

  /* state variable is 00, 01, 10, 11 based on the enum 
     * (STATE_00 = 0b00, STATE_01 = 0b01, etc.)
     * so currState is represented by the actual current value of bits A and B
     * setting currState here unconditionally avoids state machine errors for missed steps
     */
  QEI_Status.currState = (QEI_State)currRead_AB;

  if (QEI_Status.Direction == CW) QEI_Status.Steps++;
  if (QEI_Status.Direction == CCW) QEI_Status.Steps--;
}

void IRAM_ATTR EncoderRegButton() {
  if (digitalRead(ENC_SW)) {
    QEI_Status.ButtonPress = true;
  }
}

void RGB_Encoder_Init()
{
  pinMode(A_PIN, INPUT);
  pinMode(B_PIN, INPUT);
  pinMode(ENC_SW, INPUT);

  attachInterrupt(digitalPinToInterrupt(A_PIN), RunEncoderSM, CHANGE);
  attachInterrupt(digitalPinToInterrupt(B_PIN), RunEncoderSM, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_SW), EncoderRegButton, RISING);
}

int RGB_Encoder_getDirection()
{
  return QEI_Status.Direction;
}

int RGB_Encoder_getSteps()
{
  return QEI_Status.Steps;
}

int RGB_Encoder_getAngle()
{
  int result = (QEI_Status.Steps % MAX_ENCODER) * 360 / MAX_ENCODER;
  if (result < 0) result += 360;
  return result;
}

int RGB_Encoder_getButtonState()
{
  if (QEI_Status.ButtonPress == true)
  {
    QEI_Status.ButtonPress = false;
    return true;
  }
  return false;
}

void RGB_Encoder_reset()
{
  QEI_Status.ButtonPress = false;
  QEI_Status.Direction = CW;
  QEI_Status.Steps = 0;
  QEI_Status.currState = STATE_00;
}

void RGB_Encoder_setColor(char RGB[3])
{
  analogWrite(LED_R, RGB[0]);
  analogWrite(LED_G, RGB[1]);
  analogWrite(LED_B, RGB[2]);
}

