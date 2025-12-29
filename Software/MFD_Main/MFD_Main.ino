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
*                            FUNCTION PROTOTYPES                              *
******************************************************************************/
void HandleOilTemp(double valueFromELM);
void HandleCoolantTemp(double valueFromELM);
void HandleBaroPress(double valueFromELM);
void HandleBoostPress(double valueFromELM);
void HandleOilPress(double valueFromELM);
void HandleFuelPress(double valueFromELM);
void HandleGaugeUpdate(double valueFromELM);
void HandleNULL(double valueFromELM);
// void IRAM_ATTR RegButton();
void ProcessPidState();
uint8_t CalculateBrightness();

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

// General definition for PID handler function
// Note: Match general function prototype for handler
using PIDHandlerFunc = void (*)(double value); 

// Indexable definition of PID request
struct PIDRequestToFunction
{
    uint8_t PIDIndex;          // index of PID array
    PIDHandlerFunc onSuccess;  // name of function which post processes the data from processPID()
};

// Array map of each PID request : handler function pair
constexpr PIDRequestToFunction PIDToFunctionMap[] =
{
  //  PID_state enumeration:function name
  { PID_OIL_TEMP,       HandleOilTemp     },
  { PID_COOLANT_TEMP,   HandleCoolantTemp },
  { PID_BARO_PRESS,     HandleBaroPress   },
  { PID_BOOST_PRESS,    HandleBoostPress  },
  { PID_OIL_PRESS,      HandleOilPress    },
  { PID_FUEL_PRESS,     HandleFuelPress   },
  { PID_INTAKE_PRESS,   HandleNULL        },
  { PID_TRIP_TIME,      HandleNULL        },
  { PID_VEHICLE_SPEED,  HandleNULL        },
  { PID_FUEL_CONS,      HandleNULL        },
  { PID_AIR_FUEL_RATIO, HandleNULL        },
  { PID_MASS_AIRFLOW,   HandleNULL        },
  { PID_MAX_STATE,      HandleGaugeUpdate      },
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
static int ProcessPIDsArray [(int)PID_MAX_STATE + 1][4] = {
  // {Service ID, Parameter ID, num responses, num bytes of response}
  {0x22, 0x5822, 1, 1}, // Oil temperature
  {0x01, 0x0005, 1, 1}, // Coolant temperature
  {0x22, 0x5834, 1, 2}, // Baro/ambient pressure
  {0x22, 0x4205, 1, 2}, // Boost pressure*
  {0x22, 0x586F, 1, 2}, // Oil pressure*
  {0x22, 0x58EF, 1, 2}, // Fuel rail pressure*
  {0x22, 0x580B, 1, 2}, // MAP
  {0x22, 0x5800, 1, 2}, // Trip time
  {0x01, 0x000D, 1, 1}, // Vehicle speed mi/hr**
  {0x22, 0x4403, 1, 4}, // Fuel consumption since last oil change***
  {0x22, 0x5889, 1, 2}, // Lambda
  {0x22, 0x5818, 1, 1}, // MAF
  {0x01, 0x0000, 1, 1}, // Placeholder
  // *   Subtract this from the baro pressure to get true pressure value
  // **  Integrate this to get trip distance
  // *** Subtract this from first reading to get trip consumption
};

// Scale and offset for each parameter
static float ProcessPIDsScalingArray [(int)PID_MAX_STATE + 1][2] = {
  // {scale, offset}
  {1, -60},                      // Oil temperature
  {1, -40},                      // Coolant temperature
  {(0.0390625 * HPA_TO_PSI), 0}, // Baro/ambient pressure
  {(0.078125 * HPA_TO_PSI), 0},  // Boost pressure
  {HPA_TO_PSI, 0},               // Oil pressure*
  {(0.0005 * MPA_TO_PSI), 0},    // Fuel rail pressure
  {(0.0390625 * HPA_TO_PSI), 0}, // MAP
  {0.100000001, 0},              // Trip time
  {0.621371, 0},                 // Vehicle speed mi/hr
  {(0.00012207 * L_TO_GAL), 0},  // Fuel consumption since last oil change**
  {0.000244141, 0},              // Lambda
  {4, 0},                        // MAF
  {999, 999},                    // Placeholder
};

/******************************************************************************
*                                    MACROS                                   *
******************************************************************************/
// Increment state variable for which PID to read
#define INCREMENT_PID_STATE(stateVar) do { \
  stateVar = PID_States( ((int)stateVar + 1) % ((int)PID_MAX_STATE + 1) ) ; \
  } while(0)

// Increment gauge between oiltemp, coolant temp, etc.
#define INCREMENT_GAUGE_TYPE(type) do { \
  type = (GaugeType)( ((int)type + 1) % GAUGE_TYPE_MAX ); \
  } while(0)

// Wrapper for ELMDuino ProcessPID()
#define REQUEST_DATA(scanner, index) (\
  scanner.processPID( \
    ProcessPIDsArray[index][0], \
    ProcessPIDsArray[index][1],  \
    ProcessPIDsArray[index][2],  \
    ProcessPIDsArray[index][3],  \
    ProcessPIDsScalingArray[index][0],  \
    ProcessPIDsScalingArray[index][1] \
  ) \
)

/******************************************************************************
*                         ACTION HANDLER FUNCTIONS                            *
******************************************************************************/

void HandleOilTemp(double valueFromELM)
{ 
  data.OilTemp_C = (int)valueFromELM; 
  // Serial.printf("%li: OilTemp=%d degC\n", millis(), data.OilTemp_C);
}
void HandleCoolantTemp(double valueFromELM)
{ 
  data.CoolantTemp_C = (int)valueFromELM; 
  // Serial.printf("%li: CoolantTemp=%d degC\n", millis(), data.CoolantTemp_C);
}
void HandleBaroPress(double valueFromELM)
{
  data.BaroPress_PSI = valueFromELM; 
  // Serial.printf("%li: BaroPress=%.2f PSI\n", millis(), data.BaroPress_PSI);
}
void HandleBoostPress(double valueFromELM)
{ 
  data.BoostPress_PSI = valueFromELM - data.BaroPress_PSI; 
  // Serial.printf("%li: TrueBoostPress=%.2f PSI\n", millis(), data.BoostPress_PSI);
} 
void HandleOilPress(double valueFromELM)
{
  data.OilPress_PSI = valueFromELM - data.BaroPress_PSI; 
  // Serial.printf("%li: TrueOilPress=%.2f PSI\n", millis(), data.OilPress_PSI);
}
void HandleFuelPress(double valueFromELM)
{
  data.FuelPress_PSI = valueFromELM - data.BaroPress_PSI; 
  // Serial.printf("%li: FuelRailPress=%.2f PSI\n", millis(), data.FuelPress_PSI);
}
// Special case for last PID reached
// Update the gauge data after all PIDs have been checked
// TODO: Run extra calculations to get trip fuel economy
void HandleGaugeUpdate(double valueFromELM)
{
  mainGauge.update(data);
}
// Placeholder handler function
void HandleNULL(double valueFromELM)
{ 
  ; 
}

/******************************************************************************
*                         INTERRUPT SERVICE ROUTINES                          *
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

/******************************************************************************
*                              HELPER FUNCTIONS                               *
******************************************************************************/

/*
ProcessPidState()
PID Processor function
This state handler goes through each PID symbolic state in PID_States
and performs the PID request defined in the ProcessPIDsArray(s)
Upon getting valid data, the function callback defined in PIDToFunctionMap
for the PID symbolic state will be executed to store and/or manipulate the data
*/
void ProcessPidState()
{
    static PID_States currPID = PID_OIL_TEMP;

    const auto &req = PIDToFunctionMap[(int)currPID]; // obtain the function handle for this PID

    double value = REQUEST_DATA(main_ELM327, req.PIDIndex); // run processPID() on this PID

    // Message is good, accept and move onto next PID
    if (main_ELM327.nb_rx_state == ELM_SUCCESS)
    {
        req.onSuccess(value); // any extra post processing for this PID
        INCREMENT_PID_STATE(currPID);
    }
    // ELM timed out or got a bad response, move onto next PID
    else if (main_ELM327.nb_rx_state != ELM_GETTING_MSG)
    {
        main_ELM327.printError();
        INCREMENT_PID_STATE(currPID);
    }
    // If the above two conditions weren't met, the loop surrounding this function
    // will re-check the request status on the next iteration of this function
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

/******************************************************************************
*                                INIT FUNCTION                                *
******************************************************************************/

void setup() {
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
  // Variable to track the first cycle in order to get initial fuel consmption reading
  // static bool gotInitialFuelConsumption = false;

  // Run the state machine
  ProcessPidState();
  


  // begin counting how long the button has been pressed once the ISR catches 
  // a button press event
  // Serial.printf("buttonPressed = %d, startPress = %d\n", 
  //   buttonPressed, startPress);
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
    // Serial.printf("Button pressed for %li ms\n", buttonInterval);

    // Short press: Switch gauges
    if (buttonInterval > BTN_1_ACTION && buttonInterval < BTN_2_ACTION){
      INCREMENT_GAUGE_TYPE(type);
      mainGauge.setType(type);

      // Serial.print("Switching gauges to:");
      // Serial.println(int(type));
    }
    // Long press: undefined action
    else {
      // Serial.println("Secondary action");
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
