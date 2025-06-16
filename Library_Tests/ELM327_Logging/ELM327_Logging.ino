/**************************************************************************
  File: ELM327_Logging.ino
  Author: Abeed Nazar
  Date: 14-Jun-2025

  No display, only serial port logging of all parameters of interest
  to help figure out PIDs/scaling
 **************************************************************************/

#include <WiFi.h>
#include "ELMduino.h"

#define INCREMENT_PID_STATE(PID) do { PID = PID_States((int)PID + 1); } while(0)
#define INCREMENT_GAUGE_TYPE(type) do { type = GaugeType( ((int)type + 1) % (int)GAUGE_TYPE_MAX); } while(0)

// ("timetag,sensor,service,PID,param_A,param_B,calc_value")
#define LOGGER_FORMAT "%ld,%s,0X%x,0X%x,%d,%d,%.2f"

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
  PID_RPM,
  PID_LOAD,
  PID_ODOMETER,
  PID_MAX_STATE,
} PID_States;

typedef enum
{
  GAUGE_TYPE_COOLANT_TEMP,
  GAUGE_TYPE_OIL_TEMP,
  GAUGE_TYPE_OIL_PRESS,
  GAUGE_TYPE_BOOST_PRESS,
  // G_METER,
  GAUGE_TYPE_TRIP_INSIGHTS,
  GAUGE_TYPE_MAX,
} GaugeType;

typedef struct
{
  // Main Gauge values
  // initalize them to 1 to avoid any div/0 errors
  int CoolantTemp = 0;
  int OilTemp = 0;
  int OilPress = 0;
  int BoostPress = 0;
  int BaroPress = 1;
  int MAF = 1;
  int MAP = 1;
  float AFR = 14.7;
  float RPM = 1;
  float EngLoad = 1;
  int RunTime = 0;
  int FuelPress = 0;
  int Odometer = 0;

  // Trip Insights statistics
  int DriveCycleTime_sec = 0;
  float DriveCycleDist_mi = 0;
  float DriveCycleFuelUsage = 0;
  float AvgFuelRate = 0;

  // IMU values (not yet implemented)
  float X_accel = 0; 
  float Y_accel = 0; 
  float Z_accel = 0;
} GaugeData;

GaugeData testData;

const char* ssid = "WiFi_OBDII";
const char* password = "your-password";

IPAddress server(192, 168, 0, 10);
WiFiClient client;
ELM327 main_ELM327;

char debugPrintBuffer[256];

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
  
  // header row for CSV logging
  Serial.println("timetag,sensor,service,PID,param_A,param_B,calc_value");

}

void loop() {
  static PID_States currPID = PID_COOLANT_TEMP;

  switch (currPID){
      case PID_COOLANT_TEMP:
      {
        // float coolantTemp = main_ELM327.engineCoolantTemp();
        int coolantTempRaw = main_ELM327.processPID(SERVICE_01, 0x05, 1, 1, 1, 1);
        int coolantTemp = coolantTempRaw - 40;
        
        if (main_ELM327.nb_rx_state == ELM_SUCCESS)
        {
          testData.CoolantTemp = coolantTemp;

          // Serial.print(currPID);
          // Serial.print(" | ");
          // Serial.print("coolant temp: ");
          // Serial.print(coolantTemp);
          // Serial.println(" deg C");

          sprintf(debugPrintBuffer, 
            LOGGER_FORMAT,
            millis(), "CoolantTemp", 0x01, 0x05, coolantTempRaw, 0, (float)coolantTemp);
          Serial.println(debugPrintBuffer);
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
        int oilTempRaw = main_ELM327.processPID(0x22, 0x5822, 1, 1, 1, 1);
        // use integer division to process
        // oilTemp = (oilTemp * 9) / 5 - 76; // fahrenheit
        int oilTemp = oilTempRaw - 60; // celcius
        if (main_ELM327.nb_rx_state == ELM_SUCCESS)
        {
          testData.OilTemp = oilTemp;

          // Serial.print(currPID);
          // Serial.print(" | ");
          // Serial.print("oil temp: ");
          // Serial.print(oilTemp);
          // Serial.println(" deg C");
          sprintf(debugPrintBuffer, 
            LOGGER_FORMAT,
            millis(), "OilTemp", 0x22, 0x5822, oilTempRaw, 0, (float)oilTemp);
          Serial.println(debugPrintBuffer);

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
        // mode 0x22, PID 0x586F, expect 1 response, expect 2 bytes...
        int oilPressRaw = main_ELM327.processPID(0x22, 0x586F, 2, 1, 1, 1);
        // bitmask A and B parameters?
        
        // use integer division to process
        int oilPress = ( (oilPressRaw * 255) + testData.BaroPress) / 69 - testData.BaroPress; // what unit?
        if (main_ELM327.nb_rx_state == ELM_SUCCESS)
        {
          testData.OilPress = oilPress;

          // Serial.print(currPID);
          // Serial.print(" | ");
          // Serial.print("oil pressure: ");
          // Serial.println(oilPress);
          sprintf(debugPrintBuffer, 
            LOGGER_FORMAT,
            millis(), "OilPress", 0x22, 0x586F, oilPressRaw, 0, (float)oilPress);
          Serial.println(debugPrintBuffer);

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
        int fuel_rail_press_raw = main_ELM327.processPID(SERVICE_01, 0x23, 2, 2, 1, 1);
        // bit masking to get 10(256A+B) from the output??
        int fuel_rail_press = 10 * (256 * fuel_rail_press_raw) + testData.BaroPress;
        if (main_ELM327.nb_rx_state == ELM_SUCCESS)
        {
          testData.FuelPress = fuel_rail_press;

          // Serial.print(currPID);
          // Serial.print(" | ");
          // Serial.print("fuel rail pressure: ");
          // Serial.print(fuel_rail_press);
          // Serial.println(" KPa");

          sprintf(debugPrintBuffer, 
            LOGGER_FORMAT,
            millis(), "FuelPress", 0x01, 0x23, fuel_rail_press_raw, 0, (float)fuel_rail_press);
          Serial.println(debugPrintBuffer);

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

          sprintf(debugPrintBuffer, 
            LOGGER_FORMAT,
            millis(), "MAP", 0x01, 0x0B, MAP, 0, (float)MAP);
          Serial.println(debugPrintBuffer);

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
        int baro_press_raw = main_ELM327.processPID(SERVICE_01, 0x33, 1, 1, 1, 1);
        float baro_press = main_ELM327.absBaroPressure();
        if (main_ELM327.nb_rx_state == ELM_SUCCESS)
        {
          testData.BaroPress = baro_press;

          // Serial.print(currPID);
          // Serial.print(" | ");
          // Serial.print("atmospheric pressure: ");
          // Serial.print(baro_press);
          // Serial.println(" kPa");

          testData.BaroPress = baro_press;

          sprintf(debugPrintBuffer, 
            LOGGER_FORMAT,
            millis(), "BaroPress", 0x01, 0x33, baro_press_raw, 0, (float)baro_press);
          Serial.println(debugPrintBuffer);
          
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

          sprintf(debugPrintBuffer, 
            LOGGER_FORMAT,
            millis(), "RunTime", 0x01, 0x1F, run_time, 0, (float)run_time);
          Serial.println(debugPrintBuffer);
          
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
        int AFR_raw = main_ELM327.processPID(SERVICE_01, 0x44, 1, 1, 1, 1);
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

          sprintf(debugPrintBuffer, 
            LOGGER_FORMAT,
            millis(), "AFR", 0x01, 0x44, AFR_raw, 0, (float)AFR);
          Serial.println(debugPrintBuffer);

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
        int MAFraw = main_ELM327.processPID(SERVICE_01, 0x10, 1, 2, 1, 1); // g/s
        int MAF = ((256 * MAFraw) + testData.BaroPress) / 100;
        // int MAF = main_ELM327.mafRate();
        if (main_ELM327.nb_rx_state == ELM_SUCCESS)
        {
          testData.MAF = MAF;

          // Serial.print(currPID);
          // Serial.print(" | ");
          // Serial.print("MAF: ");
          // Serial.println(MAF);

          sprintf(debugPrintBuffer, 
            LOGGER_FORMAT,
            millis(), "MAF", 0x01, 0x10, MAFraw, 0, (float)MAF);
          Serial.println(debugPrintBuffer);

          INCREMENT_PID_STATE(currPID);
        }
        else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
        {
          main_ELM327.printError();
          INCREMENT_PID_STATE(currPID);
        }
        break;
      }
      

      case PID_RPM:
      {
        float RPM = main_ELM327.rpm();
        
        if (main_ELM327.nb_rx_state == ELM_SUCCESS)
        {
          testData.RPM = RPM;

          // Serial.print(currPID);
          // Serial.print(" | ");
          // Serial.print("AFR:");
          // Serial.println(AFR);

          sprintf(debugPrintBuffer, 
            LOGGER_FORMAT,
            millis(), "RPM", 0x01, 0x0C, 0, 0, RPM);
          Serial.println(debugPrintBuffer);

          INCREMENT_PID_STATE(currPID);
        }
        else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
        {
          main_ELM327.printError();
          INCREMENT_PID_STATE(currPID);
        }
        break;
      }

      case PID_LOAD:
      {
        float engLoad = main_ELM327.engineLoad();
        
        if (main_ELM327.nb_rx_state == ELM_SUCCESS)
        {
          testData.EngLoad = engLoad;

          // Serial.print(currPID);
          // Serial.print(" | ");
          // Serial.print("AFR:");
          // Serial.println(AFR);

          sprintf(debugPrintBuffer, 
            LOGGER_FORMAT,
            millis(), "EngineLoad", 0x01, 0x04, 0, 0, engLoad);
          Serial.println(debugPrintBuffer);

          INCREMENT_PID_STATE(currPID);
        }
        else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
        {
          main_ELM327.printError();
          INCREMENT_PID_STATE(currPID);
        }
        break;
      }

      case PID_ODOMETER:
      {
        int ODO_raw = main_ELM327.processPID(SERVICE_01, 0xA6, 4, 1, 1, 1);
        // bitmask and scale?
        // ( A * 2^24 + B * 2^16 + C * 2^8 + D ) / 10
        int ODO = ODO_raw;
        
        if (main_ELM327.nb_rx_state == ELM_SUCCESS)
        {
          testData.Odometer = ODO;

          // Serial.print(currPID);
          // Serial.print(" | ");
          // Serial.print("AFR:");
          // Serial.println(AFR);

          sprintf(debugPrintBuffer, 
            LOGGER_FORMAT,
            millis(), "Odometer", 0x01, 0xA6, 0, 0, (float)ODO);
          Serial.println(debugPrintBuffer);

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
        int tempAFR = testData.AFR;
        if (testData.AFR == 0){
          tempAFR = 0xFFFF; // avoid div/0 error
        }
        float inst_fuel_rate = (testData.MAF * 0.264172) / (tempAFR * 820);
        /*
        calculation for current trip MPG:
        (avg fuel rate) gal   
        ------------------- * (run time) s
        1 sec
        */
        int tempRunTime = testData.RunTime;
        if (testData.RunTime == 0){
          tempRunTime = 0xFFFF; // avoid div/0 error
        }
        float avg_fuel_rate = (testData.AvgFuelRate * (tempRunTime-1) + inst_fuel_rate) / tempRunTime;
        testData.AvgFuelRate = avg_fuel_rate;
        /*
        calculation for turbocharger boost pressure:
        MAP - baro pressure
        */
        float boost_press = testData.MAP - testData.BaroPress;

        sprintf(debugPrintBuffer, 
          LOGGER_FORMAT,
          millis(), "InstFuelRate_custom", 0, 0, 0, 0, (float)inst_fuel_rate);
        Serial.println(debugPrintBuffer);

        sprintf(debugPrintBuffer, 
          LOGGER_FORMAT,
          millis(), "AvgFuelRate_custom", 0, 0, 0, 0, (float)avg_fuel_rate);
        Serial.println(debugPrintBuffer);

        sprintf(debugPrintBuffer, 
          LOGGER_FORMAT,
          millis(), "BoostPress_custom", 0, 0, 0, 0, (float)boost_press);
        Serial.println(debugPrintBuffer);

        currPID = PID_States(0);
        break;
      }
  }

}
