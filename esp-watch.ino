#include "U8g2lib.h"
#include "FastLED.h"
#include "Time.h"
#include "Ds1302.h"

//---------Define the chip type----------//
//NOTE: Everything in ESP32 may not work (like sleeping),
//it is only here for the OLED pins in the simulation
//#define ESP32
#define ESP12

//----------Define the buttons-----------//
#define BUTTON_0 0 //PROG button
#define BUTTON_UP 15
#define BUTTON_DOWN 14
#define BUTTON_ENTER 13
#define BUTTON_BACK 12

//-----------Define the pins-------------//
#define DS1302_RST 2
#define WS2812B_PIN 1 //Neopixel pin
#define WAKE_PIN 16 //Used for keeping the watch awake
#define I2C_SCL 5
#define I2C_SDA 4
#define NUM_LEDS 3 //Neopixel count

#ifdef ESP32
#define INIT_WITH_SCL_SDA
#endif

CRGB leds[NUM_LEDS];

// DS1302 initalizing
Ds1302 rtc(DS1302_RST, I2C_SCL, I2C_SDA);


const static char* WeekDays[] =
{
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday",
  "Sunday"
};


void setup()
{
  FastLED.addLeds<NEOPIXEL, WS2812B_PIN>(leds, NUM_LEDS);

  Serial.begin(9600);

  // initialize the RTC
  rtc.init();

  // test if clock is halted and set a date-time (see example 2) to start it
  if (rtc.isHalted())
  {
    Serial.println("RTC is halted. Setting time...");
updateRTC(24,9,20,11,3,0,5);
  }
}


void loop()
{
  // get the current time
  Ds1302::DateTime now;
  rtc.getDateTime(&now);

  static uint8_t last_second = 0;
  if (last_second != now.second)
  {
    last_second = now.second;

    Serial.print("20");
    Serial.print(now.year);    // 00-99
    Serial.print('-');
    if (now.month < 10) Serial.print('0');
    Serial.print(now.month);   // 01-12
    Serial.print('-');
    if (now.day < 10) Serial.print('0');
    Serial.print(now.day);     // 01-31
    Serial.print(' ');
    Serial.print(WeekDays[now.dow - 1]); // 1-7
    Serial.print(' ');
    if (now.hour < 10) Serial.print('0');
    Serial.print(now.hour);    // 00-23
    Serial.print(':');
    if (now.minute < 10) Serial.print('0');
    Serial.print(now.minute);  // 00-59
    Serial.print(':');
    if (now.second < 10) Serial.print('0');
    Serial.print(now.second);  // 00-59
    Serial.println();
  }

  delay(100);
}