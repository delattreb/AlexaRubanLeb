//DEBUG INFORMATION
//#define SERIALLOG
//#define INFO
//#define DEBUG
//#define WIFIDEBUG

// Serial configuration
#define SERIALBAUDS 115200

// Alexa device
#define DEVICE_NAME "Ruban1"
#define LED_EFFECT FX_MODE_FIREWORKS
//ESP8266 configuration
#define NETWORKNAME "ESPRuban1"
#define ATTENPTING 1000

// Led configuration
#pragma region LED_CONFIGURATION
#define LED_PIN 4
#define LED_COUNT 30
#define SPEED_EFFECT 3500
#define BRIGHTNESS 255 //0-255
#define RED 255
#define GREEN 0
#define BLUE 0
#pragma endregion

// BTN
#pragma region BTN_CONFIGURATION
#define PIN_BTN 5
#define EFFECT_MAX 9
#pragma endregion

//Save Data EEPROM
#pragma region SAVE_DATA
#define EEPROM_SIZE 5
#define EEPROM_START 0
#pragma endregion