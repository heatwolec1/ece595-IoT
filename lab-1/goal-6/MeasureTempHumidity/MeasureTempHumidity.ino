/*
 * This code is nearly identical to the example code provided by Adafruit for the
 * SHTC3 sensor. My modications include printing the temperature in Fahrenheit,
 * reformatting the text being printed to serial, and replacing the delay function
 * calls with system time differences.
 *
 * Rather than displaying the data on the Arduino serial monitor, I will be using
 * Node-RED to listen for and display the data.
 *
 * ALL MESSAGES BEING SENT OUT OVER SERIAL SHOULD HAVE A "#" AS THE FIRST CHARACTER
 * OF THE LINE UNLESS THE LINE CONTAINS SENSOR DATA!!!
*/

#include "Adafruit_GFX.h"
#include "Adafruit_SHTC3.h"
#include "Adafruit_SSD1306.h"
#include "SPI.h"
#include "WiFi.h"
#include "WiFiUdp.h"
#include "Wire.h"

// Globals
// Wifi
#define NETWORK_SSID     "Cisco04646"
#define NETWORK_PASSWORD ""
#define NODE_RED_HOST_PORT 12345				// Port on Node-RED server listening for messages
IPAddress NODE_RED_HOST_IP(192, 168, 1, 112);	// IP address of the Node-RED server
IPAddress local_IP(192, 168, 1, 140);			// IP address of the microcontroller
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
WiFiUDP udp;	// the UDP library class object

// OLED screen
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_MOSI  18	// start OLED screen pin definitions
#define OLED_CLK    5
#define OLED_DC    21
#define OLED_CS    17
#define OLED_RESET  4	// end OLED screen pin definitions
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
	OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);	// OLED screen object

// SHTC3 sensor
Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();	// sensor object
sensors_event_t humidity, temp;				// sensor value objects

// Other
unsigned long prevMillis, curMillis;		// timer values for creating delays
String csvOutput = "";						// final data output
char csvOutputChar[20];

float celsiusToFahrenheit(float tempC) {
	return (tempC * 1.8) + 32;
}

void setup() {
	// Initialize error handling pin
	pinMode(LED_BUILTIN, OUTPUT);

	// Initialize timer check previous value
	prevMillis = 0;

	// Try to open a serial connection
	// don't proceed until it is successful
	Serial.begin(115200);
	while (!Serial)
		delay(10);     // 10 ms is short enough that delay function isn't problematic
	Serial.print("# SHTC3 data logging\n");

	// Try to open a wifi connection
	// don't proceed until it is successful
	if (!WiFi.config(local_IP, gateway, subnet)) fatalHandler();
	WiFi.begin(NETWORK_SSID, NETWORK_PASSWORD);
	while (WiFi.status() != WL_CONNECTED) {
		delay(10);     // 10 ms is short enough that delay function isn't problematic
	}

	// Open UDP transfer buffer
	udp.begin(local_IP, NODE_RED_HOST_PORT);

	// Try to connect to the SHTC3 sensor
	// don't proceed unless it is successful
	if (! shtc3.begin()) {
		Serial.print("# Couldn't find SHTC3\n");
		while (1) delay(1);	// infinite loop, delay isn't stopping anything else from occurring
	}

	// Print out the CSV logging format (timestamps will be added in Node-RED)
	Serial.print("# CSV format:\n#\ttempC,tempF,humidity");
}

void loop() {
	// Update timer check current value
	curMillis = millis();
	if (curMillis-prevMillis >= 1000) {
		// Update timer check previous value
		prevMillis = curMillis;

		// Read in fresh data from the sensor
		shtc3.getEvent(&humidity, &temp);
		csvOutput = String(temp.temperature, 2) + ","
			+ String(celsiusToFahrenheit(temp.temperature), 2) + ","
			+ String(humidity.relative_humidity, 2);

		// If there is a wifi connection, send the data in UDP packets
		if (WiFi.status() == WL_CONNECTED) {
			csvOutput.toCharArray(csvOutputChar, csvOutput.length());
			udp.beginPacket(NODE_RED_HOST_IP, NODE_RED_HOST_PORT);
			udp.printf(csvOutputChar);
			udp.endPacket();
		}
	}
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

