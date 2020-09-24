/*
 * This code is nearly identical to the example code provided by Adafruit for the
 * SHTC3 sensor. My modications include printing the temperature in Fahrenheit
 * and reformatting the text being printed to the serial monitor.
*/

#include "Adafruit_SHTC3.h"
#include <Wire.h>
#include <WiFi.h>

// Globals
const char SSID[] = "Cisco04646";
const char PASSWD[] = "";	// TODO: DELETE THE PASSWORD BEFORE COMMITTING!!
WiFiClient esp32Client;
char url[] = "google.com";
Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();

float celsiusToFahrenheit(float tempC) {
	return (tempC * 1.8) + 32;
}

void setup() {
	// Establish serial connection
	Serial.begin(19200);
	while (!Serial)
		delay(10);     // will pause Zero, Leonardo, etc until serial console opens

	// Initialize wifi connection, don't proceed until successful
	Serial.print("Attempting to connect to "); Serial.print(SSID); Serial.println(" ...");
	WiFi.begin(SSID, PASSWD);
	int counter = 0;
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print(".. ");
		if (++counter >= 12) {
			Serial.println();
			counter = 0;
		}
		delay(100);
	}
	if (counter == 0) Serial.println("Connection successful!");
	else Serial.println("\nConnection successful!");

	// Connect to a server
	Serial.print("\nAttempting to connect to "); Serial.print(url); Serial.println(" ...");
	esp32Client.connect(url, 80);
	if (esp32Client.connected()) {
		Serial.print("Connected to "); Serial.println(url);
		// make request
		esp32Client.println("GET /search?q=arduino HTTP/1.0");	// TODO: look up how to format HTTP requests
		esp32Client.println();
		while (esp32Client.available()) {
			Serial.print(esp32Client.read());
		}
	}

	// Disconnect from the server
	esp32Client.stop();
	Serial.print("\nDisconnected from "); Serial.println(url);

//	Serial.println("SHTC3 test");
//	if (! shtc3.begin()) {
//		Serial.println("Couldn't find SHTC3");
//		while (1) delay(1);
//	}
//	Serial.println("Found SHTC3 sensor");
}

void loop() {
//	sensors_event_t humidity, temp;
//
//	shtc3.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
//
//	Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
//	Serial.print("Temperature: "); Serial.print(celsiusToFahrenheit(temp.temperature)); Serial.println(" degrees F");
//	Serial.print("Humidity:    "); Serial.print(humidity.relative_humidity); Serial.println(" % rH");
//	Serial.println();	// Just to space out the measurements for reading at a glance
//
//	delay(1000);
}
