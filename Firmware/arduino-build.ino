#include "BluetoothSerial.h"
#include "Arduino_JSON.h"

const int PushButton = 16;

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

JSONVar packet;

void setup() {
  pinMode(PushButton, INPUT);
  Serial.begin(115200);
  SerialBT.begin("PSW"); //Bluetooth device name
  Serial.println("The device online, standing by...");
}

void loop() {
  // Preparing json packet
  packet["left"] = 0;
  packet["right"] = 0;

  int update_flag = 0;
  
  int Push_button_state = digitalRead(PushButton);
  
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    Serial.write(SerialBT.read());
  }
  
  if (Push_button_state == HIGH) {
    update_flag = 1;
    Serial.write("Button pressed\n");
    SerialBT.println("Button pressed");
    packet["left"] = 1;
    delay(300);
  }
  delay(20);
  if (update_flag==1) {
    SerialBT.println(packet);
    Serial.println(packet);
  }
}
