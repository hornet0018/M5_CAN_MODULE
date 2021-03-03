#include <CAN.h>
#include <FastLED.h>
#include <ArduinoJson.h>

#define NUM_LEDS 25
#define DATA_PIN 27
CRGB leds[NUM_LEDS];
uint8_t R = 0;
uint8_t G = 0;
uint8_t B = 0;

void task0(void *arg)
{
  while (1)
  {
    CAN.beginExtendedPacket(0x03);
    CAN.write(R);
    CAN.write(G);
    CAN.write(B);
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
  R = 255;
  G = 0;
  B = 0;
  delay(100);
  R = 0;
  G = 255;
  B = 0;
  delay(100);
  R = 0;
  G = 0;
  B = 255;
  delay(100);
}
