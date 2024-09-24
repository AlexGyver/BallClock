#pragma once
#include <Arduino.h>

// Синхронизатор Unix, getUnix() выдаёт актуальное время на базе установленного
// в update + millis() с момента обновления

#include "core/StampCore.h"

#define STAMP_SYNC_LOOP_PRD (7ul * 24 * 60 * 60 * 1000)

class StampSync : public StampCore {
   public:
    // установить unix и миллисекунды
    StampSync(uint32_t unix = 0, uint16_t ms = 0) {
        update(unix, ms);
    }

    // установить unix и миллисекунды
    void update(uint32_t unix, uint16_t ms = 0) {
        _unix = unix;
        _syncTime = millis() - ms;
    }

    // время синхронизировано
    inline bool synced() {
        return _unix;
    }

    // время синхронизировано
    explicit operator bool() {
        return synced();
    }

    // получить текущий unix
    uint32_t getUnix() {
        if (!synced()) return 0;
        uint32_t diff = millis() - _syncTime;
        if (diff > STAMP_SYNC_LOOP_PRD) {
            _unix += diff / 1000ul;
            _syncTime += diff - diff % 1000ul;
            return _unix;
        }
        return _unix + diff / 1000ul;
    }

    // получить миллисекунды текущей секунды
    uint16_t ms() {
        return synced() ? ((millis() - _syncTime) % 1000ul) : 0;
    }

    // получить миллисекунды с epoch
    uint64_t unixMs() {
        return synced() ? (getUnix() * 1000ull + ms()) : 0;
    }

   private:
    uint32_t _unix = 0;
    uint32_t _syncTime = 0;
};