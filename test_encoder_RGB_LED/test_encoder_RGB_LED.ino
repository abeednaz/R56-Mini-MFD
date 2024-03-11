#define LED_R 7
#define LED_G 6
#define LED_B 5

#define LED_R_CHANNEL 0
#define LED_G_CHANNEL 1
#define LED_B_CHANNEL 2
#define LED_FREQ 5000
#define LED_RES 8

void setup() {
  // put your setup code here, to run once:
  // pinMode(LED_R, OUTPUT);
  // pinMode(LED_G, OUTPUT);
  // pinMode(LED_B, OUTPUT);

  // ledcSetup(LED_R_CHANNEL, LED_FREQ, LED_RES);
  // ledcSetup(LED_G_CHANNEL, LED_FREQ, LED_RES);
  // ledcSetup(LED_B_CHANNEL, LED_FREQ, LED_RES);
  // ledcAttachPin(LED_R, LED_R_CHANNEL);
  // ledcAttachPin(LED_G, LED_G_CHANNEL);
  // ledcAttachPin(LED_B, LED_B_CHANNEL);
  
  // analogWrite(LED_R, 255);
  // analogWrite(LED_G, 122);
  // analogWrite(LED_B, 0);
  
  Serial.begin(115200);
  Serial.println("Starting");
}

void loop() {
  // put your main code here, to run repeatedly:
  
  int i;
  for (i = 0; i < 255; i++){
    // ledcWrite(LED_R_CHANNEL, i);
    // ledcWrite(LED_G_CHANNEL, i);
    // ledcWrite(LED_B_CHANNEL, i);
    analogWrite(LED_R, i);
    analogWrite(LED_G, i);
    analogWrite(LED_B, i);
    delay(100);
  }
}
