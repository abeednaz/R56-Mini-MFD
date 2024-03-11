#include <WiFi.h>
#include "ELMduino.h"

/*
  2/24/2024
  Adapted from ESP32_WiFi.ino from ELMDuino library
  Constanttly prints throttle position 0-100 (works with engine off)
*/

const char* ssid = "WiFi_OBDII";
const char* password = "your-password";


//IP Adress of your ELM327 Dongle
IPAddress server(192, 168, 0, 10);
WiFiClient client;
ELM327 myELM327;


uint32_t throttlePos = 0;


void setup()
{
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
  myELM327.begin(client, false, 2000);
}


void loop()
{
  float tempThrottlePos = myELM327.absThrottlePosD();

  if (myELM327.nb_rx_state == ELM_SUCCESS)
  {
    throttlePos = (uint32_t)tempThrottlePos;
    throttlePos = throttlePosConv(throttlePos);
    Serial.print("Throttle position: "); Serial.println(throttlePos);
  }
  else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
    myELM327.printError();
}

#define MIN_THROTTLE 14
#define MAX_THROTTLE 80
int throttlePosConv(int throttlePosRaw)
{
  return ( 100 * (throttlePosRaw - MIN_THROTTLE) ) / (MAX_THROTTLE - MIN_THROTTLE);
}