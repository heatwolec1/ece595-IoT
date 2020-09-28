/*
 * This code is nearly identical to the example code provided by Adafruit for the
 * SHTC3 sensor. My modications include printing the temperature in Fahrenheit,
 * reformatting the text being printed to serial, and replacing the delay function
 * calls with system time differences.
 *
 * Rather than displaying the data on the Arduino serial monitor, I will be using
 * Node-Red to listen for and display the data.
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
	prevMillis = 0;

	Serial.begin(115200);
	while (!Serial)
		delay(10);     // 10 ms is short enough that delay function isn't problematic

	Serial.println("SHTC3 test");
	if (! shtc3.begin()) {
		Serial.println("Couldn't find SHTC3");
		while (1) delay(1);	// infinite loop, delay isn't stopping anything else from occurring
	}
	Serial.println("Found SHTC3 sensor");
}

void loop() {
	curMillis = millis();
	if (curMillis-prevMillis >= 1000) {
		prevMillis = curMillis;

		sensors_event_t humidity, temp;

		shtc3.getEvent(&humidity, &temp);	// populate temp and humidity objects with fresh data

		Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
		Serial.print("Temperature: "); Serial.print(celsiusToFahrenheit(temp.temperature)); Serial.println(" degrees F");
		Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println(" % rH");
		Serial.print("\r");	// used to separate messages in node-red
	}
}
