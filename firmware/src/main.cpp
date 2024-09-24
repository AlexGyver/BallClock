#include <Arduino.h>
#include <GyverNTP.h>
#include <Looper.h>
#include <RunningGFX.h>
#include <WiFiConnector.h>

#include "config.h"
#include "matrix.h"
#include "settings.h"

void runString(String str) {
    RunningGFX run(matrix);
    matrix.clear();
    matrix.setModeDiag();
    run.setSpeed(10);
    run.setWindow(0, matrix.width(), 1);
    run.setColor24(0x00ff00);
    run.setFont(gfx_font_3x5);
    run.setText(str);
    run.start();

    while (run.tick() != RG_FINISH) {
        delay(0);
        yield();
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("\n" PROJECT_NAME " v" PROJECT_VER);

    matrix.begin();

    WiFiConnector.setName(PROJECT_NAME);

    WiFiConnector.onConnect([]() {
        NTP.begin();

        Serial.print("Connected: ");
        Serial.println(WiFi.localIP());
        if (db[kk::show_ip]) runString(WiFi.localIP().toString());
        
        ota.checkUpdate();
    });

    WiFiConnector.onError([]() {
        String str("Error! AP: ");
        str += WiFi.softAPSSID();
        Serial.println(str);
        runString(str);
    });
}

void loop() {
    Looper.loop();
}