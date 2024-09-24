#pragma once
#include <Arduino.h>

#include "reader.h"

// aaaaaabb
// bbbbcccc
// ccdddddd

namespace gfx {

class Unpacker {
   public:
    struct Chunk {
        bool value;
        uint8_t size;
    };

    Unpacker(Reader& reader, uint16_t width, uint16_t height) : width(width), height(height), _reader(reader) {}

    Chunk readChunk() {
        uint8_t chunk = 0;

        switch ((_shift++) & 0b11) {
            case 0:
                chunk = (_reader.read() >> 2);
                break;
            case 1:
                chunk = (_reader.current() << 4) & 0b00110000;
                chunk |= (_reader.read() >> 4);
                break;
            case 2:
                chunk = (_reader.current() << 2) & 0b00111100;
                chunk |= (_reader.read() >> 6);
                break;
            case 3:
                chunk = _reader.current() & 0b00111111;
                break;
        }

        return Chunk{bool(chunk & 1), uint8_t(chunk >> 1)};
    }

    uint16_t width = 0;
    uint16_t height = 0;

   private:
    uint8_t _shift = 0;
    Reader& _reader;
};

}  // namespace gfx
