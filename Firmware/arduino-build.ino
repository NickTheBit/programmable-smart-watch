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

// Display initialisation
Epd epd;

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

JSONVar packet;

void setup() {
  Serial.begin(115200);
  // Wire.begin(I2C_SDA, I2C_SCL, 100000);
  
  pinMode(Left, INPUT);
  pinMode(Ok, INPUT);
  pinMode(Right, INPUT);
  
  SerialBT.begin("PSW"); //Bluetooth device name

  if (epd.Init() != 0) {
    Serial.println("e-Paper init failed");
    return;
  } else {
    Serial.println("e-Paper initiated");
  }
  epd.ClearFrame();
  unsigned char image[1024];
  Paint paint(image, 152, 18);    //width should be the multiple of 8 

  paint.Clear(UNCOLORED);
  paint.DrawStringAt(8, 2, "PSW", &Font24, COLORED);
  epd.SetPartialWindowBlack(paint.GetImage(), 0, 3, paint.GetWidth(), paint.GetHeight());
  epd.DisplayFrame();
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
