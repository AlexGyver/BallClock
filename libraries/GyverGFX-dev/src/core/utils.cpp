#include "utils.h"

namespace gfx {

// определить длину строки с любыми символами (в т.ч. русскими)
uint16_t strlen_unicode(const char* str, bool pgm) {
    uint16_t count = 0;
    while (true) {
        char c = pgm ? pgm_read_byte(str) : *str;
        if (!c) break;
        if ((c & 0xc0) != 0x80) count++;
        str++;
    }
    return count;
}

// определить длину PGM строки с любыми символами (в т.ч. русскими)
uint16_t strlen_unicode_P(PGM_P str) {
    return strlen_unicode(str, true);
}

float ease(float t, ease_t type) {
    // https://stackoverflow.com/a/25730573
    switch (type) {
        case ease_t::Linear: return t;
        case ease_t::In: return t * t;
        case ease_t::Out: return sqrt(t);
        case ease_t::InOut: {
            float t2 = t * t;
            return t2 / ((t2 - t) * 2.0f + 1.0f);
        }
    }
    return 0;
}

union _RGB_conv {
    uint8_t bytes[4];
    uint32_t hex;
};

uint32_t rgb565to888(uint16_t col) {
    // rrrrrggg gggbbbbb
    uint8_t r = (col >> 8) & 0b11111000;
    uint8_t g = (col >> 3) & 0b11111100;
    uint8_t b = (col << 3);

    return _RGB_conv{b, g, r, 0xff}.hex;
}

uint32_t rgb565to888full(uint16_t col) {
    // https://web.archive.org/web/20140604073359/https://developer.apple.com/library/Mac/documentation/Performance/Reference/vImage_conversion/Reference/reference.html#//apple_ref/c/func/vImageConvert_RGB565toARGB8888
    uint8_t r = ((col >> 11) * 255 + 15) / 31;
    uint8_t g = (((col >> 5) & 0b00111111) * 255 + 31) / 63;
    uint8_t b = ((col & 0b00011111) * 255 + 15) / 31;

    return _RGB_conv{b, g, r, 0xff}.hex;
}

uint16_t rgb888to565(uint32_t col) {
    //          rrrrrggg gggbbbbb
    // rrrrrrrr gggggggg bbbbbbbb
    return ((col >> 8) & 0b1111100000000000) | ((col >> 5) & 0b0000011111100000) | ((col >> 3) & 0b0000000000011111);
}

}  // namespace gfx