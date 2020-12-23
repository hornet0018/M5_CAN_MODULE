#include <CAN.h>
#include <FastLED.h>

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
int speed_R = 0;
int speed_L = 0;
int speedR_data = 0;
int speedL_data = 0;

void setup()
{
  pinMode(39, INPUT);
  Serial.begin(115200);

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
  if (button == 0)
  {
    speedR_data = 200;
    speedL_data = 200;
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
  else
  {
    speedR_data = 0;
    speedL_data = 0;
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
