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

class Gauge
{
  private:
    char _gaugeState[NUM_INDICES];
  public:
    Gauge();
    void begin();
    void paintIndex(char index, char state);
    void paintIndices(char startIndex, char endIndex, char state);
    void paintIcon(char icon, char color);
};

#endif // GAUGE_PAINTER_H
