#pragma once
#include <Arduino.h>
#include <BufferGFX.h>

class StreamDisplay : public BufferGFX {
   public:
    StreamDisplay(uint16_t width, uint16_t height, Stream* stream = nullptr, const char* fill = "[]", const char* clear = "<>") : BufferGFX(width, height), stream(stream), _fill(fill), _clear(clear) {}

    void update() {
        if (stream) {
            for (uint16_t y = 0; y < bufHeight(); y++) {
                for (uint16_t x = 0; x < bufWidth(); x++) {
                    stream->print(bufGet(x, y) ? _fill : _clear);
                }
                stream->println();
            }
        }
    }

    void begin() {}

   private:
    Stream* stream = nullptr;
    const char* _fill;
    const char* _clear;

    using gfx::Buffer::bufGet;
    using gfx::Buffer::bufSet;
};