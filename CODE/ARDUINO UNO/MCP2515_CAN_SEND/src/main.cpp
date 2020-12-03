#include <mcp_can.h>
#include <SPI.h>
 
//設定値
#define CS_PIN  (10) // CSを10ピンとする（変更可能）
#define INT_PIN (9)  // INTを9ピンとする（変更可能）
 
long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];

MCP_CAN CAN0(CS_PIN);      
 
void setup() {
    Serial.begin(115200);
    CAN0.begin(CAN_STDID, CAN_500KBPS, MCP_8MHZ); // CANの通信速度を500kbpsにする 
    pinMode(INT_PIN, INPUT); // 割り込みのためのピンを設定
    CAN0.setMode(MCP_NORMAL);
    Serial.println("MCP2515 Library Receive Example...");
}

byte data[8] = {'h', 'e', 'l', 'l', 'o','w', '0', '1'};

void loop(){
    //受信
    if(!digitalRead(INT_PIN)) {  // 受信割り込みが発生したら、CANデータをReadする
        CAN0.readMsgBuf(&rxId, &len, rxBuf);
        Serial.print("ID: ");
        Serial.print(rxId, HEX);
        Serial.print("  Data: ");
        for(int i = 0; i<len; i++) {
            if(rxBuf[i] < 0x10) {
                Serial.print("0");
            }
            Serial.print(rxBuf[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }


    //送信
    byte sndStat = CAN0.sendMsgBuf(0x111, 0, 8, data);
    if (sndStat != CAN_OK) {
        Serial.println("Error Sending Message...");
    }
    delay(1000);   // send data per 100ms
}
