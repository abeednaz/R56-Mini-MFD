#include "Gauge.h"

/******************************************************************************
*                            MODULE-LEVEL VARIABLES                           *
******************************************************************************/

Gauge mainGauge;

// value to print
// static char value_str[8];

/********9o**********************************************************************
*                              HELPER FUNCTIONS                               *
******************************************************************************/


/******************************************************************************
*                                INIT FUNCTION                                *
******************************************************************************/

void setup() {

  Serial.begin(115200);
  Serial.println("Starting...");

  mainGauge.begin();
}


/******************************************************************************
*                                 MAIN DRIVER                                 *
******************************************************************************/
void loop() {
  static GaugeData data; static int i = 0; static GaugeType type = GAUGE_TYPE_OIL_TEMP;

  data.CoolantTemp = random(0,200);
  data.OilTemp = random(0,200);
  data.FuelPress = random(0,2000);
  data.OilPress = random(0,100);
  data.BoostPress = random(0,15);
  // update gauge icon
  i = i + 1;
  if (i == 20){
    type = (GaugeType)( ((int)type + 1) % 5 );
    mainGauge.setType(type);
    i = 0;
  }
  mainGauge.update(data);
  delay(100);
  
}
