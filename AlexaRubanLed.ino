#include <arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <GyverButton.h>
#include <EEPROM.h>
#include <WS2812FX.h>
#include <Espalexa.h>
#include "config.h"

WiFiClient wifiClient;
GButton touch(PIN_BTN, LOW_PULL, NORM_OPEN);
WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

//callback functions
void deltaChanged(EspalexaDevice *dev);
Espalexa espalexa;
EspalexaDevice *epsilon;

uint8_t effect = LED_EFFECT;
uint8_t bright = BRIGHTNESS;
uint8_t effects[EFFECT_MAX] = {FX_MODE_COLOR_WIPE, FX_MODE_RUNNING_LIGHTS, FX_MODE_COMET, FX_MODE_FIREWORKS, FX_MODE_TWINKLEFOX, FX_MODE_FADE, FX_MODE_SCAN, FX_MODE_DUAL_SCAN};
uint8_t red = RED, green = GREEN, blue = BLUE;
int cpt_effect = 0;
uint16_t speed = SPEED_EFFECT;

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
	Serial.print("Device name: ");
	Serial.println(DEVICE_NAME);
	Serial.print("Network name: ");
	Serial.println(NETWORKNAME);

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
	//BuiltIn LED
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH); // turn the LED off by making the voltage LOW
	//Input configuration
	pinMode(PIN_BTN, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(PIN_BTN), pinDidChange, CHANGE);
	touch.setDebounce(80);
	touch.setTimeout(200);

	load_EEPROM();

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

	if (touch.isSingle())
	{
		cpt_effect++;
		if (cpt_effect >= EFFECT_MAX)
			cpt_effect = 0;
		change_effect();
	}
	if (touch.isDouble())
	{
		cpt_effect--;
		if (cpt_effect < 0)
			cpt_effect = EFFECT_MAX - 1;
		change_effect();
	}

	if (touch.isTriple())
	{
		save_EEPROM();
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
		bright = uint8_t(d->getValue());
		red = d->getR();
		green = d->getG();
		blue = d->getB();

		//Set new color
		ws2812fx.stop();
		ws2812fx.setColor(red, green, blue);
		ws2812fx.setBrightness(bright);
		ws2812fx.start();
#ifdef DEBUG
		Serial.println("Alexa value");
		Serial.print("B: ");
		Serial.println(bright);
		Serial.print("R: ");
		Serial.println(red);
		Serial.print("G: ");
		Serial.println(green);
		Serial.print("B: ");
		Serial.println(blue);
#endif
	}
}

// Change LED effect
void change_effect()
{
#ifdef DEBUG
	Serial.print("Effect: ");
	Serial.println(cpt_effect);
#endif
	ws2812fx.stop();
	ws2812fx.setMode(effects[cpt_effect]);
	ws2812fx.start();
}

// Blink after saving data
void saveLed()
{
	for (int i = 0; i <= 3; i++)
	{
		digitalWrite(LED_BUILTIN, LOW); // turn the LED on (HIGH is the voltage level)
		delay(600);
		digitalWrite(LED_BUILTIN, HIGH); // turn the LED off by making the voltage LOW
		delay(600);
	}
}

// Save EEPROM
void save_EEPROM()
{
#ifdef DEBUG
	Serial.println("Save EEPROM");
	Serial.print("B: ");
	Serial.println(bright);
	Serial.print("R: ");
	Serial.println(red);
	Serial.print("G: ");
	Serial.println(green);
	Serial.print("B: ");
	Serial.println(blue);
	Serial.print("E: ");
	Serial.println(effects[cpt_effect]);
#endif
	EEPROM.begin(EEPROM_SIZE);
	int address = EEPROM_START;
	EEPROM.write(address, bright);
	address += sizeof(bright);
	EEPROM.write(address, red);
	address += sizeof(red);
	EEPROM.write(address, green);
	address += sizeof(green);
	EEPROM.write(address, blue);
	address += sizeof(blue);
	EEPROM.write(address, effects[cpt_effect]);
	EEPROM.end();
	EEPROM.commit();
	saveLed();
}

// Load EEPROM
void load_EEPROM()
{
	EEPROM.begin(EEPROM_SIZE);
	int address = EEPROM_START;
	bright = EEPROM.read(address);
	address += sizeof(bright);
	red = EEPROM.read(address);
	address += sizeof(red);
	green = EEPROM.read(address);
	address += sizeof(green);
	blue = EEPROM.read(address);
	address += sizeof(blue);
	effect = EEPROM.read(address);
	EEPROM.end();
#ifdef DEBUG
	Serial.println("Load EEPROM");
	Serial.print("B: ");
	Serial.println(bright);
	Serial.print("R: ");
	Serial.println(red);
	Serial.print("G: ");
	Serial.println(green);
	Serial.print("B: ");
	Serial.println(blue);
	Serial.print("E: ");
	Serial.println(effect);
#endif
}