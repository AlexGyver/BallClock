#pragma once
#include <Arduino.h>

namespace gfx {

class Reader {
   public:
    Reader(const uint8_t* bytes, bool pgm = false) : bytes(bytes), _pgm(pgm) {}

    uint8_t read() {
        _prev = _buf;
        return _buf = _pgm ? pgm_read_byte(bytes++) : *bytes++;
    }

    uint8_t current() {
        return _buf;
    }

    uint8_t prev() {
        return _prev;
    }

    void read(void* dest, size_t size) {
        _pgm ? memcpy_P(dest, bytes, size) : memcpy(dest, bytes, size);
        bytes += size;
    }

    template <typename T>
    void read(T& val) {
        read(&val, sizeof(T));
    }

    const uint8_t* bytes = nullptr;

   private:
    bool _pgm = false;
    uint8_t _buf = 0, _prev = 0;
};

}  // namespace gfx