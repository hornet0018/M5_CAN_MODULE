#include <CAN.h>
#include <FastLED.h>

#define NUM_LEDS 25
#define DATA_PIN 27
CRGB leds[NUM_LEDS];


void setup()
{
  Serial.begin(115200);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(20);

  CAN.setPins(25, 21);
  while (!Serial);

  Serial.println("CAN Receiver");

  // start the CAN bus at 500 kbps
  if (!CAN.begin(1000E3))
  {
    Serial.println("Starting CAN failed!");

  }
}


void loop()
{
  // try to parse packet
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

      int32_t data = (int32_t)(
          (((int32_t)dataHH << 24) & 0xFF000000) | (((int32_t)dataHL << 16) & 0x00FF0000) | (((int32_t)dataLH << 8) & 0x0000FF00) | (((int32_t)dataLL << 0) & 0x000000FF));

      Serial.print(data);
      Serial.print("mv");
      Serial.println();
    }
    else{
      break;
    }
  }
}