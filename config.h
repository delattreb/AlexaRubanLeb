//DEBUG INFORMATION
//#define SERIALLOG
//#define INFO
//#define DEBUG
//#define WIFIDEBUG

// Serial configuration
#define SERIALBAUDS 115200

//Alexa device
#define DEVICE_NAME "Ruban1"

//ESP8266 configuration
#define NETWORKNAME "ESPRuban1"
#define ATTENPTING 1000

//Led configuration
#pragma region LED_CONFIGURATION
#define LED_PIN 4
#define PIN_BUTTON_TOUCH 12

#define LED_COUNT 30
#define SPEED_EFFECT 3500
#define BRIGHTNESS 150
#define RED 255
#define GREEN 0
#define BLUE 0
#pragma endregion

//Save Data EEPROM
#pragma region SAVE_DATA
#define EEPROM_SIZE 1
#define EEPROM_PLACE_BRIGHT 0
#define EEPROM_PLACE_RED 1
#define EEPROM_PLACE_GREEN 2
#define EEPROM_PLACE_BLUE 3
#pragma endregion