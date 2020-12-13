#include <CAN.h>
#include <FastLED.h>

#define NUM_LEDS 25
#define DATA_PIN 27
CRGB leds[NUM_LEDS];

uint8_t R = 0;
uint8_t G = 0;
uint8_t B = 0;
void LED();

void setup() {
  Serial.begin(115200);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(20);

  CAN.setPins(21, 25);
  while (!Serial);

  Serial.println("CAN Receiver");

  // start the CAN bus at 500 kbps
 if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }
}

void LED()
{
   for (int k = 0; k < NUM_LEDS; k++)
  {
    leds[k].setRGB(R,G,B);
  }
  FastLED.show();
  delay(50);
}

void loop() {
  // try to parse packet
  int packetSize = CAN.parsePacket();

  if (packetSize) {
    // received a packet
    Serial.print("Received ");

    if (CAN.packetExtended()) {
      Serial.print("extended ");
    }

    if (CAN.packetRtr()) {
      // Remote transmission request, packet contains no data
      Serial.print("RTR ");
    }

    Serial.print("packet with id 0x");
    Serial.print(CAN.packetId(), HEX);

    if (CAN.packetRtr()) {
      Serial.print(" and requested length ");
      Serial.println(CAN.packetDlc());
    } else {
      Serial.print(" and length ");
      Serial.println(packetSize);

      // only print packet data for non-RTR packets
      while (CAN.available()) {

        R = (uint8_t)CAN.read();
        G = (uint8_t)CAN.read();
        B = (uint8_t)CAN.read();
        Serial.print(R);
        Serial.print(",");
        Serial.print(G);
        Serial.print(",");
        Serial.print(B);
      }
      Serial.println();
    }
    Serial.println();
  }
 LED();
}