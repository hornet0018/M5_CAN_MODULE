#include <CAN.h>
#include <FastLED.h>
#include <Arduino.h>

#define NUM_LEDS 25
#define DATA_PIN 27
CRGB leds[NUM_LEDS];

#define M1A 22
#define M1B 19
#define M2A 23
#define M2B 33

#define PWM_FREQUENCY 1000
#define PWM_RESOLUTION 8
#define M1A_PWM_CHANNEL 0
#define M1B_PWM_CHANNEL 1
#define M2A_PWM_CHANNEL 2
#define M2B_PWM_CHANNEL 3

#define MAX_SPEED 255

int16_t speedR_data = 0;
int16_t speedL_data = 0;

void robotMove(int speedLeft, int speedRight)
{
  if (speedLeft > 0)
  {
    int speedL = map(speedLeft, 0, 255, 255, 0);

    ledcWrite(M1A_PWM_CHANNEL, 255);
    ledcWrite(M1B_PWM_CHANNEL, speedL);
  }
  else
  {
    int speedL = map(speedLeft, -255, 0, 0, 255);
    ledcWrite(M1A_PWM_CHANNEL, speedL);
    ledcWrite(M1B_PWM_CHANNEL, 255);
  }

  if (speedRight > 0)
  {
    int speedR = map(speedRight, 0, 255, 255, 0);
    ledcWrite(M2A_PWM_CHANNEL, 255);
    ledcWrite(M2B_PWM_CHANNEL, speedR);
  }
  else
  {
    int speedR = map(speedRight, -255, 0, 0, 255);
    ledcWrite(M2A_PWM_CHANNEL, speedR);
    ledcWrite(M2B_PWM_CHANNEL, 255);
  }
}

void setup()
{

  ledcSetup(M1A_PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcSetup(M1B_PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcSetup(M2A_PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcSetup(M2B_PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);

  ledcAttachPin(M1A, M1A_PWM_CHANNEL);
  ledcAttachPin(M1B, M1B_PWM_CHANNEL);
  ledcAttachPin(M2A, M2A_PWM_CHANNEL);
  ledcAttachPin(M2B, M2B_PWM_CHANNEL);

  Serial.begin(115200);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(20);

  CAN.setPins(25, 21);

  for (int k = 0; k < NUM_LEDS; k++)
  {
    leds[k] = CRGB::Blue;
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
  // try to parse packet
  int packetSize = CAN.parsePacket();
  // only print packet data for non-RTR packets

  while (CAN.available())
  {
    if (CAN.packetId() == 0x02)
    {
      uint8_t speedR_HH = (uint8_t)CAN.read();
      uint8_t speedR_HL = (uint8_t)CAN.read();
      uint8_t speedL_HH = (uint8_t)CAN.read();
      uint8_t speedL_HL = (uint8_t)CAN.read();

      speedR_data = (int16_t)(
          (((int16_t)speedR_HH << 8) & 0xFF00) | (((int16_t)speedR_HL << 0) & 0x00FF));
      speedL_data = (int16_t)(
          (((int16_t)speedL_HH << 8) & 0xFF00) | (((int16_t)speedL_HL << 0) & 0x00FF));
      Serial.print("R_data: ");
      Serial.print(speedR_data);
      Serial.print(",");
      Serial.print("L_data: ");
      Serial.println(speedL_data);

      robotMove(speedR_data, speedL_data);
    }
    else
    {
      break;
    }
  }
}