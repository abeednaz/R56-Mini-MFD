/*
  File: RGB_Encoder.cpp
  Version: 1
  Author: Abeed Nazar
  Date: 2/27/2024

  Incorporates functions in RGB_Encoder.h
*/

#include "RGB_Encoder.h"

typedef struct {
  char Direction;
  int Steps;
  int Degrees;
  char ButtonPress;
  QEI_State currState;
} QEI_data_t;

static QEI_data_t internalData;

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

  switch (internalData.currState) {
    case STATE_00:
      if (currRead_AB & 0b01) {
        internalData.Direction = CW;
      }
      if (currRead_AB & 0b10) {
        internalData.Direction = CCW;
      }
      break;
    case STATE_01:
      if (currRead_AB & 0b11) {
        internalData.Direction = CW;
      }
      if (currRead_AB & 0b00) {
        internalData.Direction = CCW;
      }
      break;
    case STATE_10:
      if (currRead_AB & 0b00) {
        internalData.Direction = CW;
      }
      if (currRead_AB & 0b11) {
        internalData.Direction = CCW;
      }
      break;
    case STATE_11:
      if (currRead_AB & 0b10) {
        internalData.Direction = CW;
      }
      if (currRead_AB & 0b01) {
        internalData.Direction = CCW;
      }
      break;
  }

  /* state variable is 00, 01, 10, 11 based on the enum 
     * (STATE_00 = 0b00, STATE_01 = 0b01, etc.)
     * so currState is represented by the actual current value of bits A and B
     * setting currState here unconditionally avoids state machine errors for missed steps
     */
  internalData.currState = (QEI_State)currRead_AB;

  if (internalData.Direction == CW) internalData.Steps++;
  if (internalData.Direction == CCW) internalData.Steps--;

  // RGB_Encoder::setAttributes(1, 1, 1, 1, STATE_00);
}

// void IRAM_ATTR EncoderRegButton() {
//   if (digitalRead(ENC_SW)) {
//     internalData.ButtonPress = true;
//   }
// }

RGB_Encoder::RGB_Encoder()
{
  pinMode(A_PIN, INPUT);
  pinMode(B_PIN, INPUT);
  pinMode(ENC_SW, INPUT);

  attachInterrupt(digitalPinToInterrupt(A_PIN), RunEncoderSM, CHANGE);
  attachInterrupt(digitalPinToInterrupt(B_PIN), RunEncoderSM, CHANGE);
  // attachInterrupt(digitalPinToInterrupt(ENC_SW), EncoderRegButton, RISING);
}

void RGB_Encoder::setAttributes(char Direction, int Steps, int Degrees, char ButtonPress, QEI_State currState)
{

}
