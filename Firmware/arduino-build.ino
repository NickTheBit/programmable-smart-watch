#include "BluetoothSerial.h"
#include "Arduino_JSON.h"
#include <RTClib.h>
#include <SPI.h>
#include "ER-ERM0154-1.h"
#include "imagedata.h"
#include "epdpaint.h"
#include <Wire.h>
#include <stdio.h>

#define COLORED     0
#define UNCOLORED   1

const int Left = 27;
const int Right = 14;
const int Ok = 12;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
JSONVar packet;
RTC_DS3231 rtc;
DateTime now , prev;
// Display initialisation
Epd epd;

void display_time() {
  char time_string[25];
  sprintf(time_string,"%d:%d",now.hour(),now.minute());
  epd.ClearFrame();
  unsigned char image[1024];
  Paint paint(image, 152, 18);    //width should be the multiple of 8 
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(30, 2, time_string, &Font24, COLORED);
  epd.SetPartialWindowBlack(paint.GetImage(), 0, 3, paint.GetWidth(), paint.GetHeight());
  epd.DisplayFrame();
}

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

  if (! Wire.begin(22,19))
    Serial.println("Wire fucked up");

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
  }
  //  rtc.adjust(DateTime(2020, 5, 3, 9, 31, 0));

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, recalibration required");
  }

  // Boot screen
  epd.ClearFrame();
  unsigned char image[1024];
  Paint paint(image, 152, 24);    //width should be the multiple of 8 
  paint.Clear(UNCOLORED);
  paint.DrawRectangle(1,1,151,23,COLORED);
  paint.DrawStringAt(50, 3, "PSW", &Font24, COLORED);
  epd.SetPartialWindowRed(paint.GetImage(), 0, 3, paint.GetWidth(), paint.GetHeight());
  epd.DisplayFrame();
  now = rtc.now();
  prev = rtc.now();
  display_time();
}

void loop() {
  now = rtc.now();
  if (now.unixtime() - prev.unixtime() >= 60) {
    prev = rtc.now();
    display_time();
  }
  
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
