#include <Arduino.h>
#include <StampTicker.h>

StampTicker st;

void setup() {
    Serial.begin(115200);
    st.update(1695146928);
}

void loop() {
    // секундный таймер
    if (st.tick()) {
        Serial.println(st.toString());

        Datime dt(st);
        dt.second;
        dt.minute;
        // ...
    }
}