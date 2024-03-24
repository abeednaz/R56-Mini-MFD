/*
  File: GaugePainter.h
  Version: 1
  Author: Abeed Nazar
  Date: 3/17/2024

  Gauge Painter header file
  Written with use of WaveShare 2.0 inch LCD library
  Provides helper functions to display the gauge
*/

#ifndef GAUGE_PAINTER_H
#define GAUGE_PAINTER_H

#include <Arduino.h>

#define ORANGE_FG 0xFA00 // HSL 16 100 49
#define ORANGE_BG 0x80A0 // HSL 11 100 25
#define INDEX_ON 1
#define INDEX_OFF 0
#define NUM_INDICES 9

#define COOLANT_ICON 0
#define OIL_ICON 1
#define TURBO_ICON 2

#define UNIT_CELCIUS 0
#define UNIT_PSI 1

typedef enum
{
  COOLANT_TEMP,
  OIL_TEMP,
  OIL_PRESS,
  BOOST_PRESS,
  G_METER
} GaugeType;

class Gauge
{
  private:
    char _gaugeState[NUM_INDICES];
    GaugeType _gaugeType;
  public:
    Gauge();
    void begin();
    void paintIndex(char index, char state);
    void paintIndices(char startIndex, char endIndex, char state);
    void paintIcon(char icon);
    void paintUnit(char unit);
    void clearUnit();
    void clearIcon();
};

#endif // GAUGE_PAINTER_H
