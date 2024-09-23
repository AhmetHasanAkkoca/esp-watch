
//---------Define the chip type----------//
//NOTE: Everything in ESP32 may not work (like sleeping),
//it is only here for the OLED pins in the simulation
//#define ESP32
#define ESP12

//----------Define the buttons-----------//
#define BUTTON_0 0  //PROG button
#define BUTTON_UP 15
#define BUTTON_DOWN 14
#define BUTTON_ENTER 13
#define BUTTON_BACK 12

//-----------Define the pins-------------//
#define DS1302_RST 2
#define WS2812B_PIN 1  //Neopixel pin
#define WAKE_PIN 16    //Used for keeping the watch awake
#define I2C_SCL 5
#define I2C_SDA 4
#define NUM_LEDS 2  //Neopixel count

//----------Define boot settings---------//
#define QUICK_WAKE  //If quick wake is defined, it will use the values below to speed the wake up

#ifdef QUICK_WAKE  //Checks if it is defined
#define language 0  //Copy the contents of the preferred language into language.ino file
#define brightness 255  //Set the value between 0-255
#define style 0  //default style
#define hour24 true  //Checks 12/24 hour style; if true, uses 24 hours.