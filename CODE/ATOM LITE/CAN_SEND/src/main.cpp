// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <CAN.h>


uint8_t R = 0;
uint8_t G = 0;
uint8_t B = 0;

void setup() {
  Serial.begin(115200);
  CAN.setPins(21, 25);
  while (!Serial);

  Serial.println("CAN Sender");

  // start the CAN bus at 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }
}

void loop() {
  R = 0;
  G = 0;
  B = 0;
  for (int i=0; i <= 255; i++){
   
  Serial.print("Sending extended packet ... ");
  CAN.beginExtendedPacket(0x64);
  CAN.write(R+i/2);
  CAN.write(G-i+255);
  CAN.write(B+i);
  CAN.endPacket();

  Serial.println("done");

  delay(1);
  }
}