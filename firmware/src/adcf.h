#pragma once
#include <Arduino.h>
#include <Looper.h>

class ADCFilt : public LoopTimerBase {
   public:
    ADCFilt(uint8_t pin, uint8_t k) : LoopTimerBase(60), pin(pin), k(k) {}

    void exec() {
        if (++count == 8) {
            count = 0;
            raw = analogRead(pin);
        }

        filt += (raw - filt) / k;

        // int16_t sum = (raw - filt) + err;
        // filt += sum / k;
        // err = sum % k;
    }

    uint16_t getRaw() {
        return raw;
    }

    uint16_t getFilt() {
        return filt;
    }

   private:
    uint8_t pin, k;
    uint16_t filt, raw, count;
    // uint16_t err;
};