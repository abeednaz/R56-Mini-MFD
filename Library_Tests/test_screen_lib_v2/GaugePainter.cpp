/**************************************************************************
  File: GaugePainter.h
  Version: 2
  Author: Abeed Nazar
  Date: 08/14/2024

  Gauge Painter implementation file
  
**************************************************************************/

#include "GaugePainter.h"

#include "bitmaps.h"

// array to reference max/min limits on each gauge 
const int lims[4][2] = { { CTEMP_MIN, CTEMP_MAX },
                         { OTEMP_MIN, OTEMP_MAX },
                         { OPRESS_MIN, OPRESS_MAX },
                         { BPRESS_MIN, BPRESS_MAX } };

// array to reference digits while printing
const uint16_t * const gImage_digits[] = {
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
Gauge::Gauge(int TFT_CS, int TFT_DC, int TFT_RST) : tft(TFT_CS, TFT_DC, TFT_RST) {
  tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
}

// Gauge.begin()
// Parameters: pins
// Returns: none
// Begins new gauge
void Gauge::begin(int xSize, int ySize) {
  for (int i = 0; i < NUM_INDICES; i++) {
    _gaugeState[i] = INDEX_OFF;
  }

  // initialize screen with size
  tft.init(xSize, ySize);
  // optional SPI speed setting (MHz)
  // tft.setSPISpeed(40000000);

  tft.fillScreen(ORANGE_BG);
  startupAnimation();
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
    case TRIP_INSIGHTS:
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
    tft.drawRGBBitmap(xStart[0], yStart, gImage_digits[digits[2]], 21, 49);
  }
  // double digit number
  if (value >= 10 && value < 100) {
    xStart[0] = 98;
    xStart[1] = 121;
    digits[1] = value / 10;
    digits[2] = value % 10;
    tft.drawRGBBitmap(xStart[0], yStart, gImage_digits[digits[1]], 21, 49);
    tft.drawRGBBitmap(xStart[1], yStart, gImage_digits[digits[2]], 21, 49);
  }
  // triple digit number
  if (value >= 100) {
    xStart[0] = 86;
    xStart[1] = 109;
    xStart[2] = 132;
    digits[0] = value / 100;
    digits[1] = (value % 100) / 10;
    digits[2] = value % 10;
    tft.drawRGBBitmap(xStart[0], yStart, gImage_digits[digits[0]], 21, 49);
    tft.drawRGBBitmap(xStart[1], yStart, gImage_digits[digits[1]], 21, 49);
    tft.drawRGBBitmap(xStart[2], yStart, gImage_digits[digits[2]], 21, 49);
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
  switch (index) {
    case 1:
      if (state == INDEX_ON) {
        tft.drawRGBBitmap(15, 149, gImage_index_8on, 50, 40);
      }
      if (state == INDEX_OFF) {
        tft.drawRGBBitmap(15, 149, gImage_index_8off, 50, 40);
      }
      if (state == INDEX_CLEAR) {
        tft.fillRect(15, 149, 50, 40, ORANGE_BG);
      }
      break;
    case 2:
      if (state == INDEX_ON) {
        tft.drawRGBBitmap(4, 108, gImage_index_9on, 51, 27);
      }
      if (state == INDEX_OFF) {
        tft.drawRGBBitmap(4, 108, gImage_index_9off, 51, 27);
      }
      if (state == INDEX_CLEAR) {
        tft.fillRect(4, 108, 51, 27, ORANGE_BG);
      }
      break;
    case 3:
      if (state == INDEX_ON) {
        tft.drawRGBBitmap(16, 55, gImage_index_10on, 49, 39);
      }
      if (state == INDEX_OFF) {
        tft.drawRGBBitmap(16, 55, gImage_index_10off, 49, 39);
      }
      if (state == INDEX_CLEAR) {
        tft.fillRect(16, 55, 49, 39, ORANGE_BG);
      }
      break;
    case 4:
      if (state == INDEX_ON) {
        tft.drawRGBBitmap(53, 17, gImage_index_11on, 40, 50);
      }
      if (state == INDEX_OFF) {
        tft.drawRGBBitmap(53, 17, gImage_index_11off, 40, 50);
      }
      if (state == INDEX_CLEAR) {
        tft.fillRect(53, 17, 40, 50, ORANGE_BG);
      }
      break;
    case 5:
      if (state == INDEX_ON) {
        tft.drawRGBBitmap(107, 5, gImage_index_12on, 26, 52);
      }
      if (state == INDEX_OFF) {
        tft.drawRGBBitmap(107, 5, gImage_index_12off, 26, 52);
      }
      if (state == INDEX_CLEAR) {
        tft.fillRect(107, 5, 26, 52, ORANGE_BG);
      }
      break;
    case 6:
      if (state == INDEX_ON) {
        tft.drawRGBBitmap(240-53-40, 17, gImage_index_1on, 40, 50);
      }
      if (state == INDEX_OFF) {
        tft.drawRGBBitmap(240-53-40, 17, gImage_index_1off, 40, 50);
      }
      if (state == INDEX_CLEAR) {
        tft.fillRect(240-53-40, 17, 40, 50, ORANGE_BG);
      }
      break;
    case 7:
      if (state == INDEX_ON) {
        tft.drawRGBBitmap(240-16-49, 55, gImage_index_2on, 49, 39);
      }
      if (state == INDEX_OFF) {
        tft.drawRGBBitmap(240-16-49, 55, gImage_index_2off, 49, 39);
      }
      if (state == INDEX_CLEAR) {
        tft.fillRect(240-16-55, 55, 49, 39, ORANGE_BG);
      }
      break;
    case 8:
      if (state == INDEX_ON) {
        tft.drawRGBBitmap(240-4-51, 108, gImage_index_3on, 51, 27);
      }
      if (state == INDEX_OFF) {
        tft.drawRGBBitmap(240-4-51, 108, gImage_index_3off, 51, 27);
      }
      if (state == INDEX_CLEAR) {
        tft.fillRect(240-4-51, 108, 51, 27, ORANGE_BG);
      }
      break;
    case 9:
      if (state == INDEX_ON) {
        tft.drawRGBBitmap(240-15-50, 149, gImage_index_4on, 50, 40);
      }
      if (state == INDEX_OFF) {
        tft.drawRGBBitmap(240-15-50, 149, gImage_index_4off, 50, 40);
      }
      if (state == INDEX_CLEAR) {
        tft.fillRect(240-15-50, 149, 50, 40, ORANGE_BG);
      }
      break;
  }
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
      clearIcon();
      tft.drawRGBBitmap(91, 177, gImage_icon_ktemp, 58, 52);
      break;
    case OIL_ICON:
      clearIcon();
      tft.drawRGBBitmap(79, 199, gImage_icon_oil, 82, 32);
      break;
    case TURBO_ICON:
      clearIcon();
      tft.drawRGBBitmap(98, 194, gImage_icon_boost, 44, 40);
      break;
  }
}

void Gauge::paintUnit(char unit) {
  switch (unit) {
    case UNIT_CELCIUS:
      clearUnit();
      tft.drawRGBBitmap(107, 166, gImage_unit_celcius, 26, 24);
      break;
    case UNIT_PSI:
      clearUnit();
      tft.drawRGBBitmap(107, 166, gImage_unit_PSI, 26, 24);
      break;
  }
}

void Gauge::clearIcon() {
  tft.fillRect(79, 166, 82, 68, ORANGE_BG);
}

void Gauge::clearUnit() {
  tft.fillRect(107, 166, 26, 24, ORANGE_BG);
}

void Gauge::clearNumber(){
  tft.fillRect(86, 96, 67, 49, ORANGE_BG);
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
  // Paint_DrawString_EN(5, 245 + (25 * row), s.c_str(), &Font24, YELLOW, RED);
}

// Gauge.startupAnimation()
// Sweeps the gauge indices on then off
void Gauge::startupAnimation(){

  paintIndices(1, 9, INDEX_OFF);
  for (char i = 1; i <= 9; i++){
    paintIndex(i, INDEX_ON);
    delay(75);
  }
  for (char i = 9; i >= 1; i--){
    paintIndex(i, INDEX_OFF);
    delay(75);
  }
}
