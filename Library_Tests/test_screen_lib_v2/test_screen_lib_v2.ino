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
GaugeData testData;

void setup(void) {
  myGauge.begin(240, 320);
  // myGauge.setType(GaugeType(OIL_TEMP));
  // myGauge.setType(GaugeType(OIL_PRESS));
  // delay(1000);
  // myGauge.setType(GaugeType(BOOST_PRESS));
  // delay(1000);
  // myGauge.setType(GaugeType(COOLANT_TEMP));
  // delay(1000);
  // myGauge.setType(GaugeType(OIL_TEMP));
  // delay(1000);
  

  // for(int i = 1; i < 250; i+=3){
  //   myGauge.paintValue(i);
  //   delay(100);
  // }
}

void loop() {

  myGauge.setType(GaugeType(OIL_TEMP));
  delay(100);
  for(int i = 5; i < 200; i+=3){
    testData.OilTemp = i;
    myGauge.updateGauge(testData);
    // delay(100);
  }
  for(int i = 160; i > 1; i-=3){
    testData.OilTemp = i;
    myGauge.updateGauge(testData);
    // delay(100);
  }

  // myGauge.setType(GaugeType(OIL_PRESS));
  // delay(1000);
  // for(int i = 9; i < 100; i+=3){
  //   testData.BoostPress = i;
  //   // testData.OilPress = i;
  //   myGauge.updateGauge(testData);
  //   delay(100);
  // }
  // for(int i = 100; i > 1; i-=3){
  //   testData.OilPress = i;
  //   myGauge.updateGauge(testData);
  //   delay(100);
  // }

  myGauge.setType(GaugeType(BOOST_PRESS));
  delay(100);
  for(int i = 0; i < 15; i+=1){
    testData.BoostPress = i;
    myGauge.updateGauge(testData);
    // delay(100);
  }
  for(int i = 15; i > 1; i-=3){
    testData.BoostPress = i;
    myGauge.updateGauge(testData);
    // delay(100);
  }

  // myGauge.setType(GaugeType(COOLANT_TEMP));
  // delay(1000);
  // for(int i = 0; i < 170; i+=3){
  //   testData.CoolantTemp = i;
  //   myGauge.updateGauge(testData);
  //   delay(100);
  // }
  // for(int i = 170; i > 1; i-=3){
  //   testData.CoolantTemp = i;
  //   myGauge.updateGauge(testData);
  //   delay(100);
  // }
  
  
  
}

