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

#define MAX_SPEED 200

int motorLeft, motorRight;
void robotMove(int speedLeft, int speedRight);

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
    leds[k] = CRGB::Red;
  }
  FastLED.show();

  // start the CAN bus at 500 kbps
  if (!CAN.begin(1000E3))
  {
    Serial.println("Starting CAN failed!");
  }
}

void robotMove(int speedLeft, int speedRight)
{
  if (speedLeft > 0)
  {
    int speedL = map(speedLeft, 0, 255, 255, 0);
    ledcWrite(M1A_PWM_CHANNEL, speedL);
    ledcWrite(M1B_PWM_CHANNEL, 255);
  }
  else
  {
    int speedL = map(speedLeft, -255, 0, 0, 255);
    ledcWrite(M1A_PWM_CHANNEL, 255);
    ledcWrite(M1B_PWM_CHANNEL, speedL);
  }

  if (speedRight > 0)
  {
    int speedR = map(speedRight, 0, 255, 255, 0);
    ledcWrite(M2A_PWM_CHANNEL, speedR);
    ledcWrite(M2B_PWM_CHANNEL, 255);
  }
  else
  {
    int speedR = map(speedRight, -255, 0, 0, 255);
    ledcWrite(M2A_PWM_CHANNEL, 255);
    ledcWrite(M2B_PWM_CHANNEL, speedR);
  }
}

void loop()
{
  // try to parse packet
  int packetSize = CAN.parsePacket();

  if (packetSize)
  {
    // received a packet
    Serial.print("Received ");

    if (CAN.packetExtended())
    {
      Serial.print("extended ");
    }

    if (CAN.packetRtr())
    {
      // Remote transmission request, packet contains no data
      Serial.print("RTR ");
    }

    Serial.print("packet with id 0x");
    Serial.print(CAN.packetId(), HEX);

    if (CAN.packetRtr())
    {
      Serial.print(" and requested length ");
      Serial.println(CAN.packetDlc());
    }
    else
    {
      Serial.print(" and length ");
      Serial.println(packetSize);

      // only print packet data for non-RTR packets
      while (CAN.available())
      {
        
      }
      Serial.println();
    }
    Serial.println();
  }
}