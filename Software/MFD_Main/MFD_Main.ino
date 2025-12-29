// SET_LOOP_TASK_STACK_SIZE(1024 * 16); 

#include "Gauge.h"

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
  PID_BARO_PRESS,
  PID_BOOST_PRESS,
  PID_OIL_PRESS,
  PID_FUEL_PRESS,
  PID_INTAKE_PRESS,
  PID_TRIP_TIME,
  PID_VEHICLE_SPEED,
  PID_FUEL_CONS,
  PID_AIR_FUEL_RATIO,
  PID_MASS_AIRFLOW,
  PID_MAX_STATE,
} PID_States;

using PidHandlerFn = void (*)(int value);
struct PidRequest
{
    uint8_t pidIndex;        // index into your PID array
    PidHandlerFn onSuccess;  // how to store/process it
};
constexpr PidRequest pidTable[] =
{
    { PID_OIL_TEMP,      handleOilTemp     },
    { PID_COOLANT_TEMP,  handleCoolantTemp },
    { PID_BOOST,         handleBoost       },
};


/******************************************************************************
*                            MODULE-LEVEL VARIABLES                           *
******************************************************************************/
static Gauge mainGauge;
static ELM327 main_ELM327;
static bool buttonPressed = false;
static GaugeData data; 
static IPAddress server(192, 168, 0, 10);
static WiFiClient client;

// Array with parameters to processPID()
// source for service 0x01 PIDs: https://en.wikipedia.org/wiki/OBD-II_PIDs#Standard_PIDs
// source for service 0x22 PIDs: https://thesecretingredient.neocities.org/bmw/dme/n55/
static int PIDsArray [(int)PID_MAX_STATE][4] = {
  // {Service ID, Parameter ID, num responses, num bytes of response}
  {0x22, 0x5822, 1, 1}, // Oil temperature
  {0x01, 0x0005, 1, 1}, // Coolant temperature
  {0x22, 0x5834, 1, 2}, // Baro/ambient pressure
  {0x22, 0x4205, 1, 2}, // Boost pressure*
  {0x22, 0x586F, 1, 2}, // Oil pressure*
  {0x22, 0x58EF, 1, 2}, // Fuel rail pressure*
  {0x22, 0x580B, 1, 2}, // MAP
  {0x22, 0x5800, 1, 2}, // Trip time
  {0x01, 0x000D, 1, 1}, // Vehicle speed mi/hr
  {0x22, 0x4403, 1, 4}, // Fuel consumption since last oil change**
  {0x22, 0x5889, 1, 2}, // Lambda
  {0x22, 0x5818, 1, 1}, // MAF
  // *  Subtract this from the baro pressure to get true pressure value
  // ** Subtract this from first reading to get trip consumption
};

// Scale and offset for each parameter
static float PIDsScalingArray [(int)PID_MAX_STATE][2] = {
  // {scale, offset}
  {1, -60}, // Oil temperature
  {1, -40}, // Coolant temperature
  {(0.0390625 * HPA_TO_PSI), 0}, // Baro/ambient pressure
  {(0.078125 * HPA_TO_PSI), 0}, // Boost pressure*
  {HPA_TO_PSI, 0}, // Oil pressure*
  {(0.0005 * MPA_TO_PSI), 0}, // Fuel rail pressure*
  {(0.0390625 * HPA_TO_PSI), 0}, // MAP
  {0.100000001, 0}, // Trip time
  {0.621371, 0}, // Vehicle speed mi/hr
  {(0.00012207 * L_TO_GAL), 0}, // Fuel consumption since last oil change**
  {0.000244141, 0}, // Lambda
  {4, 0} // MAF
};

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

void processPidState(ELM327 &elm)
{
    static uint8_t pidState = 0;
    const auto &req = pidTable[pidState];

    int value = REQUEST_DATA(elm, req.pidIndex);

    if (elm.nb_rx_state == ELM_SUCCESS)
    {
        req.onSuccess(value);
        pidState = (pidState + 1) % (sizeof(pidTable) / sizeof(pidTable[0]));
    }
    else if (elm.nb_rx_state != ELM_GETTING_MSG)
    {
        elm.printError();
        pidState = (pidState + 1) % (sizeof(pidTable) / sizeof(pidTable[0]));
    }
}


// Calculate brightness level based on ambient light sensor input
uint8_t CalculateBrightness() {
  static uint8_t brightness_setting = BRIGHTNESS_HI;
  
  int brightness = analogRead(BRIGHTNESS_SENSOR_PIN);
  // Serial.println(brightness);

  // implement hysteresis lower bound (transition from high to low brightness)
  if (brightness_setting == BRIGHTNESS_HI && 
      brightness < BRIGHTNESS_LO_BOUND - BRIGHTNESS_HYST){
    brightness_setting = BRIGHTNESS_LO;
  }
  // implement hysteresis upper bound (transition from low to high brightness)
  else if (brightness_setting == BRIGHTNESS_LO && 
      brightness > BRIGHTNESS_LO_BOUND + BRIGHTNESS_HYST){
    brightness_setting = BRIGHTNESS_HI;
  }
  return brightness_setting;
}

void handleOilTemp(int v) { data.OilTemp = v; }

void handleCoolantTemp(int v) { data.CoolantTemp = v; }

void handleBoost(int v) { data.BoostPSI = (v - 14.7f); } 


/******************************************************************************
*                                    MACROS                                   *
******************************************************************************/
#define INCREMENT_PID_STATE(PID) do { \
  PID = PID_States((int)PID + 1); \
  } while(0)

#define INCREMENT_GAUGE_TYPE(type) do { \
  type = (GaugeType)( ((int)type + 1) % GAUGE_TYPE_MAX ); \
  } while(0)

#define REQUEST_DATA(scanner, index) (\
  scanner.processPID( \
    PIDsArray[index][0], \
    PIDsArray[index][1],  \
    PIDsArray[index][2],  \
    PIDsArray[index][3],  \
    PIDsScalingArray[index][0],  \
    PIDsScalingArray[index][1] \
  ) \
)

#define IS_RX_SUCCESS(scanner) (scanner.nb_rx_state == ELM_SUCCESS)
#define IS_RX_FAIL(scanner) (scanner.nb_rx_state != ELM_GETTING_MSG)

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
  mainGauge.begin();
  mainGauge.update(data);

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
  

  // mainGauge.begin();
}


/******************************************************************************
*                                 MAIN DRIVER                                 *
******************************************************************************/
void loop() {
  // Gauge variables
  static GaugeType type = GAUGE_TYPE_OIL_TEMP;
  // Timer variables to track button presses
  static unsigned long buttonTimer = 0; static bool startPress = false;
  // Timer for when to check the ambient light level/set brightness
  static unsigned long brightnessTimer = 0;
  // PID to read
  static PID_States currPID = PID_OIL_TEMP;
  // Variable to track the first cycle in order to get initial fuel consmption reading
  static bool gotInitialFuelConsumption = false;

  int PIDsArrayIndex = (int)currPID;
  switch (currPID){
    case PID_OIL_TEMP:
    {
      // make a request to ELM327 for the oil temp PID
      int oilTemp = REQUEST_DATA(main_ELM327, PIDsArrayIndex);
      if (IS_RX_SUCCESS(main_ELM327))
      {
        data.OilTemp = oilTemp;
        INCREMENT_PID_STATE(currPID);
      }
      else if (IS_RX_FAIL(main_ELM327))
      {
        main_ELM327.printError();
        INCREMENT_PID_STATE(currPID);
      }
      break;
    }
    case PID_COOLANT_TEMP:
    {
      int coolantTemp = REQUEST_DATA(main_ELM327, PIDsArrayIndex);
      if (IS_RX_SUCCESS(main_ELM327))
      {
        data.CoolantTemp = coolantTemp;
        INCREMENT_PID_STATE(currPID);
      }
      else if (IS_RX_FAIL(main_ELM327))
      {
        main_ELM327.printError();
        INCREMENT_PID_STATE(currPID);
      }
      break;
    }
    case PID_BARO_PRESS:
    {
      break;
    }
    case PID_BOOST_PRESS:
    {
      break;
    }
    case PID_OIL_PRESS:
    {
      break;
    }
    case PID_FUEL_PRESS:
    {
      break;
    }
    case PID_INTAKE_PRESS:
    {
      break;
    }
    case PID_TRIP_TIME:
    {
      break;
    }
    case PID_VEHICLE_SPEED:
    {
      break;
    }
    case PID_FUEL_CONS:
    {
      break;
    }
    case PID_AIR_FUEL_RATIO:
    {
      break;
    }
    case PID_MASS_AIRFLOW:
    {
      break;
    }
    // got to last PID to check
    // now perform extra calculations (?) and gauge drawing
    case PID_MAX_STATE:
    {
      // update all gauge data, gauge library handles repainting internally 
      mainGauge.update(data);

      // reset state variable
      currPID = PID_States(0);
      break;
    }
  }


  // begin counting how long the button has been pressed once the ISR catches 
  // a button press event
  // Serial.printf("buttonPressed = %d, startPress = %d\n", 
  //   buttonPressed, startPress);
  if (buttonPressed && !startPress){
    buttonTimer = millis();
    startPress = true;
    Serial.println("Button pressed");
  }
  // stop timing how long the button press took after the ISR catches
  // a button de-press event
  if (!buttonPressed && startPress){  
    // won't account for o'flow which happens after 50 days of continuous use
    unsigned long buttonInterval = millis() - buttonTimer; 
    startPress = false;
    Serial.printf("Button pressed for %li ms\n", buttonInterval);

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
  
  // check ambient light level and update screen brightness
  // performed at BRIGHTNESS_INTVL milliseconds
  unsigned long lightInterval = millis() - brightnessTimer; 
  if (lightInterval > BRIGHTNESS_INTVL){
    int brightness = CalculateBrightness();
    mainGauge.setBrightness(brightness);
    brightnessTimer = millis(); // reset timer
  }

}
