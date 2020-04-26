#include "BluetoothSerial.h"
#include "Arduino_JSON.h"
#include <RTClib.h>
#include <SPI.h>
#include "ER-ERM0154-1.h"
#include "imagedata.h"
#include "epdpaint.h"
#include <Wire.h>

#define COLORED     0
#define UNCOLORED   1

const int Left = 15;
const int Right = 17;
const int Ok = 13;

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

TwoWire i2c = TwoWire(0);

BluetoothSerial SerialBT;

JSONVar packet;

int scanDevices(void) {
  Serial.println("\nI2C Scanner starting");
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    i2c.beginTransmission(address);
    error = i2c.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
  delay(10000);
}

void setup() {
  i2c.begin(12,14);
  pinMode(Left, INPUT);
  pinMode(Ok, INPUT);
  pinMode(Right, INPUT);
  
  Serial.begin(115200);
  SerialBT.begin("PSW"); //Bluetooth device name

  // Display initialisation
  Epd epd;
  if (epd.Init() != 0) {
    Serial.println("e-Paper init failed");
    return;
  }
  epd.ClearFrame();
}

void loop() {
  // Searching for screen
  Serial.println(SDA);
  Serial.println(SCL);
  scanDevices();
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
