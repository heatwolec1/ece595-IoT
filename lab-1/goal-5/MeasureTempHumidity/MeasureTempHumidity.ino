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
Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();	// sensor object
unsigned long prevMillis, curMillis;		// timer values for creating delays
sensors_event_t humidity, temp;				// sensor value objects
String csvOutput = "";						// final data output

float celsiusToFahrenheit(float tempC) {
	return (tempC * 1.8) + 32;
}

void setup() {
	// Initialize timer check previous value
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

		// Output the sensor readings in the CSV format
		csvOutput = String(temp.temperature, 2) + ","
			+ String(celsiusToFahrenheit(temp.temperature), 2) + ","
			+ String(humidity.relative_humidity, 2) + "\n";
		Serial.print(csvOutput);
	}
}

