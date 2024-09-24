#include <Arduino.h>
#include <WiFiConnector.h>

void setup() {
    WiFiConnector.connect("SSID", "PASS");

    WiFiConnector.onConnect([]() {
        Serial.print("Local IP: ");
        Serial.println(WiFi.localIP());
    });
    WiFiConnector.onTimeout([]() {
        Serial.println("WiFi error");
    });
}

void loop() {
    WiFiConnector.tick();
}