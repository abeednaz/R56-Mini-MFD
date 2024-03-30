/*
  File: GaugePainter.cpp
  Version: 1
  Author: Abeed Nazar
  Date: 3/17/2024

  Gauge Painter implementation file
  Written with use of WaveShare 2.0 inch LCD library
  Provides helper functions to display the gaugegImage_digits
*/

#include "GaugePainter.h"

#include <SPI.h>
#include "LCD_Driver.h"
#include "GUI_Paint.h"
#include "image.h"

// array to reference max/min limits on each gauge 
const int lims[4][2] = { { CTEMP_MIN, CTEMP_MAX },
                         { OTEMP_MIN, OTEMP_MAX },
                         { OPRESS_MIN, OPRESS_MAX },
                         { BPRESS_MIN, BPRESS_MAX } };

// array to reference digits while printing
const unsigned char * const gImage_digits[] = {
    gImage_digit_0,
    gImage_digit_1,
    gImage_digit_2,
    gImage_digit_3,
    gImage_digit_4,
    gImage_digit_5,
    gImage_digit_6,
    gImage_digit_7,
    gImage_digit_8,
    gImage_digit_9,
};

// const unsigned char *digits = {&gImage_digit_0, 
//                                 &gImage_digit_1 };

// Gauge object constructor
// Parameters: none
// Returns: none
// Constructor for new Gauge object
Gauge::Gauge() {
}

// Gauge.begin()
// Parameters: none
// Returns: none
// Begins new gauge
void Gauge::begin() {
  for (int i = 0; i < NUM_INDICES; i++) {
    _gaugeState[i] = INDEX_OFF;
  }

  Config_Init();
  LCD_Init();
  // LCD_Clear(BLACK);
  LCD_Clear(ORANGE_BG);
  Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, ROTATE_180, WHITE);
  // Paint_DrawCircle((LCD_WIDTH / 2) + 1, (LCD_WIDTH / 2) + 2, (LCD_WIDTH / 2) - 1, ORANGE_BG, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  Paint_SetMirroring(MIRROR_NONE);
}

// Gauge.setType()
// Converts screen to new display
// Parameters:
// - type: type of gauge to change to (coolant temp, oil pressure, etc.)
void Gauge::setType(GaugeType type) {
  _gaugeType = type;
  _limits.lowerLim = lims[type][0];
  _limits.upperLim = lims[type][1];
  clearIcon();
  switch (type) {
    case COOLANT_TEMP:
      paintIcon(COOLANT_ICON);
      break;
    case OIL_TEMP:
      paintIcon(OIL_ICON);
      paintUnit(UNIT_CELCIUS);
      break;
    case OIL_PRESS:
      paintIcon(OIL_ICON);
      paintUnit(UNIT_PSI);
      break;
    case BOOST_PRESS:
      paintIcon(TURBO_ICON);
      paintUnit(UNIT_PSI);
      break;
    case G_METER:
      break;
  }
}

// Gauge.paintGauge
// Paints gauge indices based on input value
// Scaled based on range of current gauge in GaugePainter.cpp->lims[]
// Parameters:
// - value: value to paint
void Gauge::paintGauge(int value) {
  static int currIndex; static bool firstPrint = true;
  int topIndex = map(value, _limits.lowerLim, _limits.upperLim, 0, 9);
  // if gauge state is the same, don't paint it
  if (currIndex == topIndex && !firstPrint){
    return;
  }
  paintIndices(0, topIndex, INDEX_ON);
  paintIndices(topIndex + 1, NUM_INDICES, INDEX_OFF);
  currIndex = topIndex;
  firstPrint = false;
}

// Gauge.paintValue
// Paints value as text in center of gauge
void Gauge::paintValue(int value) {
  // clear any existing values
  clearNumber();

  int yStart = 96;
  int xStart[3] = { 0, 0, 0 };
  int digits[3] = { 0, 0, 0 };

  // single digit number
  if (value < 10) {
    xStart[0] = 109;
    digits[2] = value;
    Paint_DrawImage(gImage_digits[digits[2]], xStart[0], yStart, 21, 49);
  }
  // double digit number
  if (value >= 10 && value < 100) {
    xStart[0] = 98;
    xStart[1] = 121;
    digits[1] = value / 10;
    digits[2] = value % 10;
    Paint_DrawImage(gImage_digits[digits[1]], xStart[0], yStart, 21, 49);
    Paint_DrawImage(gImage_digits[digits[2]], xStart[1], yStart, 21, 49);
  }
  // triple digit number
  if (value >= 100) {
    xStart[0] = 86;
    xStart[1] = 109;
    xStart[2] = 132;
    digits[0] = value / 100;
    digits[1] = (value % 100) / 10;
    digits[2] = value % 10;
    Paint_DrawImage(gImage_digits[digits[0]], xStart[0], yStart, 21, 49);
    Paint_DrawImage(gImage_digits[digits[1]], xStart[1], yStart, 21, 49);
    Paint_DrawImage(gImage_digits[digits[2]], xStart[2], yStart, 21, 49);
  }

  // String debugStr = String(value);
  // debugStr.concat(" ");
  // debugStr.concat(digits[0]);
  // debugStr.concat(" ");
  // debugStr.concat(digits[1]);
  // debugStr.concat(" ");
  // debugStr.concat(digits[2]);
  // printDebugMsg(debugStr, 0);
}

// Gauge.paintIndex()
// Parameters:
// - index: index of the gauge to paint (1 thru 9)
// - state: color to paint index (0 = off/black, 1 = on/FG colorr)
void Gauge::paintIndex(char index, char state) {
  if (index > 5) Paint_SetMirroring(MIRROR_HORIZONTAL);
  switch (index) {
    case 1:
      if (state == INDEX_ON) {
        Paint_DrawImage(gImage_index_8on, 15, 149, 50, 40);
      }
      if (state == INDEX_OFF) {
        Paint_DrawImage(gImage_index_8off, 15, 149, 50, 40);
      }
      break;
    case 2:
      if (state == INDEX_ON) {
        Paint_DrawImage(gImage_index_9on, 4, 108, 51, 27);
      }
      if (state == INDEX_OFF) {
        Paint_DrawImage(gImage_index_9off, 4, 108, 51, 27);
      }
      break;
    case 3:
      if (state == INDEX_ON) {
        Paint_DrawImage(gImage_index_10on, 16, 55, 49, 39);
      }
      if (state == INDEX_OFF) {
        Paint_DrawImage(gImage_index_10off, 16, 55, 49, 39);
      }
      break;
    case 4:
      if (state == INDEX_ON) {
        Paint_DrawImage(gImage_index_11on, 53, 17, 40, 50);
      }
      if (state == INDEX_OFF) {
        Paint_DrawImage(gImage_index_11off, 53, 17, 40, 50);
      }
      break;
    case 5:
      if (state == INDEX_ON) {
        Paint_DrawImage(gImage_index_12on, 107, 5, 26, 52);
      }
      if (state == INDEX_OFF) {
        Paint_DrawImage(gImage_index_12off, 107, 5, 26, 52);
      }
      break;
    case 6:
      if (state == INDEX_ON) {
        Paint_DrawImage(gImage_index_11on, 53 - 1, 17, 40, 50);
      }
      if (state == INDEX_OFF) {
        Paint_DrawImage(gImage_index_11off, 53 - 1, 17, 40, 50);
      }
      break;
    case 7:
      if (state == INDEX_ON) {
        Paint_DrawImage(gImage_index_10on, 16 - 1, 55, 49, 39);
      }
      if (state == INDEX_OFF) {
        Paint_DrawImage(gImage_index_10off, 16 - 1, 55, 49, 39);
      }
      break;
    case 8:
      if (state == INDEX_ON) {
        Paint_DrawImage(gImage_index_9on, 4, 108, 51, 27);
      }
      if (state == INDEX_OFF) {
        Paint_DrawImage(gImage_index_9off, 4, 108, 51, 27);
      }
      break;
    case 9:
      if (state == INDEX_ON) {
        Paint_DrawImage(gImage_index_8on, 15, 149, 50, 40);
      }
      if (state == INDEX_OFF) {
        Paint_DrawImage(gImage_index_8off, 15, 149, 50, 40);
      }
      break;
  }
  Paint_SetMirroring(MIRROR_NONE);
  _gaugeState[(int)index - 1] = state;
}

void Gauge::paintIndices(char startIndex, char endIndex, char state) {
  for (int i = startIndex; i <= endIndex; i++) {
    paintIndex(i, state);
  }
}

void Gauge::paintIcon(char icon) {
  switch (icon) {
    case COOLANT_ICON:
      Paint_DrawImage(gImage_icon_Ctemp, 91, 177, 58, 52);
      break;
    case OIL_ICON:
      Paint_DrawImage(gImage_icon_oil, 79, 199, 82, 32);
      break;
    case TURBO_ICON:
      Paint_DrawImage(gImage_icon_turbo, 98, 194, 44, 40);
      break;
  }
}

void Gauge::paintUnit(char unit) {
  switch (unit) {
    case UNIT_CELCIUS:
      Paint_DrawImage(gImage_unit_celcius, 107, 166, 26, 24);
      break;
    case UNIT_PSI:
      Paint_DrawImage(gImage_unit_PSI, 107, 166, 26, 24);
      break;
  }
}

void Gauge::clearIcon() {
  // Paint_DrawRectangle(79, 166, 82+79, 68+166, ORANGE_BG, DOT_PIXEL_1X1, DRAW_FILL_FULL );
  Paint_DrawImage(gImage_icon_block, 79, 166, 82, 68);
}

void Gauge::clearNumber(){
  // Paint_DrawRectangle(86, 96+1, 86+67, 96+50, ORANGE_BG, DOT_PIXEL_1X1, DRAW_FILL_FULL );
  Paint_DrawImage(gImage_text_block, 86, 96, 67, 49);
}

void Gauge::getLimits(int lims[2]) {
  lims[0] = _limits.lowerLim;
  lims[1] = _limits.upperLim;
}

// Gauge.printDebugMsg
// Prints up to 3 rows of debug messages at the bottom of the screen
// Parameters:
// - s: String to print
// - row: row at bottom of screen to print on (0, 1, or 2)
void Gauge::printDebugMsg(String s, int row) {
  Paint_DrawString_EN(5, 245 + (25 * row), s.c_str(), &Font24, YELLOW, RED);
}
