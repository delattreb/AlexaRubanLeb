#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <GyverButton.h>
#include <EEPROM.h>
#include "WS2812FX.h"
#include <Espalexa.h>
#include "config.h"

WiFiClient wifiClient;
GButton touch(PIN_BUTTON_TOUCH, LOW_PULL, NORM_OPEN);
WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

//callback functions
void deltaChanged(EspalexaDevice *dev);
Espalexa espalexa;
EspalexaDevice *epsilon;

static int effect = LED_EFFECT;
static uint8_t bright = BRIGHTNESS;
static int speed = SPEED_EFFECT;
uint8_t red = RED, green = GREEN, blue = BLUE;

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
	//BuiltIn LED
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);	 // turn the LED off by making the voltage LOW
	//Input configuration
	pinMode(PIN_BUTTON_TOUCH, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_TOUCH), pinDidChange, CHANGE);
	touch.setDebounce(80);
	touch.setTimeout(200);

	if (EEPROM.read(EEPROM_PLACE_BRIGHT) >= 1)
	{
		bright = EEPROM.read(EEPROM_PLACE_BRIGHT);
		red = EEPROM.read(EEPROM_PLACE_RED);
		green = EEPROM.read(EEPROM_PLACE_GREEN);
		blue = EEPROM.read(EEPROM_PLACE_BLUE);

#ifdef DEBUG
		Serial.print("Load EEPROM: ");
		Serial.print("Bright");
		Serial.print(bright);
		Serial.print("R");
		Serial.print(red);
		Serial.print("G");
		Serial.print(green);
		Serial.print("B");
		Serial.println(blue);
#endif
	}

	// Define your devices here.
	espalexa.addDevice(DEVICE_NAME, deltaChanged, EspalexaDeviceType::color);
	espalexa.begin();

	//WS2812
	ws2812fx.init();
	ws2812fx.setColor(red, green, blue);
	ws2812fx.setBrightness(bright);
	ws2812fx.setMode(effect);
	ws2812fx.setSpeed(speed);
	ws2812fx.start();
}

// loop
void loop()
{
	//Button gesture
	touch.tick();

	//Alexa
	espalexa.loop();
	delay(1);

	//WS2812
	ws2812fx.service();

	if (touch.isDouble())
	{
#ifdef DEBUG
		Serial.print("Save EEPROM: ");
		Serial.print("Bright");
		Serial.print(bright);
		Serial.print("R");
		Serial.print(red);
		Serial.print("G");
		Serial.print(green);
		Serial.print("B");
		Serial.println(blue);
#endif
		EEPROM.write(EEPROM_PLACE_BRIGHT, bright);
		EEPROM.write(EEPROM_PLACE_RED, red);
		EEPROM.write(EEPROM_PLACE_GREEN, green);
		EEPROM.write(EEPROM_PLACE_BLUE, blue);
		EEPROM.commit();
		saveLed();
	}
}

//pinDidChange
ICACHE_RAM_ATTR void pinDidChange()
{
	touch.tick();
}

//Callback
void deltaChanged(EspalexaDevice *d)
{
	if (d == nullptr)
		return;
	if (d->getName() == DEVICE_NAME)
	{
		//Get color
		bright = d->getValue();
		red = d->getR();
		green = d->getG();
		blue = d->getB();

		//Set new color
		ws2812fx.stop();
		ws2812fx.setColor(red, green, blue);
		ws2812fx.setBrightness(bright);
		ws2812fx.start();
#ifdef DEBUG
		Serial.print("Value: ");
		Serial.print(bright);
		Serial.print(", color R");
		Serial.print(red);
		Serial.print(", G");
		Serial.print(green);
		Serial.print(", B");
		Serial.println(blue);
#endif
	}
}

// Blink after saving data 
void saveLed()
{
	digitalWrite(LED_BUILTIN, LOW); // turn the LED on (HIGH is the voltage level)
	delay(1000);					 // wait for a second
	digitalWrite(LED_BUILTIN, HIGH);	 // turn the LED off by making the voltage LOW
}