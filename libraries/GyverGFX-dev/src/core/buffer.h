#pragma once
#include <Arduino.h>

namespace gfx {

class Buffer {
   public:
    Buffer(uint16_t width = 0, uint16_t height = 0) : _width(width), _height(height), _hbytes((height + 8 - 1) / 8) {}

    inline uint8_t* buffer() const { return _buffer; }
    inline uint16_t bufWidth() const { return _width; }
    inline uint16_t bufHeight() const { return _height; }
    inline uint16_t hbytes() const { return _hbytes; }

    void bufSet(uint16_t x, uint16_t y, bool fill) {
        fill ? bref(x, y) |= bit(y & 0b111) : bref(x, y) &= ~bit(y & 0b111);
    }

    bool bufGet(uint16_t x, uint16_t y) const {
        return bref(x, y) & bit(y & 0b111);
    }

   protected:
    // 8 пикселей вертикально, MSB вниз, столбиками сверху вниз слева направо
    uint8_t* _buffer = nullptr;
    uint16_t _width = 0;
    uint16_t _height = 0;
    uint16_t _hbytes = 0;

    uint8_t& bref(uint16_t x, uint16_t y) const {
        return _buffer[(y >> 3) + (x * _hbytes)];
    }
};

}  // namespace gfx