/* This is an example program for getting familiar with both the Wifi and the
 * SSD1306_OLED libraries. The ESP32 board will attempt to connect to a Wifi
 * network (with a static IP address) and will print what it is doing and its
 * status messages to an SSD1306 OLED screen.
 */

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <WiFi.h>
#include <Wire.h>

// Wifi globals
#define NETWORK_SSID     "Cisco04646"
#define NETWORK_PASSWORD "your_network_password"
IPAddress local_IP(192, 168, 1, 140);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// OLED screen globals
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_MOSI  18
#define OLED_CLK    5
#define OLED_DC    21
#define OLED_CS    17
#define OLED_RESET  4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
	OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

void setup() {
	// Initialize error handling pin
	pinMode(LED_BUILTIN, OUTPUT);

	// Initialize the display
	if (!display.begin(SSD1306_SWITCHCAPVCC)) fatalHandler();
	display.clearDisplay();
	display.setTextSize(2);
	display.setTextColor(SSD1306_WHITE);
	display.setCursor(0, 0);
}

void loop() {
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

