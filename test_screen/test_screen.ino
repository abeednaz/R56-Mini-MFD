#include <SPI.h>
#include "LCD_Driver.h"
#include "GUI_Paint.h"
#include "image.h"

#define ORANGE_FG 0xFBE5
#define ORANGE_BG 0x8000

// #define DEFAULT_PROGRAM

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

#ifndef DEFAULT_PROGRAM

void setup()
{
  Config_Init();
  LCD_Init();
  LCD_Clear(BLACK);
  Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, ROTATE_180, WHITE);
  // Paint_DrawRectangle(125, 10, 225, 58, 0xD2C4,  DOT_PIXEL_2X2,DRAW_FILL_FULL);
  Paint_DrawCircle((LCD_WIDTH / 2) + 1, (LCD_WIDTH / 2) + 2, (LCD_WIDTH / 2) - 1, ORANGE_BG, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  // Paint_DrawRectangle(100, 100, 164, 164, ORANGE_FG,  DOT_PIXEL_2X2, DRAW_FILL_FULL);
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
  // Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 0, BLUE);
}

#endif // NOT DEFAULT_PROGRAM

void loop()
{
  
}



/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
