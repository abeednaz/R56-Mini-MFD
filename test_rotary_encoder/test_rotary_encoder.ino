/*
  2/25/2024
  Rotary encoder test program
  Sparkfun rotary encoder with RGB LED built into knob progresses through the 
  RGB color wheel while the encoder is turned, and printout when its button
  is pressed.
*/

#define A_PIN 1
#define B_PIN 2

#define LED_R 7
#define LED_G 6
#define LED_B 5

#define ENC_SW 4

#define CW 1
#define CCW 0

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
  int Degrees;
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

void setup() {
  // put your setup code here, to run once:
  pinMode(A_PIN, INPUT);
  pinMode(B_PIN, INPUT);
  pinMode(ENC_SW, INPUT);

  attachInterrupt(digitalPinToInterrupt(A_PIN), RunEncoderSM, CHANGE);
  attachInterrupt(digitalPinToInterrupt(B_PIN), RunEncoderSM, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_SW), EncoderRegButton, RISING);
  Serial.begin(115200);
  Serial.println("Starting");
}

void loop() {
  // put your main code here, to run repeatedly:

  int RGB[3];
  static int lastAngle = 0;
  static int angle = 0;
  // Serial.print(QEI_Status.Steps);
  // Serial.println();

  angle = GetAngleFromEncoder(QEI_Status.Steps);
  GetColorFromAngle(angle, RGB);

  // if (lastAngle != angle){
  //   Serial.println(angle);
  //   Serial.print("R: ");
  //   Serial.print(RGB[0]);
  //   Serial.print(" G: ");
  //   Serial.print(RGB[1]);
  //   Serial.print(" B: ");
  //   Serial.print(RGB[2]);
  //   Serial.println();
  // }

  if (lastAngle != angle) {
    analogWrite(LED_R, RGB[0]);
    analogWrite(LED_G, RGB[1]);
    analogWrite(LED_B, RGB[2]);
  }

  static int i = 0;

  if (QEI_Status.ButtonPress == true) {
    QEI_Status.ButtonPress = false;
    i++;
    Serial.print("BUTTON PRESSED ");
    Serial.print(i);
    Serial.println();
  }

  lastAngle = angle;
}

// 0 = 0deg
// 92 = 360deg
// rolls over 360deg or 92 steps
int GetAngleFromEncoder(int encoderSteps) {
  int result = (encoderSteps % MAX_ENCODER) * 360 / MAX_ENCODER;
  if (result < 0) result += 360;
  return result;
}

/*
 * angle and RGB value index table
 * angle    R   G   B
 * 0        255 0   0
 * 60       255 255 0
 * 120      0   255 0
 * 180      0   255 255
 * 240      0   0   255
 * 300      255 0   255
 * 
 * source: https://dawnsbrain.com/wp-content/uploads/2009/03/rgb-color-wheel-lg.jpg
 */
void GetColorFromAngle(int angle, int colors[3]) {
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