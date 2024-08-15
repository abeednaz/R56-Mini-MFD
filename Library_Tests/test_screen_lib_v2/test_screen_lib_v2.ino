/**************************************************************************
  File: test_screen_v2.ino
  Author: Abeed Nazar
  Date: 08-Aug-2024

  Testing with the Adafruit ST7789 library for increased speed
 **************************************************************************/

#include "GaugePainter.h"

#define CS        10
#define RST       37 // Or set to -1 and connect to Arduino RESET pin
#define DC        36

Gauge myGauge(CS, DC, RST);
GaugeType type;

void setup(void) {
  myGauge.begin(240, 320);
  myGauge.setType(GaugeType(COOLANT_TEMP));
  delay(1000);
  myGauge.setType(GaugeType(OIL_TEMP));
  delay(1000);
  myGauge.setType(GaugeType(OIL_PRESS));
  delay(1000);
  myGauge.setType(GaugeType(BOOST_PRESS));
  delay(1000);

  for(int i = 0; i < 250; i+=1){
    myGauge.paintValue(i);
    delay(50);
  }
}

void loop() {
  // for (char i = 1; i <= 9; i++){
  //   myGauge.paintIndex(i, INDEX_ON);
  //   delay(500);
  // }
  // delay(1000);
  // for (char i = 9; i >= 1; i--){
  //   myGauge.paintIndex(i, INDEX_OFF);
  //   delay(500);
  // }
  // delay(1000);
  
}

