#pragma once
#include <Arduino.h>

class DaySeconds {
   public:
    DaySeconds() {}
    DaySeconds(uint8_t hour, uint8_t minute, uint8_t second) {
        set(hour, minute, second);
    }

    void set(uint8_t hour, uint8_t minute, uint8_t second) {
        seconds = hour * 3600ul + minute * 60 + second;
    }

    bool operator==(uint32_t u) {
        return seconds == u;
    }
    bool operator!=(uint32_t u) {
        return seconds != u;
    }
    bool operator>(uint32_t u) {
        return seconds > u;
    }
    bool operator>=(uint32_t u) {
        return seconds >= u;
    }
    bool operator<(uint32_t u) {
        return seconds < u;
    }
    bool operator<=(uint32_t u) {
        return seconds <= u;
    }

    uint32_t seconds = 0;
};