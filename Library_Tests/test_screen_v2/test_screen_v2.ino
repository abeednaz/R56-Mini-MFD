/**************************************************************************
  File: test_screen_v2.ino
  Author: Abeed Nazar
  Date: 08-Aug-2024

  Testing with the Adafruit ST7789 library for increased speed
 **************************************************************************/

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include "bitmaps.h"

// SPI pins
#define TFT_CS        10
#define TFT_RST        37 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         36

// For 1.14", 1.3", 1.54", 1.69", and 2.0" TFT with ST7789:
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

float p = 3.1415926;

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

void setup(void) {
  Serial.begin(9600);
  Serial.print(F("Hello! ST77xx TFT Test"));

  // use this initializer (uncomment) if using a 2.0" 320x240 TFT:
  tft.init(240, 320);           // Init ST7789 320x240

  // SPI speed defaults to SPI_DEFAULT_FREQ defined in the library, you can override it here
  // Note that speed allowable depends on chip and quality of wiring, if you go too fast, you
  // may end up with a black screen some times, or all the time.
  // tft.setSPISpeed(40000000);

  Serial.println(F("Initialized"));

  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);
  time = millis() - time;

  Serial.println(time, DEC);
  delay(500);

  // large block of text
  // tft.fillScreen(ST77XX_BLACK);
  tft.fillScreen(0x80C0);
  // testdrawtext("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", ST77XX_WHITE);
  delay(1000);

  // tft print function!
  tft.drawRGBBitmap(15, 149, gImage_index_8off, 50, 40);
  tft.drawRGBBitmap(4, 108, gImage_index_9off, 51, 27);
  tft.drawRGBBitmap(16, 55, gImage_index_10off, 49, 39);
  tft.drawRGBBitmap(53, 17, gImage_index_11off, 40, 50);
  tft.drawRGBBitmap(107, 5, gImage_index_12off, 26, 52);
  tft.drawRGBBitmap(240-53-40, 17, gImage_index_1off, 40, 50);
  tft.drawRGBBitmap(240-16-49, 55, gImage_index_2off, 49, 39);
  tft.drawRGBBitmap(240-4-51, 108, gImage_index_3off, 51, 27);
  tft.drawRGBBitmap(240-15-50, 149, gImage_index_4off, 50, 40);

  // number print test
  // draw number 215
  int value = 215;
  int yStart = 96;
  int xStart[3] = { 0, 0, 0 };
  int digits[3] = { 0, 0, 0 };
  xStart[0] = 86;
  xStart[1] = 109;
  xStart[2] = 132;
  digits[0] = value / 100;
  digits[1] = (value % 100) / 10;
  digits[2] = value % 10;
  tft.drawRGBBitmap(xStart[0], yStart, gImage_digits[digits[0]], 21, 49);
  tft.drawRGBBitmap(xStart[1], yStart, gImage_digits[digits[1]], 21, 49);
  tft.drawRGBBitmap(xStart[2], yStart, gImage_digits[digits[2]], 21, 49);

  // clear the number after a second
  delay(1000);
  tft.drawRGBBitmap(86, 96, gImage_text_block, 67, 49);

  // draw a new number
  value = 89;
  xStart[0] = 98;
  xStart[1] = 121;
  digits[1] = value / 10;
  digits[2] = value % 10;
  tft.drawRGBBitmap(xStart[0], yStart, gImage_digits[digits[1]], 21, 49);
  tft.drawRGBBitmap(xStart[1], yStart, gImage_digits[digits[2]], 21, 49);

  // icon print test
  tft.drawRGBBitmap(91, 177, gImage_icon_ktemp, 58, 52);
  delay(1000);

  // clear and draw new icon
  tft.drawRGBBitmap(79, 166, gImage_icon_block, 82, 68);
  tft.drawRGBBitmap(79, 199, gImage_icon_oil, 82, 32);
  delay(1000);

  // clear and draw new icon
  tft.drawRGBBitmap(79, 166, gImage_icon_block, 82, 68);
  tft.drawRGBBitmap(98, 194, gImage_icon_boost, 44, 40);
  delay(1000);

  // units print test
  tft.drawRGBBitmap(107, 166, gImage_unit_celcius, 26, 24);
  delay(1000);
  tft.drawRGBBitmap(107, 166, gImage_unit_PSI, 26, 24);
  delay(1000);

  Serial.println("done");
  delay(1000);
}

void loop() {
  // tft.invertDisplay(false);
  tft.drawRGBBitmap(15, 149, gImage_index_8on, 50, 40);
  delay(100);
  tft.drawRGBBitmap(4 , 108, gImage_index_9on, 51, 27);
  delay(100);
  tft.drawRGBBitmap(16, 55, gImage_index_10on, 49, 39);
  delay(100);
  tft.drawRGBBitmap(53, 17, gImage_index_11on, 40, 50);
  delay(100);
  tft.drawRGBBitmap(107, 5, gImage_index_12on, 26, 52);
  delay(1000);
  tft.drawRGBBitmap(107, 5, gImage_index_12off, 26, 52);
  delay(100);
  tft.drawRGBBitmap(53, 17, gImage_index_11off, 40, 50);
  delay(100);
  tft.drawRGBBitmap(16, 55, gImage_index_10off, 49, 39);
  delay(100);
  tft.drawRGBBitmap(4 , 108, gImage_index_9off, 51, 27);
  delay(100);
  tft.drawRGBBitmap(15, 149, gImage_index_8off, 50, 40);
  delay(1000);
}

void testlines(uint16_t color) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, 0, x, tft.height()-1, color);
    delay(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, 0, tft.width()-1, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, 0, x, tft.height()-1, color);
    delay(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, 0, 0, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, tft.height()-1, x, 0, color);
    delay(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, tft.height()-1, tft.width()-1, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, x, 0, color);
    delay(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, 0, y, color);
    delay(0);
  }
}

void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

void testfastlines(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t y=0; y < tft.height(); y+=5) {
    tft.drawFastHLine(0, y, tft.width(), color1);
  }
  for (int16_t x=0; x < tft.width(); x+=5) {
    tft.drawFastVLine(x, 0, tft.height(), color2);
  }
}

void testdrawrects(uint16_t color) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color);
  }
}

void testfillrects(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=tft.width()-1; x > 6; x-=6) {
    tft.fillRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color1);
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color2);
  }
}

void testfillcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=radius; x < tft.width(); x+=radius*2) {
    for (int16_t y=radius; y < tft.height(); y+=radius*2) {
      tft.fillCircle(x, y, radius, color);
    }
  }
}

void testdrawcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=0; x < tft.width()+radius; x+=radius*2) {
    for (int16_t y=0; y < tft.height()+radius; y+=radius*2) {
      tft.drawCircle(x, y, radius, color);
    }
  }
}

void testtriangles() {
  tft.fillScreen(ST77XX_BLACK);
  uint16_t color = 0xF800;
  int t;
  int w = tft.width()/2;
  int x = tft.height()-1;
  int y = 0;
  int z = tft.width();
  for(t = 0 ; t <= 15; t++) {
    tft.drawTriangle(w, y, y, x, z, x, color);
    x-=4;
    y+=4;
    z-=4;
    color+=100;
  }
}

void testroundrects() {
  tft.fillScreen(ST77XX_BLACK);
  uint16_t color = 100;
  int i;
  int t;
  for(t = 0 ; t <= 4; t+=1) {
    int x = 0;
    int y = 0;
    int w = tft.width()-2;
    int h = tft.height()-2;
    for(i = 0 ; i <= 16; i+=1) {
      tft.drawRoundRect(x, y, w, h, 5, color);
      x+=2;
      y+=3;
      w-=4;
      h-=6;
      color+=1100;
    }
    color+=100;
  }
}

void tftPrintTest() {
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 30);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(3);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(4);
  tft.print(1234.567);
  delay(1500);
  tft.setCursor(0, 0);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(0);
  tft.println("Hello World!");
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_GREEN);
  tft.print(p, 6);
  tft.println(" Want pi?");
  tft.println(" ");
  tft.print(8675309, HEX); // print 8,675,309 out in HEX!
  tft.println(" Print HEX!");
  tft.println(" ");
  tft.setTextColor(ST77XX_WHITE);
  tft.println("Sketch has been");
  tft.println("running for: ");
  tft.setTextColor(ST77XX_MAGENTA);
  tft.print(millis() / 1000);
  tft.setTextColor(ST77XX_WHITE);
  tft.print(" seconds.");
}

void mediabuttons() {
  // play
  tft.fillScreen(ST77XX_BLACK);
  tft.fillRoundRect(25, 10, 78, 60, 8, ST77XX_WHITE);
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_RED);
  delay(500);
  // pause
  tft.fillRoundRect(25, 90, 78, 60, 8, ST77XX_WHITE);
  tft.fillRoundRect(39, 98, 20, 45, 5, ST77XX_GREEN);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST77XX_GREEN);
  delay(500);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_BLUE);
  delay(50);
  // pause color
  tft.fillRoundRect(39, 98, 20, 45, 5, ST77XX_RED);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST77XX_RED);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_GREEN);
}
