#include <CAN.h>
#include <FastLED.h>
#include <Battery.h>

#define NUM_LEDS 25
#define DATA_PIN 27
CRGB leds[NUM_LEDS];
const int voutPin = 33;
const float VOLT = 3.3;      // 3.3Vを電源とした場合
const int ANALOG_MAX = 4096; // ESP32の場合
const float CAL = 8.5925;    // 3.3Vを電源とした場合
Battery battery(9000, 12600, voutPin);
uint8_t soc = 0;
void task0(void *arg)
{
  while (true)
  {
    if (soc > 75)
    {
      for (int k = 0; k < NUM_LEDS; k++)
      {
        leds[k] = CRGB::Green;
      }
      FastLED.show();
      delay(500);
    }
    else if (soc > 50)
    {
      for (int k = 0; k < NUM_LEDS; k++)
      {
        leds[k] = CRGB::Yellow;
      }
      FastLED.show();
      delay(500);
    }
    else
    {
      for (int k = 0; k < NUM_LEDS; k++)
      {
        leds[k] = CRGB::Red;
      }
      FastLED.show();
      delay(500);
    }
  }
}

void setup()
{
  Serial.begin(115200);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(20);

  // create tasks
  xTaskCreatePinnedToCore(task0, "Task0", 4096, NULL, 1, NULL, 0);

  battery.begin(3300, 2.1555943878);

  CAN.setPins(25, 21);

  for (int k = 0; k < NUM_LEDS; k++)
  {
    leds[k] = CRGB::Red;
  }
  FastLED.show();

  // start the CAN bus at 500 kbps
  if (!CAN.begin(1000E3))
  {
    Serial.println("Starting CAN failed!");
    while (1)
      ;
  }
}

void loop()
{
  Serial.print("Battery voltage is ");
  Serial.print(battery.voltage());
  Serial.print(" (");
  Serial.print(battery.level());
  Serial.println("%)");
  uint32_t data = battery.voltage();
  soc = battery.level();

  uint8_t dataHH = (uint8_t)((data & 0xFF000000) >> 24);
  uint8_t dataHL = (uint8_t)((data & 0x00FF0000) >> 16);
  uint8_t dataLH = (uint8_t)((data & 0x0000FF00) >> 8);
  uint8_t dataLL = (uint8_t)((data & 0x000000FF) >> 0);

  //Serial.print("Sending extended packet ... ");
  CAN.beginExtendedPacket(0x01);
  CAN.write(dataHH);
  CAN.write(dataHL);
  CAN.write(dataLH);
  CAN.write(dataLL);
  CAN.write(soc);
  CAN.endPacket();

  delay(500);
}