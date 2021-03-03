//LED ID:0x03

#include <CAN.h>
#include <FastLED.h>

#define NUM_LEDS 25
#define DATA_PIN 27
CRGB leds[NUM_LEDS];

uint8_t R = 0;
uint8_t G = 0;
uint8_t B = 0;

void LED(int R, int G, int B)
{
  for (int k = 0; k < NUM_LEDS; k++)
  {
    leds[k].setRGB(R, G, B);
  }
  FastLED.show();
  delay(50);
}

void setup()
{
  Serial.begin(115200);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(20);
  for (int k = 0; k < NUM_LEDS; k++)
  {
    leds[k] = CRGB::Purple;
  }
  FastLED.show();

  CAN.setPins(25, 21);
  while (!Serial)
    ;

  Serial.println("CAN Receiver");

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
  // try to parse packet
  int packetSize = CAN.parsePacket();
  // only print packet data for non-RTR packets

  while (CAN.available())
  {
    if (CAN.packetId() == 0x03)
    {
      R = (uint8_t)CAN.read();
      G = (uint8_t)CAN.read();
      B = (uint8_t)CAN.read();
      Serial.print(R);
      Serial.print(",");
      Serial.print(G);
      Serial.print(",");
      Serial.print(B);
      LED(R,G,B);
    }
    else
    {
      break;
    }
  }
}