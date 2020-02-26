
// Include Libraries
#include "Arduino.h"
#include "BLEHM10.h"
#include "Wire.h"
#include "SPI.h"
#include "Adafruit_SSD1306.h"
#include "Adafruit_GFX.h"
#include "Button.h"
#include "RTClib.h"

// Pin Definitions
#define BLEHM10_PIN_TXD 3
#define BLEHM10_PIN_RXD 10
#define OLED128X64_PIN_RST 5
#define OLED128X64_PIN_DC 4
#define OLED128X64_PIN_CS 2
#define PUSHBUTTON_1_PIN_2 6
#define PUSHBUTTON_2_PIN_2 7
#define PUSHBUTTON_3_PIN_2 8

// Global variables and defines
String blehm10Str = "";
// object initialization
BLEHM10 blehm10(BLEHM10_PIN_RXD, BLEHM10_PIN_TXD);
#define SSD1306_LCDHEIGHT 64
Adafruit_SSD1306 oLed128x64(OLED128X64_PIN_DC, OLED128X64_PIN_RST, OLED128X64_PIN_CS);
Button pushButton_1(PUSHBUTTON_1_PIN_2);
Button pushButton_2(PUSHBUTTON_2_PIN_2);
Button pushButton_3(PUSHBUTTON_3_PIN_2);
RTC_PCF8523 rtcPCF;

// define vars for testing menu
const int timeout = 10000; //define timeout of 10 sec
char menuOption = 0;
long time0;

// Setup the essentials for your circuit to work. It runs first every time your circuit is powered with electricity.
void setup()
{
    // Setup Serial which is useful for debugging
    // Use the Serial Monitor to view printed messages
    Serial.begin(9600);
    while (!Serial)
        ; // wait for serial port to connect. Needed for native USB
    Serial.println("start");

    blehm10.begin(9600);
    blehm10.println("BLE On....");
    oLed128x64.begin(SSD1306_SWITCHCAPVCC); // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
    oLed128x64.clearDisplay();              // Clear the buffer.
    oLed128x64.display();
    pushButton_1.init();
    pushButton_2.init();
    pushButton_3.init();
    if (!rtcPCF.begin())
    {
        Serial.println("Couldn't find RTC");
        while (1)
            ;
    }
    if (!rtcPCF.initialized())
    {
        Serial.println("RTC lost power, lets set the time!");
        // following line sets the RTC to the date & time this sketch was compiled
        rtcPCF.adjust(DateTime(F(__DATE__), F(__TIME__)));
        // This line sets the RTC with an explicit date & time, for example to set
        // January 21, 2014 at 3am you would call:
        // rtcPCF.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }
    menuOption = menu();
}

// Main logic of your circuit. It defines the interaction between the components you selected. After setup, it runs over and over again, in an eternal loop.
void loop()
{

    if (menuOption == '1')
    {
        // HM-10 BLE Bluetooth 4.0 - Test Code
        //Receive String from bluetooth device
        if (blehm10.available())
        {
            //Read a complete line from bluetooth terminal
            blehm10Str = blehm10.readStringUntil('\n');
            // Print raw data to serial monitor
            Serial.print("BT Raw Data: ");
            Serial.println(blehm10Str);
        }
        //Send sensor data to Bluetooth device
        blehm10.println("PUT YOUR SENSOR DATA HERE");
    }
    else if (menuOption == '2')
    {
        // Monochrome 1.3 inch 128x64 OLED graphic display - Test Code
        oLed128x64.setTextSize(1);
        oLed128x64.setTextColor(WHITE);
        oLed128x64.setCursor(0, 10);
        oLed128x64.clearDisplay();
        oLed128x64.println("Circuito.io Rocks!");
        oLed128x64.display();
        delay(1);
        oLed128x64.startscrollright(0x00, 0x0F);
        delay(2000);
        oLed128x64.stopscroll();
        delay(1000);
        oLed128x64.startscrollleft(0x00, 0x0F);
        delay(2000);
        oLed128x64.stopscroll();
    }
    else if (menuOption == '3')
    {
        bool pushButton_1Val = pushButton_1.read();
        Serial.print(F("Val: "));
        Serial.println(pushButton_1Val);
    }
    else if (menuOption == '4')
    {
        bool pushButton_2Val = pushButton_2.read();
        Serial.print(F("Val: "));
        Serial.println(pushButton_2Val);
    }
    else if (menuOption == '5')
    {

        bool pushButton_3Val = pushButton_3.read();
        Serial.print(F("Val: "));
        Serial.println(pushButton_3Val);
    }
    else if (menuOption == '6')
    {

        DateTime now = rtcPCF.now();
        Serial.print(now.month(), DEC);
        Serial.print('/');
        Serial.print(now.day(), DEC);
        Serial.print('/');
        Serial.print(now.year(), DEC);
        Serial.print("  ");
        Serial.print(now.hour(), DEC);
        Serial.print(':');
        Serial.print(now.minute(), DEC);
        Serial.print(':');
        Serial.print(now.second(), DEC);
        Serial.println();
        delay(1000);
    }

    if (millis() - time0 > timeout)
    {
        menuOption = menu();
    }
}

char menu()
{

    Serial.println(F("\nWhich component would you like to test?"));
    Serial.println(F("(1) HM-10 BLE Bluetooth 4.0"));
    Serial.println(F("(2) Monochrome 1.3 inch 128x64 OLED graphic display"));
    Serial.println(F("(3) Mini Pushbutton Switch #1"));
    Serial.println(F("(4) Mini Pushbutton Switch #2"));
    Serial.println(F("(5) Mini Pushbutton Switch #3"));
    Serial.println(F("(6) Adafruit PCF8523 Real Time Clock Assembled Breakout Board"));
    Serial.println(F("(menu) send anything else or press on board reset button\n"));
    while (!Serial.available())
        ;

    // Read data from serial monitor if received
    while (Serial.available())
    {
        char c = Serial.read();
        if (isAlphaNumeric(c))
        {

            if (c == '1')
                Serial.println(F("Now Testing HM-10 BLE Bluetooth 4.0"));
            else if (c == '2')
                Serial.println(F("Now Testing Monochrome 1.3 inch 128x64 OLED graphic display"));
            else if (c == '3')
                Serial.println(F("Now Testing Mini Pushbutton Switch #1"));
            else if (c == '4')
                Serial.println(F("Now Testing Mini Pushbutton Switch #2"));
            else if (c == '5')
                Serial.println(F("Now Testing Mini Pushbutton Switch #3"));
            else if (c == '6')
                Serial.println(F("Now Testing Adafruit PCF8523 Real Time Clock Assembled Breakout Board"));
            else
            {
                Serial.println(F("illegal input!"));
                return 0;
            }
            time0 = millis();
            return c;
        }
    }
}
