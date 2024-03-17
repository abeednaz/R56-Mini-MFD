#include <SPI.h>
#include "LCD_Driver.h"
#include "GUI_Paint.h"
#include "image.h"

#include "GaugePainter.h"

// #define DEFAULT_PROGRAM
// #define GAUGE_TEST
// #define COLOR_TEST
#define GAUGE_LIB_TEST

// defined in library, don't redefine
#ifndef GAUGE_LIB_TEST
#define ORANGE_FG 0xFA00 // HSL 16 100 49
#define ORANGE_BG 0x80A0 // HSL 11 100 25
#endif // NOT GAUGE_LIB_TEST

#ifdef DEFAULT_PROGRAM
void setup()
{
  Config_Init();
  LCD_Init();
  LCD_Clear(0xffff);
  Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 0, WHITE);
  Paint_DrawString_EN(30, 10, "123", &Font24, YELLOW, RED);
  Paint_DrawString_EN(30, 34, "ABC", &Font24, BLUE, CYAN);
  
  Paint_DrawRectangle(125, 10, 225, 58, RED,  DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
  Paint_DrawLine(125, 10, 225, 58, MAGENTA,   DOT_PIXEL_2X2,LINE_STYLE_SOLID);
  Paint_DrawLine(225, 10, 125, 58, MAGENTA,   DOT_PIXEL_2X2,LINE_STYLE_SOLID);
  
  Paint_DrawCircle(150,100, 25, BLUE,   DOT_PIXEL_2X2,   DRAW_FILL_EMPTY);
  Paint_DrawCircle(180,100, 25, BLACK,  DOT_PIXEL_2X2,   DRAW_FILL_EMPTY);
  Paint_DrawCircle(210,100, 25, RED,    DOT_PIXEL_2X2,   DRAW_FILL_EMPTY);
  Paint_DrawCircle(165,125, 25, YELLOW, DOT_PIXEL_2X2,   DRAW_FILL_EMPTY);
  Paint_DrawCircle(195,125, 25, GREEN,  DOT_PIXEL_2X2,   DRAW_FILL_EMPTY);
  
  Paint_DrawImage(gImage_70X70, 20, 80, 70, 70); 
  //Paint_DrawFloatNum (5, 150 ,987.654321,4,  &Font20,    WHITE,   LIGHTGREEN);
  
}

#endif // DEFAULT_PROGRAM

#ifdef GAUGE_TEST

void setup()
{
  Config_Init();
  LCD_Init();
  LCD_Clear(BLACK);
  Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, ROTATE_180, WHITE);
  // Paint_DrawRectangle(125, 10, 225, 58, 0xD2C4,  DOT_PIXEL_2X2,DRAW_FILL_FULL);
  Paint_DrawCircle((LCD_WIDTH / 2) + 1, (LCD_WIDTH / 2) + 2, (LCD_WIDTH / 2) - 1, ORANGE_BG, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  // Paint_DrawRectangle(100, 100, 164, 164, ORANGE_FG,  DOT_PIXEL_2X2, DRAW_FILL_FULL);
  Paint_DrawImage(gImage_oil_icon, 78, 193, 84, 34); 
  Paint_DrawImage(gImage_index_8B, 15, 149, 50, 40); 
  Paint_DrawImage(gImage_index_9B, 4, 108, 51, 27); 
  Paint_DrawImage(gImage_index_10B, 16, 55, 49, 39); 
  Paint_DrawImage(gImage_index_11B, 53, 17, 40, 50); 
  Paint_DrawImage(gImage_index_12B, 107, 5, 26, 52); 

  Paint_SetMirroring(MIRROR_HORIZONTAL);
  Paint_DrawImage(gImage_index_11B, 53-1, 17, 40, 50); 
  Paint_DrawImage(gImage_index_10B, 16-1, 55, 49, 39); 
  Paint_DrawImage(gImage_index_9B, 4-1, 108, 51, 27); 
  Paint_DrawImage(gImage_index_8B, 15-1, 149, 50, 40); 

  delay(1000);
  Paint_SetMirroring(MIRROR_NONE);
  Paint_DrawImage(gImage_index_8O, 15, 149, 50, 40); 
  Paint_DrawImage(gImage_index_9O, 4, 108, 51, 27); 
  Paint_DrawImage(gImage_index_10O, 16, 55, 49, 39); 
  Paint_DrawImage(gImage_index_11O, 53, 17, 40, 50); 
  Paint_DrawImage(gImage_index_12O, 107, 5, 26, 52); 

  Paint_SetMirroring(MIRROR_HORIZONTAL);
  Paint_DrawImage(gImage_index_11O, 53-1, 17, 40, 50); 
  Paint_DrawImage(gImage_index_10O, 16-1, 55, 49, 39); 
  Paint_DrawImage(gImage_index_9O, 4-1, 108, 51, 27); 
  Paint_DrawImage(gImage_index_8O, 15-1, 149, 50, 40); 

  analogWrite(DEV_BL_PIN, 100);
  delay(1000);
  analogWrite(DEV_BL_PIN, 0);
  delay(1000);
  analogWrite(DEV_BL_PIN, 255);
  delay(1000);
  analogWrite(DEV_BL_PIN, 50);
  // Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 0, BLUE);
}

#endif // GAUGE_TEST

#ifdef GAUGE_LIB_TEST
void setup()
{
  Gauge g;
  g.begin();
  // for (int i = 1; i < 10; i++){
  //   if (i < 6) g.paintIndex(i, INDEX_ON);
  //   else g.paintIndex(i, INDEX_OFF);
  // } 
  g.paintIndices(1, 9, INDEX_OFF);
  g.paintIndices(1, 5, INDEX_ON);
  g.paintIndices(6, 9, INDEX_OFF);

}

#endif // GAUGE_LIB_TEST

#ifdef COLOR_TEST

void setup()
{
  Serial.begin(115200);
  Config_Init();
  LCD_Init();
  LCD_Clear(BLACK);
  Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, ROTATE_180, WHITE);
  // Paint_DrawRectangle(125, 10, 225, 58, 0xD2C4,  DOT_PIXEL_2X2,DRAW_FILL_FULL);
  LCD_Clear(ORANGE_BG);
  Paint_DrawCircle((LCD_WIDTH / 3) + 1, (LCD_WIDTH / 3) + 2, (LCD_WIDTH / 3) - 1, ORANGE_FG, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  
}

void loop()
{
  static int lastVal = 0;
  int backlightVal = Serial.parseInt();
  if (backlightVal > 255) backlightVal = 255;
  if (backlightVal < 0) backlightVal = 0;
  
  if (lastVal != backlightVal && backlightVal != 0) {
    Serial.println(backlightVal);
    analogWrite(DEV_BL_PIN, backlightVal);
  }

  lastVal = backlightVal;
}
#endif // COLOR_TEST


#ifndef COLOR_TEST
void loop()
{
  
}
#endif // NOT COLOR_TEST



/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
