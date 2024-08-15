/**************************************************************************
  File: GaugePainter.h
  Version: 2
  Author: Abeed Nazar
  Date: 08/14/2024

  Gauge Painter library header file
  
  This library implements helper functions to make displaying to the gauge
  more simple and modular. The GaugePainter library interfaces with the 
  Adafruit GFX library to display the RGB565 color bitmaps in bitmaps.h.

  Written with use of Adafruit GFX library

**************************************************************************/

#ifndef GAUGE_PAINTER_H
#define GAUGE_PAINTER_H

#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

#define ORANGE_FG 0xFA00 // HSL 16 100 49
#define ORANGE_BG 0x80A0 // HSL 11 100 25
#define INDEX_ON 1
#define INDEX_OFF 0
#define INDEX_CLEAR 2
#define NUM_INDICES 9

#define COOLANT_ICON 0
#define OIL_ICON 1
#define TURBO_ICON 2

#define UNIT_CELCIUS 0
#define UNIT_PSI 1

#define CTEMP_MIN 10
#define CTEMP_MAX 170

#define OTEMP_MIN 10
#define OTEMP_MAX 150

#define OPRESS_MIN 9
#define OPRESS_MAX 99

#define BPRESS_MIN 0
#define BPRESS_MAX 12

typedef enum
{
  COOLANT_TEMP,
  OIL_TEMP,
  OIL_PRESS,
  BOOST_PRESS,
  G_METER,
  TRIP_INSIGHTS
} GaugeType;

typedef struct
{
  int upperLim;
  int lowerLim;
} Limits;

class Gauge
{
  private:
    Adafruit_ST7789 tft;
    char _gaugeState[NUM_INDICES];
    GaugeType _gaugeType;
    Limits _limits;
  public:
    Gauge(int TFT_CS, int TFT_DC, int TFT_RST);
    void begin(int xSize, int ySize);
    void setType(GaugeType type);
    GaugeType getType();
    void paintGauge(int value);
    void paintValue(int value);
    void paintIndex(char index, char state);
    void paintIndices(char startIndex, char endIndex, char state);
    void paintIcon(char icon);
    void paintUnit(char unit);
    void clearIcon();
    void clearUnit();
    void clearNumber();
    void getLimits(int lims[2]);
    void printDebugMsg(String s, int row);
    void startupAnimation();
};

#endif // GAUGE_PAINTER_H
