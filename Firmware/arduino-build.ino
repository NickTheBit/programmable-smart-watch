#include "BluetoothSerial.h"
#include "Arduino_JSON.h"
#include <RTClib.h>

const int Left = 15;
const int Right = 17;
const int Ok = 13;

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

JSONVar packet;

void setup() {
  pinMode(Left, INPUT);
  pinMode(Ok, INPUT);
  pinMode(Right, INPUT);
  
  Serial.begin(115200);
  SerialBT.begin("PSW"); //Bluetooth device name
  Serial.println("The device online, standing by...");
}

void loop() {
  // Preparing json packet
  packet["left"] = 0;
  packet["ok"] = 0;
  packet["right"] = 0;

  int update_flag = 0;
  
  int L_Button = digitalRead(Left);
  int R_Button = digitalRead(Right);
  int O_Button = digitalRead(Ok);
  
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    Serial.write(SerialBT.read());
  }
  
  if (L_Button == HIGH) {
    update_flag = 1;
    packet["left"] = 1;
    delay(50);
  }
  if (R_Button == HIGH) {
    update_flag = 1;
    packet["right"] = 1;
    delay(50);
  }
  if (O_Button == HIGH) {
    update_flag = 1;
    packet["ok"] = 1;
    delay(50);
  }
  
  delay(300);
  if (update_flag==1) {
    SerialBT.println(packet);
    Serial.println(packet);
  }
}
