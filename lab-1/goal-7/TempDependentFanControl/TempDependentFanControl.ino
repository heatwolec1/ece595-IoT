/*
 * This code is nearly identical to the example code provided by Adafruit for the
 * SHTC3 sensor. My modications include calculating the temperature in Fahrenheit,
 * sending the data to the adafruit.io servers, and replacing the delay function
 * calls with system time differences.
*/

#include "Adafruit_GFX.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "Adafruit_SHTC3.h"
#include "Adafruit_SSD1306.h"
#include "SPI.h"
#include "WiFi.h"
#include "Wire.h"

// Globals
// Wifi
#define NETWORK_SSID     "Cisco04646"
#define NETWORK_PASSWORD ""
WiFiClient client;	// client object to connect to adafruit's MQTT server

// OLED screen
#define SCREEN_UPDATE_INTERVAL 1000	// interval between display text updates, in milliseconds
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define CHARACTER_WIDTH 6
#define CHARACTER_HEIGHT 8
#define OLED_MOSI  18	// start OLED screen pin definitions
#define OLED_CLK    5
#define OLED_DC    21
#define OLED_CS    17
#define OLED_RESET  4	// end OLED screen pin definitions
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
	OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);	// OLED screen object

// SHTC3 sensor
#define MEASUREMENT_INTERVAL 10000			// interval between measurements, in milliseconds
Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();	// sensor object
sensors_event_t humidity, temp;				// sensor value objects

// Adafruit.io setup
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "heatwolec1"
#define AIO_KEY         ""
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
// feeds:
Adafruit_MQTT_Publish temperatureC = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");
Adafruit_MQTT_Publish temperatureF = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature-f");
Adafruit_MQTT_Publish humidityTopic = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity");
Adafruit_MQTT_Publish fanStateTopic = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/fan-state");

// Other
#define CONNECTION_TEST_INTERVAL 1000		// interval between wifi and mqtt connection tests, in milliseconds
unsigned long prevMillisMeasurement, prevMillisConnectionTest, prevMillisScreenUpdate, curMillis;		// timer values for creating delays
float outsideTemp, outsideHumidity;
boolean fanState = false;

float celsiusToFahrenheit(float tempC) {
	return (tempC * 1.8) + 32;
}

boolean setFanState(float insideTemperature, float outsideTemperature) {
	// TODO: move cold air in if insideTemperature is above some threshold, otherwise move warm air in
	return (insideTemperature > outsideTemperature);
}

void setup() {
	// Initialize error handling pin
	pinMode(LED_BUILTIN, OUTPUT);

	// Initialize timer check previous values
	prevMillisMeasurement = 0;
	prevMillisConnectionTest = 0;
	prevMillisScreenUpdate = 0;

	// Try to open a serial connection
	// don't proceed until it is successful
	Serial.begin(115200);
	while (!Serial)
		delay(10);     // 10 ms is short enough that delay function isn't problematic

	// Initialize the display
	if (!display.begin(SSD1306_SWITCHCAPVCC)) fatalHandler();
	display.clearDisplay(); display.display();
	display.setTextSize(1);
	display.setTextColor(SSD1306_WHITE);
	display.setCursor(0, 0);
	display.cp437(true);

	// Try to connect to the SHTC3 sensor
	// don't proceed unless it is successful
	if (!shtc3.begin()) {
		display.println("Couldn't find SHTC3");
		display.display();
		while (1) delay(1);	// infinite loop, delay isn't stopping anything else from occurring
	}
}

void loop() {
	// Update timer check current value
	curMillis = millis();

	// Check connections
	if (curMillis-prevMillisConnectionTest >= CONNECTION_TEST_INTERVAL) {
		// Update timer check previous value
		prevMillisConnectionTest = curMillis;

		// Check connections
		WiFi_connect();
		MQTT_connect();

		// This block took some time, update timer check current value again
		curMillis = millis();
	}

	// Sensor value read/publish
	if (curMillis-prevMillisMeasurement >= MEASUREMENT_INTERVAL) {
		// Update timer check previous value
		prevMillisMeasurement = curMillis;

		// Read in fresh data from the sensor and publish data to Adafruit MQTT server
		shtc3.getEvent(&humidity, &temp);
		if (!temperatureC.publish(temp.temperature))
			Serial.println(F("Publishing temperature (C) failed!"));
		if (!temperatureF.publish(celsiusToFahrenheit(temp.temperature)))
			Serial.println(F("Publishing temperature (F) failed!"));
		if (!humidityTopic.publish(humidity.relative_humidity))
			Serial.println(F("Publishing humidity failed!"));

		// Read the local outside weather measurements
		outsideTemp = 1234;
		outsideHumidity = 7890;

		// Set fan state and publish to Adafruit MQTT server
		fanState = setFanState(temp.temperature, outsideTemp);
		if (!fanStateTopic.publish(fanState))
			Serial.println(F("Publishing fan state failed!"));

		// This block took some time, update timer check current value again
		curMillis = millis();
	}

	// Update display text
	if (curMillis-prevMillisScreenUpdate >= SCREEN_UPDATE_INTERVAL) {
		// Update timer check previous value
		prevMillisScreenUpdate = curMillis;

		// Update the display text
		screenUpdate();

		// This block took some time, update timer check current value again
		curMillis = millis();
	}

}

void WiFi_connect() {

	// Stop if already connected
	int status = WiFi.status();
	if (status == WL_CONNECTED) {
		return;
	}

	// Try to open a wifi connection
	// don't proceed until it is successful
	display.clearDisplay(); display.setCursor(0, 0);
	display.println(F("Connecting to:"));
	display.print(F("  ")); display.print(F(NETWORK_SSID));
	display.display();
	while (status != WL_CONNECTED) {
		status = WiFi.begin(NETWORK_SSID, NETWORK_PASSWORD);
		display.print(F("."));
		display.display();
		delay(1000);	// delay is OK here since we can't do anything without a wifi connection
	}
}

void MQTT_connect() {
	// This function is borrowed from the ESP8266 MQTT example provided by
	// Adafruit in the Adafruit_MQTT library.
	int8_t ret;

	// Stop if already connected.
	if (mqtt.connected()) {
		return;
	}

	Serial.print("Connecting to MQTT... ");

	uint8_t retries = 3;
	while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
		Serial.println(mqtt.connectErrorString(ret));
		Serial.println("Retrying MQTT connection in 5 seconds...");
		mqtt.disconnect();
		delay(5000);  // wait 5 seconds
		//if (--retries == 0) fatalHandler();
	}
	Serial.println("MQTT Connected!");
}

void screenUpdate() {
	// Initialize display buffer
	display.clearDisplay();
	display.setCursor(0, 0);

	// Add the current IP address to the display buffer
	display.print(F("IP: ")); display.println(WiFi.localIP());

	// Add the current temp and humidity (inside) to the display buffer
	display.println();
	display.print(F("Inside Temp: ")); display.println(temp.temperature);

	// Add the current temp and humidity (outside) to the display buffer
	display.println();
	display.print(F("Outside Temp: ")); display.println(outsideTemp);

	// Add the fan status to the display buffer
	display.println();
	display.print(F("Fan Status: ")); display.println(fanState ? "On" : "Off");

	// Write display buffer to screen
	display.display();
}

void fatalHandler() {
	// Infinitely loop, flashing the built-in led
	while (true) {
		digitalWrite(LED_BUILTIN, HIGH);
		delay(50);	// looping here infinitely, so using a delay doesn't affect anything significant
		digitalWrite(LED_BUILTIN, LOW);
		delay(50);
	}
}
