// SET_LOOP_TASK_STACK_SIZE(1024 * 16); 

// #include "Gauge.h"

#include <WiFi.h>
#include "ELMduino.h"

/******************************************************************************
*                                   DEFINES                                   *
******************************************************************************/
#define BTN_PIN 18
#define BRIGHTNESS_SENSOR_PIN 17

#define BTN_1_ACTION 1   // how many ms to press button to switch gauges
#define BTN_2_ACTION 500 // how many ms to press button for secondary action (action is TBD)

#define BRIGHTNESS_LO 155 // screen brightness setting
#define BRIGHTNESS_HI 255
#define BRIGHTNESS_LO_BOUND 1000 // ambient light sensor value
#define BRIGHTNESS_HYST 200 // hysteresis value for light sensor
#define BRIGHTNESS_INTVL (1 * 1000) // when to check ambient light level (ms)

#define WIFI_SSID "WiFi_OBDII"
#define WIFI_PW ""

#define RX_PASS 0
#define RX_FAIL 1
#define RX_NONE 2

#define HPA_TO_PSI 0.0145038
#define MPA_TO_PSI 145.037738
#define L_TO_GAL 0.264172

/******************************************************************************
*                                  TYPEDEFS                                   *
******************************************************************************/
// enum to go through while collecting OBD data
typedef enum
{
  PID_OIL_TEMP,
  PID_COOLANT_TEMP,
  // PID_RANGE,
  PID_AMBIENT_PRESS,
  PID_BOOST_PRESS,
  PID_OIL_PRESS,
  PID_FUEL_PRESS,
  PID_BRAKE_PRESS,
  PID_BRAKE_PRESS2,
  PID_ENGINE_SPEED,
  PID_GEAR,
  PID_BATTERY_VOLTAGE,
  PID_AFR_TARGET,
  PID_AFR_ACTUAL,
  PID_SPEED,
  PID_TRIP_TIME,
  PID_FUEL_CONS,
  PID_MAP,
  PID_IAT,
  PID_MAF,
  PID_MAX_STATE,
} PID_States;

/******************************************************************************
*                            MODULE-LEVEL VARIABLES                           *
******************************************************************************/
// static Gauge mainGauge;
static ELM327 main_ELM327;
static bool buttonPressed = false;
// static GaugeData data; 
static IPAddress server(192, 168, 0, 10);
static WiFiClient client;

/******************************************************************************
*                              HELPER FUNCTIONS                               *
******************************************************************************/
// ISR for button
void IRAM_ATTR RegButton() {
  if (digitalRead(BTN_PIN)) {
    buttonPressed = true;
  }
  else {
    buttonPressed = false;
  }
}

int CheckScannerState(ELM327 scanner)
{
  if (scanner.nb_rx_state == ELM_SUCCESS)
  {
    return RX_PASS;
  }
  else if (scanner.nb_rx_state != ELM_GETTING_MSG)
  {
    return RX_FAIL;
  }
  return RX_NONE;
}

/******************************************************************************
*                                    MACROS                                   *
******************************************************************************/
#define INCREMENT_PID_STATE(PID) do { \
  PID = PID_States((int)PID + 1); \
  } while(0)

#define MSG_RECEIVED(scanner) (scanner.nb_rx_state == ELM_SUCCESS)
#define MSG_NOT_RECEIVED(scanner) (scanner.nb_rx_state != ELM_GETTING_MSG)

/******************************************************************************
*                                INIT FUNCTION                                *
******************************************************************************/

void setup() {
  // SET_LOOP_TASK_STACK_SIZE(1024 * 64); 

  // IPAddress server(192, 168, 0, 10);
  // WiFiClient client;

  Serial.begin(115200);
  Serial.println("Starting...");

  // initialize gauge library
  // mainGauge.begin();
  // mainGauge.update(data);

  // configure ISR for button
  pinMode(BTN_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BTN_PIN), RegButton, CHANGE);

  // setup wifi for ELM327
  WiFi.mode(WIFI_AP);
  WiFi.begin(WIFI_SSID);
  // WiFi.begin(WIFI_SSID, WIFI_PW);

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
  
  Serial.printf("timetag,sensor,service,PID,raw_value,proc_value,unit,\n");

  // mainGauge.begin();
}


/******************************************************************************
*                                 MAIN DRIVER                                 *
******************************************************************************/
void loop() {
  static PID_States currPID = PID_OIL_TEMP;
  static float latestAmbientPressure_PSI = 14.7; 
  static double trip_dist_mi = 0;
  static double start_fuel_gal = 0;

  switch (currPID){
    
    case PID_OIL_TEMP:
    {
      uint8_t service = 0x22; uint16_t PID = 0x5822;
      const char* sensor = "OilTemp"; const char* unit = "degC";
      int rawVal = main_ELM327.processPID(service, PID, 1, 1, 1, 1);
      float processedVal = rawVal - 60;
      if (main_ELM327.nb_rx_state == ELM_SUCCESS)
      {
        Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
        INCREMENT_PID_STATE(currPID);
      }
      else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
      {
        // main_ELM327.printError();
        Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
        INCREMENT_PID_STATE(currPID);
      }

      break;
    }
    case PID_COOLANT_TEMP:
    {
      uint8_t service = 0x22; uint16_t PID = 0x4300;
      const char* sensor = "CoolantTemp"; const char* unit = "degC";
      int rawVal = main_ELM327.processPID(service, PID, 1, 1, 1, 1);
      float processedVal = 0.75 * rawVal - 48;
      if (main_ELM327.nb_rx_state == ELM_SUCCESS)
      {
        Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
        INCREMENT_PID_STATE(currPID);
      }
      else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
      {
        // main_ELM327.printError();
        Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
        INCREMENT_PID_STATE(currPID);
      }
      // int scannerState = CheckScannerState(main_ELM327);
      // if (scannerState == RX_PASS){
      //   Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
      //   INCREMENT_PID_STATE(currPID);
      // }
      // else {
      //   Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
      //   INCREMENT_PID_STATE(currPID);
      // }
      break;
    }
    case PID_AMBIENT_PRESS:
    {
      uint8_t service = 0x22; uint16_t PID = 0x5834;
      const char* sensor = "AmbientPressure"; const char* unit = "hPa";
      float processedVal = main_ELM327.processPID(service, PID, 1, 1, (0.0390625 * HPA_TO_PSI), 0);
      
      if (main_ELM327.nb_rx_state == ELM_SUCCESS)
      {
        latestAmbientPressure_PSI = processedVal;
        Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, 9999, processedVal, unit);
        INCREMENT_PID_STATE(currPID);
      }
      else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
      {
        // main_ELM327.printError();
        Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
        INCREMENT_PID_STATE(currPID);
      }
      break;
    }
    case PID_BOOST_PRESS:
    {
      uint8_t service = 0x22; uint16_t PID = 0x4205;
      const char* sensor = "BoostPressure"; const char* unit = "hPa";
      float processedVal  = main_ELM327.processPID(service, PID, 1, 1, (0.078125 * HPA_TO_PSI), 0) - latestAmbientPressure_PSI;
      
      if (main_ELM327.nb_rx_state == ELM_SUCCESS)
      {
        Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, 9999, processedVal, unit);
        INCREMENT_PID_STATE(currPID);
      }
      else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
      {
        // main_ELM327.printError();
        Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
        INCREMENT_PID_STATE(currPID);
      }
      break;
    }
    case PID_OIL_PRESS:
    {
      uint8_t service = 0x22; uint16_t PID = 0x586F;
      const char* sensor = "OilPressure"; const char* unit = "hPa";
      float processedVal = main_ELM327.processPID(service, PID, 1, 1, HPA_TO_PSI, 0) - latestAmbientPressure_PSI;
      
      if (main_ELM327.nb_rx_state == ELM_SUCCESS)
      {
        Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, 9999, processedVal, unit);
        INCREMENT_PID_STATE(currPID);
      }
      else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
      {
        // main_ELM327.printError();
        Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
        INCREMENT_PID_STATE(currPID);
      }
      // int scannerState = CheckScannerState(main_ELM327);
      // if (scannerState == RX_PASS){
        
      //   Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
      //   INCREMENT_PID_STATE(currPID);
      // }
      // else {
      //   Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
      //   INCREMENT_PID_STATE(currPID);
      // }
      break;
    }
    case PID_FUEL_PRESS:
    {
      uint8_t service = 0x22; uint16_t PID = 0x58EF;
      const char* sensor = "FuelRailPress(Filtered)"; const char* unit = "MPa";
      float processedVal = main_ELM327.processPID(service, PID, 1, 1, (0.0005 * MPA_TO_PSI), 0) - latestAmbientPressure_PSI;
      
      if (main_ELM327.nb_rx_state == ELM_SUCCESS)
      {
        Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, 9999, processedVal, unit);
        INCREMENT_PID_STATE(currPID);
      }
      else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
      {
        // main_ELM327.printError();
        Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
        INCREMENT_PID_STATE(currPID);
      }
      // int scannerState = CheckScannerState(main_ELM327);
      // if (scannerState == RX_PASS){
        
      //   Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
      //   INCREMENT_PID_STATE(currPID);
      // }
      // else {
      //   Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
      //   INCREMENT_PID_STATE(currPID);
      // }
      break;
    }
    case PID_BRAKE_PRESS:
    {
      uint8_t service = 0x22; uint16_t PID = 0x58B7;
      const char* sensor = "BrakePressure"; const char* unit = "hPa";
      int rawVal = main_ELM327.processPID(service, PID, 1, 1, 1, 1);
      float processedVal = rawVal;
      if (main_ELM327.nb_rx_state == ELM_SUCCESS)
      {
        Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
        INCREMENT_PID_STATE(currPID);
      }
      else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
      {
        // main_ELM327.printError();
        Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
        INCREMENT_PID_STATE(currPID);
      }
      // int scannerState = CheckScannerState(main_ELM327);
      // if (scannerState == RX_PASS){
        
      //   Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
      //   INCREMENT_PID_STATE(currPID);
      // }
      // else {
      //   Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
      //   INCREMENT_PID_STATE(currPID);
      // }
      break;
    }
    case PID_BRAKE_PRESS2:
    {
      uint8_t service = 0x22; uint16_t PID = 0x58B7;
      const char* sensor = "AmbientPressMinusBrakeBoosterPress"; const char* unit = "hPa";
      int rawVal = main_ELM327.processPID(service, PID, 1, 1, 1, 1);
      float processedVal = 0.0390625 * rawVal;
      if (main_ELM327.nb_rx_state == ELM_SUCCESS)
      {
        Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
        INCREMENT_PID_STATE(currPID);
      }
      else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
      {
        // main_ELM327.printError();
        Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
        INCREMENT_PID_STATE(currPID);
      }
      // int scannerState = CheckScannerState(main_ELM327);
      // if (scannerState == RX_PASS){
        
      //   Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
      //   INCREMENT_PID_STATE(currPID);
      // }
      // else {
      //   Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
      //   INCREMENT_PID_STATE(currPID);
      // }
      break;
    }
    // case PID_RANGE:
    // {
    //   uint8_t service = 0x22; uint16_t PID = 0x4411;
    //   const char* sensor = "Range"; const char* unit = "km?";
    //   int rawVal = main_ELM327.processPID(service, PID, 1, 1, 1, 1);
    //   float processedVal = 10 * rawVal;
    //   if (main_ELM327.nb_rx_state == ELM_SUCCESS)
    //   {
    //     Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
    //     INCREMENT_PID_STATE(currPID);
    //   }
    //   else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
    //   {
    //     // main_ELM327.printError();
    //     Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
    //     INCREMENT_PID_STATE(currPID);
    //   }
    //   // int scannerState = CheckScannerState(main_ELM327);
    //   // if (scannerState == RX_PASS){
        
    //   //   Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
    //   //   INCREMENT_PID_STATE(currPID);
    //   // }
    //   // else {
    //   //   Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
    //   //   INCREMENT_PID_STATE(currPID);
    //   // }
    //   break;
    // }
    
    case PID_ENGINE_SPEED:
    {
      uint8_t service = 0x22; uint16_t PID = 0x4807;
      const char* sensor = "EngineSpeed"; const char* unit = "rpm";
      int rawVal = main_ELM327.processPID(service, PID, 1, 1, 1, 0);
      float processedVal = 0.25 * rawVal;
      if (main_ELM327.nb_rx_state == ELM_SUCCESS)
      {
        Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
        INCREMENT_PID_STATE(currPID);
      }
      else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
      {
        // main_ELM327.printError();
        Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
        INCREMENT_PID_STATE(currPID);
      }
      // int scannerState = CheckScannerState(main_ELM327);
      // if (scannerState == RX_PASS){
        
      //   Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
      //   INCREMENT_PID_STATE(currPID);
      // }
      // else {
      //   Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
      //   INCREMENT_PID_STATE(currPID);
      // }
      break;
    }
    case PID_GEAR:
    {
      uint8_t service = 0x22; uint16_t PID = 0x5881;
      const char* sensor = "Gear"; const char* unit = "-";
      int rawVal = main_ELM327.processPID(service, PID, 1, 1, 1, 1);
      float processedVal = rawVal;
      if (main_ELM327.nb_rx_state == ELM_SUCCESS)
      {
        Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
        INCREMENT_PID_STATE(currPID);
      }
      else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
      {
        // main_ELM327.printError();
        Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
        INCREMENT_PID_STATE(currPID);
      }
      // int scannerState = CheckScannerState(main_ELM327);
      // if (scannerState == RX_PASS){
        
      //   Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
      //   INCREMENT_PID_STATE(currPID);
      // }
      // else {
      //   Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
      //   INCREMENT_PID_STATE(currPID);
      // }
      break;
    }
    case PID_BATTERY_VOLTAGE:
    {
      uint8_t service = 0x22; uint16_t PID = 0x460C;
      const char* sensor = "BatteryVoltage"; const char* unit = "V";
      int rawVal = main_ELM327.processPID(service, PID, 1, 1, 1, 1);
      float processedVal = 0.023481 * rawVal;
      if (main_ELM327.nb_rx_state == ELM_SUCCESS)
      {
        Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
        INCREMENT_PID_STATE(currPID);
      }
      else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
      {
        // main_ELM327.printError();
        Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
        INCREMENT_PID_STATE(currPID);
      }
      // int scannerState = CheckScannerState(main_ELM327);
      // if (scannerState == RX_PASS){
        
      //   Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
      //   INCREMENT_PID_STATE(currPID);
      // }
      // else {
      //   Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
      //   INCREMENT_PID_STATE(currPID);
      // }
      break;
    }
    
    case PID_AFR_TARGET:
    {
      uint8_t service = 0x22; uint16_t PID = 0x5816;
      const char* sensor = "LambdaSetpoint"; const char* unit = "-";
      int rawVal = main_ELM327.processPID(service, PID, 1, 1, 1, 1);
      float processedVal = rawVal * 0.000244141;
      if (main_ELM327.nb_rx_state == ELM_SUCCESS)
      {
        Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
        INCREMENT_PID_STATE(currPID);
      }
      else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
      {
        // main_ELM327.printError();
        Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
        INCREMENT_PID_STATE(currPID);
      }
      // int scannerState = CheckScannerState(main_ELM327);
      // if (scannerState == RX_PASS){
        
      //   Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
      //   INCREMENT_PID_STATE(currPID);
      // }
      // else {
      //   Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
      //   INCREMENT_PID_STATE(currPID);
      // }
      break;
    }
    case PID_AFR_ACTUAL:
    {
      uint8_t service = 0x22; uint16_t PID = 0x5889;
      const char* sensor = "LambdaActual"; const char* unit = "-";
      int rawVal = main_ELM327.processPID(service, PID, 1, 1, 1, 1);
      float processedVal = 0.000244141 * rawVal;
      if (main_ELM327.nb_rx_state == ELM_SUCCESS)
      {
        Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
        INCREMENT_PID_STATE(currPID);
      }
      else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
      {
        // main_ELM327.printError();
        Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
        INCREMENT_PID_STATE(currPID);
      }
      // int scannerState = CheckScannerState(main_ELM327);
      // if (scannerState == RX_PASS){
        
      //   Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
      //   INCREMENT_PID_STATE(currPID);
      // }
      // else {
      //   Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
      //   INCREMENT_PID_STATE(currPID);
      // }
      break;
    }
    case PID_SPEED:
    {
      // uint8_t service = 0x22; uint16_t PID = 0x4AB1;
      

      uint8_t service = 0x01; uint16_t PID = 0x0D;
      const char* sensor = "Speed"; const char* unit = "km/hr";
      int rawVal = main_ELM327.processPID(service, PID, 1, 1, 1, 0);
      // float processedVal = rawVal * 0.0078125;
      float processedVal = rawVal;
      if (main_ELM327.nb_rx_state == ELM_SUCCESS)
      {
        // take the current time, subtract the last time this was run
        static unsigned long t1 = 0; static float v1 = 0;
        static unsigned long t2 = 0; static float v2 = 0;
        static bool sub_first = false;
        
        double interval_hrs; double interval_km;
        
        if (sub_first){
          t1 = millis();
          v1 = processedVal;
          interval_hrs = (t1 - t2) / (1000 * 3600);
          interval_km = v1 - v2;
        }
        else {
          t2 = millis();
          v2 = processedVal;
          interval_hrs = (t2 - t1) / (1000 * 3600);
          interval_km = v2 - v1;
        }
        // Trapezoidal integration
        trip_dist_mi += 0.621371 * (0.5 * interval_hrs * interval_km);
        // switch state for which should be subbed first
        sub_first = !sub_first;
        Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
        Serial.printf("%li,TripDist,-,-,-,%.2f,Mi\n", millis(), trip_dist_mi);
        INCREMENT_PID_STATE(currPID);
      }
      else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
      {
        // main_ELM327.printError();
        Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
        INCREMENT_PID_STATE(currPID);
      }
      // int scannerState = CheckScannerState(main_ELM327);
      // if (scannerState == RX_PASS){
        
      //   Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
      //   INCREMENT_PID_STATE(currPID);
      // }
      // else {
      //   Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
      //   INCREMENT_PID_STATE(currPID);
      // }
      break;
    }
    
    
    case PID_TRIP_TIME:
    {
      uint8_t service = 0x22; uint16_t PID = 0x5800;
      const char* sensor = "TripTime"; const char* unit = "s";
      int rawVal = main_ELM327.processPID(service, PID, 1, 1, 1, 1);
      float processedVal = rawVal * 0.100000001;
      if (main_ELM327.nb_rx_state == ELM_SUCCESS)
      {
        Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
        INCREMENT_PID_STATE(currPID);
      }
      else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
      {
        // main_ELM327.printError();
        Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
        INCREMENT_PID_STATE(currPID);
      }
      // int scannerState = CheckScannerState(main_ELM327);
      // if (scannerState == RX_PASS){
        
      //   Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
      //   INCREMENT_PID_STATE(currPID);
      // }
      // else {
      //   Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
      //   INCREMENT_PID_STATE(currPID);
      // }
      break;
    }
    case PID_FUEL_CONS:
    {
      uint8_t service = 0x22; uint16_t PID = 0x4403;
      const char* sensor = "FuelConsSinceOilChange"; const char* unit = "L";
      static bool isFirstRequest = true;
      int rawVal = main_ELM327.processPID(service, PID, 1, 1, 1, 1);
      float processedVal = 0.00012207 * rawVal;
      if (main_ELM327.nb_rx_state == ELM_SUCCESS)
      {
        Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);

        float curr_fuel_cons_gal = L_TO_GAL * processedVal;
        if (isFirstRequest && curr_fuel_cons_gal > 0){
          isFirstRequest = !isFirstRequest;
          start_fuel_gal = curr_fuel_cons_gal; // store the starting total fuel usage value
        }
        else {
          float trip_fuel_cons_gal = curr_fuel_cons_gal - start_fuel_gal;
          Serial.printf("%li,TripFuelEconomy,-,-,-,%.2f,mpg\n", millis(), trip_dist_mi / trip_fuel_cons_gal);
        }
        
        INCREMENT_PID_STATE(currPID);
      }
      else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
      {
        // main_ELM327.printError();
        Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
        INCREMENT_PID_STATE(currPID);
      }
      // int scannerState = CheckScannerState(main_ELM327);
      // if (scannerState == RX_PASS){
        
      //   Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
      //   INCREMENT_PID_STATE(currPID);
      // }
      // else {
      //   Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
      //   INCREMENT_PID_STATE(currPID);
      // }
      break;
    }
    case PID_MAP:
    {
      uint8_t service = 0x22; uint16_t PID = 0x580B;
      const char* sensor = "MAP"; const char* unit = "hPa";
      int rawVal = main_ELM327.processPID(service, PID, 1, 1, 1, 1);
      float processedVal = rawVal * 0.0390625;
      if (main_ELM327.nb_rx_state == ELM_SUCCESS)
      {
        Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
        INCREMENT_PID_STATE(currPID);
      }
      else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
      {
        // main_ELM327.printError();
        Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
        INCREMENT_PID_STATE(currPID);
      }
      // int scannerState = CheckScannerState(main_ELM327);
      // if (scannerState == RX_PASS){
        
      //   Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
      //   INCREMENT_PID_STATE(currPID);
      // }
      // else {
      //   Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
      //   INCREMENT_PID_STATE(currPID);
      // }
      break;
    }
    case PID_IAT:
    {
      uint8_t service = 0x22; uint16_t PID = 0x580F;
      const char* sensor = "IAT"; const char* unit = "degC";
      int rawVal = main_ELM327.processPID(service, PID, 1, 1, 1, 1);
      float processedVal = 0.75 * rawVal - 48;
      if (main_ELM327.nb_rx_state == ELM_SUCCESS)
      {
        Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
        INCREMENT_PID_STATE(currPID);
      }
      else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
      {
        // main_ELM327.printError();
        Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
        INCREMENT_PID_STATE(currPID);
      }
      // int scannerState = CheckScannerState(main_ELM327);
      // if (scannerState == RX_PASS){
        
      //   Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
      //   INCREMENT_PID_STATE(currPID);
      // }
      // else {
      //   Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
      //   INCREMENT_PID_STATE(currPID);
      // }
      break;
    }
    case PID_MAF:
    {
      uint8_t service = 0x22; uint16_t PID = 0x5818;
      const char* sensor = "AirMassFlow"; const char* unit = "kg/hr";
      int rawVal = main_ELM327.processPID(service, PID, 1, 1, 1, 1);
      float processedVal = 4 * rawVal;
      if (main_ELM327.nb_rx_state == ELM_SUCCESS)
      {
        Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
        INCREMENT_PID_STATE(currPID);
      }
      else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
      {
        // main_ELM327.printError();
        Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
        INCREMENT_PID_STATE(currPID);
      }
      // int scannerState = CheckScannerState(main_ELM327);
      // if (scannerState == RX_PASS){
        
      //   Serial.printf("%li,%s,0x%x,0x%x,%d,%.2f,%s\n", millis(), sensor, service, PID, rawVal, processedVal, unit);
      //   INCREMENT_PID_STATE(currPID);
      // }
      // else {
      //   Serial.printf("%li,%s,0x%x,0x%x,N/A,N/A,N/A\n", millis(), sensor, service, PID);
      //   INCREMENT_PID_STATE(currPID);
      // }
      break;
    }
    case PID_MAX_STATE:
    {
      currPID = PID_States(0);
      break;
    }
  }
  // delay(10);
}


