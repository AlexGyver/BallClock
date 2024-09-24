#include <Arduino.h>
#include <StampSync.h>

StampSync ss;

void setup() {
    Serial.begin(115200);
    ss.update(1695146928);
}

void loop() {
    Serial.println(ss.toString());
    delay(1000);
}