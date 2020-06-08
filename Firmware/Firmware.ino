#include <BluetoothSerial.h>
#include <GxFont_GFX.h>
#include <ArduinoJson.h>

#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMono24pt7b.h>


#include <GxEPD.h>
#include <GxGDEH0154D67/GxGDEH0154D67.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
#include <SPI.h>
#include <Wire.h>
#include <stdio.h>
#include <RTClib.h>

#define COLORED     0
#define UNCOLORED   1

const int Left = 27;
const int Right = 14;
const int Ok = 12;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

GxIO_Class io(SPI, SS, 17, /*RST=*/ 16); // arbitrary selection of 17, 16
GxEPD_Class display(io, /*RST=*/ 16, /*BUSY=*/ 4); // arbitrary selection of (16), 4

BluetoothSerial SerialBT;
const int capacity = JSON_OBJECT_SIZE(64);
StaticJsonDocument <capacity> packet;

RTC_DS3231 rtc;
DateTime now , prev;
// Display initialisation

void display_time() {
  char time_string[25];
  sprintf(time_string,"%d:%d",now.hour(),now.minute());
  display.print(time_string);
  display.update();
}

void setup() {
  Serial.begin(115200);
  // Wire.begin(I2C_SDA, I2C_SCL, 100000);
  Serial.println("setup");
  display.init(115200); // enable diagnostic output on Serial
  Serial.println("setup done");
  
  pinMode(Left, INPUT);
  pinMode(Ok, INPUT);
  pinMode(Right, INPUT);
  
  SerialBT.begin("PSW"); //Bluetooth device name
  

  if (! Wire.begin(22,19))
    Serial.println("Wire fucked up");

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
  } 

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, recalibration required");
    rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));
  }

  // Boot screen
  display.fillScreen(GxEPD_WHITE);
  display.setFont(&FreeMono24pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setRotation(0);
  display.print("Hello World");
  display.update();
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
//    SerialBT.write(packet);/
//    Serial.println(packet);
  }
}
