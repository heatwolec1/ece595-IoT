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

// Wifi constants
#define NETWORK_SSID     "Cisco04646"
#define NETWORK_PASSWORD "your_network_password"
IPAddress local_IP(192, 168, 1, 140);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

void setup() {
}

void loop() {
}

