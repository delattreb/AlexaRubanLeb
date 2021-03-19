#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <GyverButton.h>
#include <EEPROM.h>
#include "WS2812FX.h"
#include <Espalexa.h>
#include "config.h"

WiFiClient wifiClient;
Espalexa espalexa;
GButton touch(PIN_BUTTON_1, LOW_PULL, NORM_OPEN);
WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

//callback function prototype
void colorLightChanged(uint8_t brightness, uint32_t rgb);

bool bbright;
static int effect = 2;
static uint8_t bright = BRIGHTNESS; //0-255
static int speed = SPEED_EFFECT;
uint32_t red, green, blue;
// setup
void setup()
{
#ifdef SERIALLOG
	Serial.begin(SERIALBAUDS);
	while (!Serial)
		continue;
#endif
#ifdef INFO
	delay(5000);
	Serial.print("Core version: ");
	Serial.println(ESP.getCoreVersion());
	Serial.print("Sdk version: ");
	Serial.println(ESP.getSdkVersion());
	Serial.print("MAC: ");
	Serial.println(WiFi.macAddress());
#endif
	WiFiManager wifiManager;
	//Reset setting
	//wifiManager.resetSettings();
	wifiManager.setAPStaticIPConfig(IPAddress(10, 0, 0, 1), IPAddress(10, 0, 0, 1), IPAddress(255, 255, 255, 0));
#ifdef WIFIDEBUG
	wifiManager.setDebugOutput(true);
#else
	wifiManager.setDebugOutput(false);
#endif

	if (!wifiManager.autoConnect(NETWORKNAME))
	{
#ifdef DEBUG
		Serial.println("Failed to connect");
#endif
		delay(1000);
		ESP.reset();
		delay(5000);
	}
	// initialize EEPROM with predefined size
	EEPROM.begin(EEPROM_SIZE);

	//Input configuration
	pinMode(PIN_BUTTON_1, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_1), pinDidChange, CHANGE);
	touch.setDebounce(80);
	touch.setTimeout(200);
	bbright = false;

	if (EEPROM.read(EEPROM_PLACE_BRIGHT) >= 1)
	{
		bright = EEPROM.read(EEPROM_PLACE_BRIGHT);
#ifdef DEBUG
		Serial.print("Load EEPROM: ");
		Serial.println(bright);
#endif
	}
	else
		bright = BRIGHTNESS;

	//Define Alexa device name
	espalexa.addDevice(DEVICE_NAME, colorLightChanged);
	espalexa.begin();

	//WS2812
	ws2812fx.init();
	ws2812fx.setColor(PURPLE);
	ws2812fx.setBrightness(bright);
	ws2812fx.setMode(effect);
	ws2812fx.setSpeed(speed);
	ws2812fx.start();
}

// loop
void loop()
{
	//Alexa
	espalexa.loop();
	delay(1);

	//WS2812
	ws2812fx.service();

	//Button gesture
	touch.tick();

	if (touch.isTriple())
	{
#ifdef DEBUG
		Serial.print("Save EEPROM: ");
		Serial.println(bright);
#endif
		EEPROM.write(EEPROM_PLACE_BRIGHT, bright);
		EEPROM.commit();
	}
	/*
	if (touch.isStep())
	{
		if (bbright)
		{
			if (bright + INCREMENT < 255)
				bright = bright + INCREMENT;
		}
		else
		{
			if (bright - INCREMENT > 5)
				bright = bright - INCREMENT;
		}
#ifdef DEBUG
		Serial.print("bright: ");
		Serial.println(bright);
#endif
	}
	if (touch.isRelease())
	{
		bbright = !bbright;
#ifdef DEBUG
		Serial.println("Change");
#endif
	}*/
}

//pinDidChange
ICACHE_RAM_ATTR void pinDidChange()
{
	touch.tick();
}

//the color device callback function has two parameters
void colorLightChanged(uint8_t brightness, uint32_t rgb)
{
	red = (rgb >> 16) & 0xFF;
	green = (rgb >> 8) & 0xFF;
	blue = rgb & 0xFF;

	ws2812fx.stop();
	ws2812fx.setBrightness(uint8_t(brightness));
	ws2812fx.start();

#ifdef DEBUG
	Serial.print("Brightness: ");
	Serial.print(brightness);
	Serial.print(", rgb: ");
	Serial.print(rgb);
	Serial.print(", Red: ");
	Serial.print((rgb >> 16) & 0xFF); //get red component
	Serial.print(", Green: ");
	Serial.print((rgb >> 8) & 0xFF); //get green
	Serial.print(", Blue: ");
	Serial.println(rgb & 0xFF); //get blue#endif
#endif
}