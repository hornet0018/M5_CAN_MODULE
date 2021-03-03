#include <CAN.h>
#include <FastLED.h>
#include <ArduinoJson.h>

#define NUM_LEDS 25
#define DATA_PIN 27
CRGB leds[NUM_LEDS];
const int voutPin = 33;
const float VOLT = 3.3;      // 3.3Vを電源とした場合
const int ANALOG_MAX = 4096; // ESP32の場合
const float CAL = 8.5925;    // 3.3Vを電源とした場合
uint8_t speedR_dataHH = 0;
uint8_t speedR_dataHL = 0;
uint8_t speedL_dataHH = 0;
uint8_t speedL_dataHL = 0;
int speedR_data = 0;
int speedL_data = 0;
float batt = 0;

void task0(void *arg)
{
  while (1)
  {
    speedR_dataHH = (uint8_t)((speedR_data & 0xFF00) >> 8);
    speedR_dataHL = (uint8_t)((speedR_data & 0x00FF) >> 0);
    speedL_dataHH = (uint8_t)((speedL_data & 0xFF00) >> 8);
    speedL_dataHL = (uint8_t)((speedL_data & 0x00FF) >> 0);

    CAN.beginExtendedPacket(0x02);
    CAN.write(speedR_dataHH);
    CAN.write(speedR_dataHL);
    CAN.write(speedL_dataHH);
    CAN.write(speedL_dataHL);
    CAN.endPacket();
    delay(10);
  }
}

void setup()
{
  pinMode(39, INPUT);
  Serial.begin(115200);

  // create tasks
  xTaskCreatePinnedToCore(task0, "Task0", 4096, NULL, 1, NULL, 0);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(20);

  CAN.setPins(25, 21);

  for (int k = 0; k < NUM_LEDS; k++)
  {
    leds[k] = CRGB::Green;
  }
  FastLED.show();

  // start the CAN bus at 500 kbps
  if (!CAN.begin(1000E3))
  {
    Serial.println("Starting CAN failed!");
  }
}

void loop()
{
  int button = digitalRead(39);
  DynamicJsonDocument root(200);

  int packetSize = CAN.parsePacket();
  // only print packet data for non-RTR packets
  while (CAN.available())
  {
    if (CAN.packetId() == 0x01)
    {
      uint8_t dataHH = (uint8_t)CAN.read();
      uint8_t dataHL = (uint8_t)CAN.read();
      uint8_t dataLH = (uint8_t)CAN.read();
      uint8_t dataLL = (uint8_t)CAN.read();
      char soc = (uint8_t)CAN.read();

      int32_t data = (int32_t)(
          (((int32_t)dataHH << 24) & 0xFF000000) | (((int32_t)dataHL << 16) & 0x00FF0000) | (((int32_t)dataLH << 8) & 0x0000FF00) | (((int32_t)dataLL << 0) & 0x000000FF));

      root["batt"] = data;
      root["soc"] = soc;
      serializeJson(root, Serial);
    }
    else
    {
      break;
    }
  }
  if (button == 0)
  {
    speedR_data = 200;
    speedL_data = 200;
  }
  else
  {
    speedR_data = 0;
    speedL_data = 0;
  }
  
}
