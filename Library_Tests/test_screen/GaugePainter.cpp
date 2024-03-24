/*
  File: GaugePainter.cpp
  Version: 1
  Author: Abeed Nazar
  Date: 3/17/2024

  Gauge Painter implementation file
  Written with use of WaveShare 2.0 inch LCD library
  Provides helper functions to display the gauge
*/

#include "GaugePainter.h"

#include <SPI.h>
#include "LCD_Driver.h"
#include "GUI_Paint.h"
#include "image.h"

// Gauge object constructor
// Parameters: none
// Returns: none
// Constructor for new Gauge object
Gauge::Gauge()
{

}

// Gauge.begin()
// Parameters: none
// Returns: none
// Begins new gauge
void Gauge::begin()
{
  for (int i = 0; i < NUM_INDICES; i++){
    _gaugeState[i] = INDEX_OFF;
  }
  // set the default gauge type
  _gaugeType = COOLANT_TEMP;
  Config_Init();
  LCD_Init();
  // LCD_Clear(BLACK);
  LCD_Clear(ORANGE_BG);
  Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, ROTATE_180, WHITE);
  // Paint_DrawCircle((LCD_WIDTH / 2) + 1, (LCD_WIDTH / 2) + 2, (LCD_WIDTH / 2) - 1, ORANGE_BG, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  Paint_SetMirroring(MIRROR_NONE);

}

// Gauge.paintIndex()
// Parameters:
// - index: index of the gauge to paint (1 thru 9)
// - state: color to paint index (0 = off/black, 1 = on/FG colorr)
void Gauge::paintIndex(char index, char state)
{
  if (index > 5) Paint_SetMirroring(MIRROR_HORIZONTAL);
  switch(index){
    case 1:
      if (state == INDEX_ON){
        Paint_DrawImage(gImage_index_8on, 15, 149, 50, 40); 
      }
      if (state == INDEX_OFF){
        Paint_DrawImage(gImage_index_8off, 15, 149, 50, 40); 
      }
      break;
    case 2:
      if (state == INDEX_ON){
        Paint_DrawImage(gImage_index_9on, 4, 108, 51, 27); 
      }
      if (state == INDEX_OFF){
        Paint_DrawImage(gImage_index_9off, 4, 108, 51, 27); 
      }
      break;
    case 3:
      if (state == INDEX_ON){
        Paint_DrawImage(gImage_index_10on, 16, 55, 49, 39); 
      }
      if (state == INDEX_OFF){
        Paint_DrawImage(gImage_index_10off, 16, 55, 49, 39); 
      }
      break;
    case 4:
      if (state == INDEX_ON){
        Paint_DrawImage(gImage_index_11on, 53, 17, 40, 50); 
      }
      if (state == INDEX_OFF){
        Paint_DrawImage(gImage_index_11off, 53, 17, 40, 50); 
      }
      break;
    case 5:
      if (state == INDEX_ON){
        Paint_DrawImage(gImage_index_12on, 107, 5, 26, 52); 
      }
      if (state == INDEX_OFF){
        Paint_DrawImage(gImage_index_12off, 107, 5, 26, 52); 
      }
      break;
    case 6:
      if (state == INDEX_ON){
        Paint_DrawImage(gImage_index_11on, 53-1, 17, 40, 50); 
      }
      if (state == INDEX_OFF){
        Paint_DrawImage(gImage_index_11off, 53-1, 17, 40, 50); 
      }
      break;
    case 7:
      if (state == INDEX_ON){
        Paint_DrawImage(gImage_index_10on, 16-1, 55, 49, 39); 
      }
      if (state == INDEX_OFF){
        Paint_DrawImage(gImage_index_10off, 16-1, 55, 49, 39); 
      }
      break;
    case 8:
      if (state == INDEX_ON){
        Paint_DrawImage(gImage_index_9on, 4, 108, 51, 27); 
      }
      if (state == INDEX_OFF){
        Paint_DrawImage(gImage_index_9off, 4, 108, 51, 27); 
      }
      break;
    case 9:
      if (state == INDEX_ON){
        Paint_DrawImage(gImage_index_8on, 15, 149, 50, 40); 
      }
      if (state == INDEX_OFF){
        Paint_DrawImage(gImage_index_8off, 15, 149, 50, 40); 
      }
      break;
  }
  Paint_SetMirroring(MIRROR_NONE);
  _gaugeState[(int)index - 1] = state;
}

void Gauge::paintIndices(char startIndex, char endIndex, char state)
{
  for (int i = startIndex; i <= endIndex; i++){
    paintIndex(i, state);
  }
}

void Gauge::paintIcon(char icon)
{
  switch(icon){
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

void Gauge::paintUnit(char unit)
{
  switch(unit){
    case UNIT_CELCIUS:
      Paint_DrawImage(gImage_unit_celcius, 107, 166, 26, 24); 
      break;
    case UNIT_PSI:
      Paint_DrawImage(gImage_unit_PSI, 107, 166, 26, 24); 
      break;
  }
}

void Gauge::clearUnit()
{
  // Paint_DrawRectangle(107, 166, 26, 24, ORANGE_BG, DOT_PIXEL_2X2, DRAW_FILL_FULL);
}

void Gauge::clearIcon()
{
  Paint_DrawImage(gImage_icon_block, 79, 166, 82, 68); 
}
