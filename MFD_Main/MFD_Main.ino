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

#define INCREMENT_PID_STATE(PID) do { PID = PID_States((int)PID + 1); } while(0)

// GPIO pin for button
// #define ...

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

Gauge myGauge(CS, DC, RST);
GaugeType type;
GaugeData testData;

const char* ssid = "WiFi_OBDII";
const char* password = "your-password";

IPAddress server(192, 168, 0, 10);
WiFiClient client;
ELM327 main_ELM327;

char debugPrintBuffer[40];



void setup(void) {

  // myGauge.begin(240, 320);

  // Connecting to ELM327 WiFi
  // sprintf(debugPrintBuffer, "Connecting to \n%s", ssid);
  // myGauge.printDebugMsg(String(debugPrintBuffer));

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

  uint32_t pids = main_ELM327.supportedPIDs_1_20();

  if (main_ELM327.nb_rx_state == ELM_SUCCESS)
  {
      Serial.print("Supported PIDS: "); Serial.println(pids);
      delay(10000);
  }
  else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
  {
      main_ELM327.printError();
  }    

}

void loop() {
  static PID_States currPID = PID_COOLANT_TEMP;

  switch (currPID){
      case PID_COOLANT_TEMP:
      {
        // float coolantTemp = main_ELM327.engineCoolantTemp();
        int coolantTemp = main_ELM327.processPID(SERVICE_01, 0x05, 1, 1, 1, 1);
        coolantTemp = coolantTemp - 40;
        
        if (main_ELM327.nb_rx_state == ELM_SUCCESS)
        {
          Serial.print(currPID);
          Serial.print(" | ");
          Serial.print("coolant temp: ");
          Serial.print(coolantTemp);
          Serial.println(" deg C");
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
          Serial.print(currPID);
          Serial.print(" | ");
          Serial.print("oil temp: ");
          Serial.print(oilTemp);
          Serial.println(" deg C");
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
          Serial.print(currPID);
          Serial.print(" | ");
          Serial.print("oil pressure: ");
          Serial.println(oilPress);
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
          Serial.print(currPID);
          Serial.print(" | ");
          Serial.print("fuel rail pressure: ");
          Serial.print(fuel_rail_press);
          Serial.println(" KPa");
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
          Serial.print(currPID);
          Serial.print(" | ");
          Serial.print("manifold abs pressure: ");
          Serial.print(MAP);
          Serial.println(" kPa");
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
          Serial.print(currPID);
          Serial.print(" | ");
          Serial.print("atmospheric pressure: ");
          Serial.print(baro_press);
          Serial.println(" kPa");

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
          Serial.print(currPID);
          Serial.print(" | ");
          Serial.print("trip time: ");
          Serial.print(run_time);
          Serial.println(" sec");
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
        int inst_fuel_rate = ((testData.MAF * 3600) / (AFR * 820)) / 10 ; // L/h
        if (main_ELM327.nb_rx_state == ELM_SUCCESS)
        {
          Serial.print(currPID);
          Serial.print(" | ");
          Serial.print("AFR:");
          Serial.print(AFR);
          Serial.print(" --> ");
          Serial.print("inst fuel economy: ");
          Serial.print(inst_fuel_rate);
          Serial.println(" L/hr");
          INCREMENT_PID_STATE(currPID);
        }
        else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
        {
          main_ELM327.printError();
          INCREMENT_PID_STATE(currPID);
        }
        break;
      }
      case PID_MASS_AIRFLOW: // L/h --> convert to mpg, integrate over trip time to get trip MPG
      {
        // int inst_fuel_rate = main_ELM327.processPID(0x01, 0x5E, 1, 2, 1, 1);
        int MAF = main_ELM327.mafRate();
        // inst_fuel_rate = inst_fuel_rate * 100 / 255;
        // float inst_fuel_rate = main_ELM327.fuelRate();
        if (main_ELM327.nb_rx_state == ELM_SUCCESS)
        {
          testData.MAF = MAF;
          Serial.print(currPID);
          Serial.print(" | ");
          Serial.print("MAF: ");
          Serial.println(MAF);
          INCREMENT_PID_STATE(currPID);
        }
        else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
        {
          main_ELM327.printError();
          INCREMENT_PID_STATE(currPID);
        }
        break;
      }
      // reset PID to check back to beginning if the last PID was reached
      case PID_MAX_STATE:
      {
        Serial.println("last PID, resetting");
        currPID = PID_States(0);
        break;
      }
  }
    
}

/*
PIDs to use:
- 0x67 - Coolant temperature
- 0x5C - Oil temperature

- 0x6F - Turbocharger boost pressure
OR
- 0x87 Intake MAP 


0x7F - Engine runtime

*/

/*
GetBoostPress()
Inputs
- ELM327 object
Outputs
- Boost pressure as a fn of manifold and atmospheric pressure.

See if this is possible with 
*/
int GetBoostPress(ELM327 main_ELM327){
  int manifoldAbsPress = main_ELM327.manifoldPressure();
  int baroPress = main_ELM327.absBaroPressure();
  return manifoldAbsPress - baroPress;
}

int GetOilPress(ELM327 main_ELM327){
  return 0;
}
