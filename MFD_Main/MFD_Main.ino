/**************************************************************************
  File: MFD_Main.ino
  Author: Abeed Nazar
  Date: 25-May-2025

  Main code for driving the multifunction display

  Upon boot, goes into coolant temp display
  Connect to ELM327
  Poll all sensors of the ELM327 at 5Hz, update internal struct
  Update 
  Upon button press, switch to next display

  For debugging, demo mode will produce random numbers 
 **************************************************************************/

#include "GaugePainter.h"
#include <WiFi.h>
#include "ELMduino.h"

// GPIO pins for screen one-way SPI
#define CS        10
#define RST       37 // Or set to -1 and connect to Arduino RESET pin
#define DC        36

#define LED_R 7
#define LED_G 6
#define LED_B 5

#define BTN_PIN 4

#define BTN_1_ACTION 1   // how many ms to press button to switch gauges
#define BTN_2_ACTION 500 // how many ms to press button for secondary action (action is TBD)

#define INCREMENT_PID_STATE(PID) do { PID = PID_States((int)PID + 1); } while(0)
#define INCREMENT_GAUGE_TYPE(type) do { type = GaugeType( ((int)type + 1) % (int)GAUGE_TYPE_MAX); } while(0)

// enum to go through while collecting OBD data
typedef enum
{
  PID_COOLANT_TEMP,
  PID_OIL_TEMP,
  PID_OIL_PRESS,
  PID_FUEL_PRESS,
  PID_INTAKE_PRESS,
  PID_BARO_PRESS,
  PID_TRIP_TIME,
  PID_AIR_FUEL_RATIO,
  PID_MASS_AIRFLOW,
  PID_MAX_STATE,
} PID_States;

Gauge mainGauge(CS, DC, RST);
GaugeType type;
GaugeData testData;

const char* ssid = "WiFi_OBDII";
const char* password = "your-password";

IPAddress server(192, 168, 0, 10);
WiFiClient client;
ELM327 main_ELM327;

char debugPrintBuffer[40];

bool buttonPressed = false;

// ISR for button
void IRAM_ATTR RegButton() {
  if (digitalRead(BTN_PIN)) {
    buttonPressed = true;
  }
  else {
    buttonPressed = false;
  }
}

void setup(void) {

  // mainGauge.begin(240, 320);

  // Connecting to ELM327 WiFi
  // sprintf(debugPrintBuffer, "Connecting to \n%s", ssid);
  // mainGauge.printDebugMsg(String(debugPrintBuffer));

  Serial.begin(115200);

  // Connecting to ELM327 WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_AP);
  WiFi.begin(ssid);
  // WiFi.begin(ssid, password); //Use this line if your ELM327 has a password protected WiFi

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Connected to Wifi");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  if (client.connect(server, 35000))
    Serial.println("connected");
  else
  {
    Serial.println("connection failed");
    while(1);
  }
  // false for debug msg OFF, true for debug msg ON
  main_ELM327.begin(client, false, 2000);  

  pinMode(BTN_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BTN_PIN), RegButton, CHANGE);

  mainGauge.begin(240, 320);
  mainGauge.setType(GaugeType(GAUGE_TYPE_COOLANT_TEMP));

}

void loop() {
  static PID_States currPID = PID_COOLANT_TEMP;
  static unsigned long buttonTimer = 0; static bool startPress = false;
  static char RGB [3] = {0, 0, 0};
  

  switch (currPID){
      case PID_COOLANT_TEMP:
      {
        // float coolantTemp = main_ELM327.engineCoolantTemp();
        int coolantTemp = main_ELM327.processPID(SERVICE_01, 0x05, 1, 1, 1, 1);
        coolantTemp = coolantTemp - 40;
        
        if (main_ELM327.nb_rx_state == ELM_SUCCESS)
        {
          testData.CoolantTemp = coolantTemp;

          // Serial.print(currPID);
          // Serial.print(" | ");
          // Serial.print("coolant temp: ");
          // Serial.print(coolantTemp);
          // Serial.println(" deg C");
          INCREMENT_PID_STATE(currPID);
        }
        else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
        {
          main_ELM327.printError();
          INCREMENT_PID_STATE(currPID);
        }
      
        break;
      }
      case PID_OIL_TEMP:
      {
        // standard PID is not broadcast
        // float oilTemp = main_ELM327.oilTemp();
        // oil temperature sensor does not exist on the R56 mini, this is an approximation

        // according to reddit post https://www.reddit.com/r/MINI/comments/10ehcu1/torque_pro_r56_n18_n16_oil_pressure_logging/
        // use mode 0x22, PID 0x5822
        // formula is A*9/5-76
        // guess of 1 byte data length: A = 0 to 255 --> range of -76F to +383F?
        // at ambient temperature, getting 83 --> 73F = 23C. need live testing

        // request special PID for oil temperature
        // mode 0x22, PID 0x5822, expect 1 response, expect 1 byte
        int oilTemp = main_ELM327.processPID(0x22, 0x5822, 1, 1, 1, 1);
        // use integer division to process
        // oilTemp = (oilTemp * 9) / 5 - 76; // fahrenheit
        oilTemp = oilTemp - 60; // celcius
        if (main_ELM327.nb_rx_state == ELM_SUCCESS)
        {
          testData.OilTemp = oilTemp;

          // Serial.print(currPID);
          // Serial.print(" | ");
          // Serial.print("oil temp: ");
          // Serial.print(oilTemp);
          // Serial.println(" deg C");
          INCREMENT_PID_STATE(currPID);
        }
        else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
        {
          main_ELM327.printError();
          INCREMENT_PID_STATE(currPID);
        }

        break;
      }
      case PID_OIL_PRESS: 
      
      {
        // no standard PID for oil pressure
        // oil pressure sensor is a 1/0 switch on the R56 mini, this is an approximation

        // according to reddit post https://www.reddit.com/r/MINI/comments/10ehcu1/torque_pro_r56_n18_n16_oil_pressure_logging/
        // use mode 0x22, PID 0x586F
        // formula is ((A*256)+B)*(1/69)-BARO()
        // guess: A = 0 to 255, baro is range ??

        // request special PID for oil temperature
        // mode 0x22, PID 0x586F, expect 1 response, expect 1 byte?
        int oilPress = main_ELM327.processPID(0x22, 0x586F, 1, 1, 1, 1);
        // use integer division to process
        oilPress = ( (oilPress * 255) + testData.BaroPress) / 69 - testData.BaroPress; // what unit?
        if (main_ELM327.nb_rx_state == ELM_SUCCESS)
        {
          testData.OilPress = oilPress;

          // Serial.print(currPID);
          // Serial.print(" | ");
          // Serial.print("oil pressure: ");
          // Serial.println(oilPress);
          INCREMENT_PID_STATE(currPID);
        }
        else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
        {
          main_ELM327.printError();
          INCREMENT_PID_STATE(currPID);
        }

        break;
      }
      case PID_FUEL_PRESS:
      {
        // float fuel_rail_press = main_ELM327.fuelRailPressure();
        int fuel_rail_press = main_ELM327.processPID(SERVICE_01, 0x23, 1, 2, 1, 1);
        fuel_rail_press = 10 * (256 * fuel_rail_press) + testData.BaroPress;
        if (main_ELM327.nb_rx_state == ELM_SUCCESS)
        {
          testData.FuelPress = fuel_rail_press;

          // Serial.print(currPID);
          // Serial.print(" | ");
          // Serial.print("fuel rail pressure: ");
          // Serial.print(fuel_rail_press);
          // Serial.println(" KPa");
          INCREMENT_PID_STATE(currPID);
        }
        else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
        {
          main_ELM327.printError();
          INCREMENT_PID_STATE(currPID);
        }
        break;
      }
      case PID_INTAKE_PRESS:
      {
        // float MAP = main_ELM327.manifoldPressure();
        int MAP = main_ELM327.processPID(SERVICE_01, 0x0B, 1, 1, 1, 1);
        if (main_ELM327.nb_rx_state == ELM_SUCCESS)
        {
          testData.MAP = MAP;

          // Serial.print(currPID);
          // Serial.print(" | ");
          // Serial.print("manifold abs pressure: ");
          // Serial.print(MAP);
          // Serial.println(" kPa");
          INCREMENT_PID_STATE(currPID);
        }
        else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
        {
          main_ELM327.printError();
          INCREMENT_PID_STATE(currPID);
        }
        break;
      }
      case PID_BARO_PRESS:
      {
        // float baro_press = main_ELM327.absBaroPressure();
        int baro_press = main_ELM327.processPID(SERVICE_01, 0x33, 1, 1, 1, 1);
        if (main_ELM327.nb_rx_state == ELM_SUCCESS)
        {
          testData.BaroPress = baro_press;

          // Serial.print(currPID);
          // Serial.print(" | ");
          // Serial.print("atmospheric pressure: ");
          // Serial.print(baro_press);
          // Serial.println(" kPa");

          testData.BaroPress = baro_press;
          
          INCREMENT_PID_STATE(currPID);
        }
        else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
        {
          main_ELM327.printError();
          INCREMENT_PID_STATE(currPID);
        }
        break;
      }
      case PID_TRIP_TIME:
      {
        int run_time = main_ELM327.runTime();
        if (main_ELM327.nb_rx_state == ELM_SUCCESS)
        {
          testData.RunTime = run_time;

          // Serial.print(currPID);
          // Serial.print(" | ");
          // Serial.print("trip time: ");
          // Serial.print(run_time);
          // Serial.println(" sec");
          INCREMENT_PID_STATE(currPID);
        }
        else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
        {
          main_ELM327.printError();
          INCREMENT_PID_STATE(currPID);
        }
        break;
      }
      case PID_AIR_FUEL_RATIO:
      {
        float AFR = main_ELM327.commandedAirFuelRatio();
        // inst_fuel_rate = inst_fuel_rate * 100 / 255;
        // float inst_fuel_rate = main_ELM327.fuelRate();
        
        if (main_ELM327.nb_rx_state == ELM_SUCCESS)
        {
          testData.AFR = AFR;

          // Serial.print(currPID);
          // Serial.print(" | ");
          // Serial.print("AFR:");
          // Serial.println(AFR);

          INCREMENT_PID_STATE(currPID);
        }
        else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
        {
          main_ELM327.printError();
          INCREMENT_PID_STATE(currPID);
        }
        break;
      }
      case PID_MASS_AIRFLOW:
      {
        int MAF = main_ELM327.processPID(SERVICE_01, 0x10, 1, 2, 1, 1); // g/s
        MAF = ((256 * MAF) + testData.BaroPress) / 100;
        // int MAF = main_ELM327.mafRate();
        if (main_ELM327.nb_rx_state == ELM_SUCCESS)
        {
          testData.MAF = MAF;

          // Serial.print(currPID);
          // Serial.print(" | ");
          // Serial.print("MAF: ");
          // Serial.println(MAF);
          INCREMENT_PID_STATE(currPID);
        }
        else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
        {
          main_ELM327.printError();
          INCREMENT_PID_STATE(currPID);
        }
        break;
      }
      // got to last PID to check
      // now perform extra calculations and gauge drawing
      case PID_MAX_STATE:
      {
        /*
        calculation for instantaneous fuel consumption (gal/s):
        (MAF) g air   1 g fuel      1 dm^3 fuel   0.264172 US gal
        ----------- * ----------- * ----------- * ---------------
        1 s           (AFR) g air   820 g fuel    1 dm^3 fuel
        */
        float inst_fuel_rate = (testData.MAF * 0.264172) / (testData.AFR * 820);
        /*
        calculation for turbocharger boost pressure:
        MAP - baro pressure
        */
        float boost_press = testData.MAP - testData.BaroPress;

        // apply the correct color to the LED 
        Calculate_Color(0, RGB);
        Button_SetColor(RGB);

        // update all gauge data, gauge library handles repainting internally 
        mainGauge.updateGauge(testData);

        Serial.print("Inst fuel rate: ");
        Serial.print(inst_fuel_rate);
        Serial.print(" gal/s");
        Serial.println();
        Serial.print("Boost pressure: ");
        Serial.print(boost_press);
        Serial.print(" KPa");
        Serial.println();
        Serial.println("last PID, resetting");
        currPID = PID_States(0);
        break;
      }
  }
  
  // begin counting how long the button has been pressed once the ISR catches 
  // a button press event
  if (buttonPressed && !startPress){
    buttonTimer = millis();
    startPress = true;
    // Serial.println("Button pressed");
  }
  // stop timing how long the button press took after the ISR catches
  // a button de-press event
  if (!buttonPressed && startPress){
    // won't account for o'flow which happens after 50 days of continuous use
    unsigned long buttonInterval = millis() - buttonTimer; 
    startPress = false;
    // Serial.print("Button pressed for ");
    // Serial.print(buttonInterval);
    // Serial.println(" ms");

    // Short press: Switch gauges
    if (buttonInterval > BTN_1_ACTION && buttonInterval < BTN_2_ACTION){
      INCREMENT_GAUGE_TYPE(type);
      mainGauge.setType(type);

      Serial.print("Switching gauges to:");
      Serial.println(int(type));
    }
    // Long press: undefined action
    else {
      Serial.println("Secondary action");
    }
    
  }

}

// TODO
// create a fn which starts with a blue color for low temperature
// (i.e. <25C)
// and extinguishes the light at operating temperature
// (i.e. 80C)
// possibly lights orange/red at high temperature
// (i.e. 120C)
// hardcoded values for now
void Calculate_Color(int temperature, char colors[3]) {
  int redVal, greenVal, blueVal;
  redVal = 153;
  greenVal = 204;
  blueVal = 255;
  colors[0] = redVal;
  colors[1] = greenVal;
  colors[2] = blueVal;
}

void Button_SetColor(char RGB[3])
{
  analogWrite(LED_R, RGB[0]);
  analogWrite(LED_G, RGB[1]);
  analogWrite(LED_B, RGB[2]);
}
