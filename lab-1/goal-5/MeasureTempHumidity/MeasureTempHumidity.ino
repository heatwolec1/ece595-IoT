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

#include "Adafruit_SHTC3.h"
#include "Wire.h"

// Globals
Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();
unsigned long prevMillis, curMillis;

float celsiusToFahrenheit(float tempC) {
	return (tempC * 1.8) + 32;
}

void setup() {
	// Initialize timer check value
	prevMillis = 0;

	// Try to open a serial connection
	// don't proceed until it is successful
	Serial.begin(115200);
	while (!Serial)
		delay(10);     // 10 ms is short enough that delay function isn't problematic
	Serial.print("# SHTC3 data logging\n");

	// Try to connect to the SHTC3 sensor
	// don't proceed unless it is successful
	if (! shtc3.begin()) {
		Serial.print("# Couldn't find SHTC3\n");
		while (1) delay(1);	// infinite loop, delay isn't stopping anything else from occurring
	}

	// Print out the CSV logging format
	Serial.print("# CSV format:\n#\ttempC,tempF,humidity");
}

void loop() {
	curMillis = millis();
	if (curMillis-prevMillis >= 1000) {
		prevMillis = curMillis;

		sensors_event_t humidity, temp;

		shtc3.getEvent(&humidity, &temp);	// populate temp and humidity objects with fresh data

		Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.print(" degrees C\n");
		Serial.print("Temperature: "); Serial.print(celsiusToFahrenheit(temp.temperature)); Serial.print(" degrees F\n");
		Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.print(" % rH\n");
		Serial.print("\r");	// used to separate messages in node-red
	}
}

