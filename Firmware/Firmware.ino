#include <BluetoothSerial.h>
#include <GxFont_GFX.h>
#include <ArduinoJson.h>

#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>


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

char daysOfTheWeek[7][12] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday","Sunday"};

int battery_level = 98;
float temp;
String bt_status;

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

void display_status(int=0,String="",String="Empty");

void print02d(uint32_t d)
{
  if (d < 10) display.print("0");
  display.print(d);
}

void boot_screen() {
  display.fillScreen(GxEPD_BLACK);
  display.update();
  uint16_t box_x = 15;
  uint16_t box_y = 15;
  uint16_t box_w = 170;
  uint16_t box_h = 2000;
  uint16_t cursor_y = box_y + 20;
  display.setCursor(box_x, cursor_y);
  display.setFont(&FreeMonoBold18pt7b);
  display.setTextColor(GxEPD_WHITE);
  display.setRotation(3);
  display.print("   PSW");
  display.setFont(&FreeMonoBold9pt7b);
  display.print("\n\n   Powered by:");
  display.setFont(&FreeMonoBold18pt7b);
  display.print("\n gloub.os");
  display.update();
  // Splash screen duration, should be lower
  delay(2000);
  display.fillScreen(GxEPD_WHITE);
  display.update();
  now = rtc.now();
  prev = rtc.now();
  display.setTextColor(GxEPD_BLACK);
  display_date();
  display_time();
  display_status(0,"","Status");
}

void display_time() {
  display.setFont(&FreeMonoBold18pt7b);
  uint16_t box_x = 15;
  uint16_t box_y = 5;
  uint16_t box_w = 170;
  uint16_t box_h = 25;
  uint16_t cursor_y = box_y + 23;
  display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
  display.setCursor(box_x, cursor_y);
  print02d(now.hour());
  display.print(":");
  print02d(now.minute());
//  display.print(":");
//  print02d(now.second());
  display.updateWindow(box_x, box_y, box_w, box_h, true);
  delay(30);
}

void display_date() {
  display.setFont(&FreeMonoBold9pt7b);
  uint16_t box_x = 20;
  uint16_t box_y = 20;
  uint16_t box_w = 170;
  uint16_t box_h = 25;
  uint16_t cursor_y = box_y + 23;
  display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
  display.setCursor(box_x, cursor_y);
  print02d(now.day());
  display.print("/");
  print02d(now.month());
  display.print("/");
  print02d(now.year());
  display.updateWindow(box_x, box_y, box_w, box_h, true);
  horizontal_line(46);
}

void display_status(int option,String msg,String title) {
  int i;
  display.setFont(&FreeMonoBold9pt7b);
  uint16_t box_x = 3;
  uint16_t box_y = 47;
  uint16_t box_w = 200;
  uint16_t box_h = 140;
  uint16_t cursor_y = box_y + 20;
  display.setCursor(box_x, cursor_y);
  display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
  
  // Setting title and placing
  int spacers = (18-title.length())/2;
  for (i=0; i<spacers; i++)
    display.print("-");
  display.print(title);
  for (i=spacers + title.length(); i<18; i++)
    display.print("-");
  
  switch (option) {
    case 0: {
      display.print("battery: ");
      display.print(battery_level);
      display.print("\nbluetooth: ");
      display.print(bt_status);
      display.print("\ntemp: ");
      display.print(rtc.getTemperature());
      display.print("\nbutton 1: ok\n");
      display.print("button 2: ok");
      break;
    }
    case 1: {
      display.print(msg);
      break;
    }
  }
  
//  display.updateWindow(box_x, box_y, box_w, box_h, true);
  display.update();
}

void horizontal_line(uint16_t box_y) {
  uint16_t box_x = 1;
  uint16_t box_w = 200;
  uint16_t box_h = 1;
  display.fillRect(box_x, box_y, box_w, box_h, GxEPD_BLACK);
  display.update();
}

void setup() {
  Serial.begin(115200);
  // Wire.begin(I2C_SDA, I2C_SCL, 100000);
  Serial.println("setup");
  display.init(0); // enable diagnostic output on Serial
  Serial.println("setup done");
  
  pinMode(Left, INPUT);
  pinMode(Ok, INPUT);
  pinMode(Right, INPUT);
  
  if (!SerialBT.begin("PSW"))
    bt_status = "Disabled";
  else
    bt_status = "Enabled";
  
  if (! Wire.begin(22,19))
    Serial.println("I2C Failed");

  if (! rtc.begin()) {
    Serial.println("RTC failed");
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, recalibration required connect to serial");
    rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));
  }
  boot_screen();
}

void loop() {
  // Clock intervals
  now = rtc.now();
  if (now.unixtime() - prev.unixtime() > 60) {
    prev = rtc.now();
    display_time();
    display_status(0,"","status");
  }

  // Preparing json packet
  packet["left"] = 0;
  packet["ok"] = 0;
  packet["right"] = 0;

  int update_flag = 0;
  
  int L_Button = digitalRead(Left);
  int R_Button = digitalRead(Right);
  int O_Button = digitalRead(Ok);
  
  
  if (L_Button == HIGH) {
    update_flag = 1;
    packet["left"] = 1;
    delay(200);
  }
  if (R_Button == HIGH) {
    update_flag = 1;
    packet["right"] = 1;
    delay(200);
  }
  if (O_Button == HIGH) {
    update_flag = 1;
    packet["ok"] = 1;
    delay(200);
    
  }

  if (Serial.available() > 0) {
    String buff = Serial.readString();
    Serial.print(buff);
    SerialBT.print(buff);
    display_status(1,buff,"Serial");
    delay(2000);
  }
  if (SerialBT.available()) {
    String buff = SerialBT.readString();
    Serial.print(buff);
    display_status(1,buff,"SerialBT");
    delay(2000);
  }

  if (update_flag==1) {
    serializeJson(packet, SerialBT);
    SerialBT.println();
    serializeJson(packet, Serial);
    Serial.println();
  }
}
