#pragma once
#include <Arduino.h>

namespace gfx {

// определить длину строки с любыми символами (в т.ч. русскими)
uint16_t strlen_unicode(const char* str, bool pgm = 0);

// определить длину PGM строки с любыми символами (в т.ч. русскими)
uint16_t strlen_unicode_P(PGM_P str);

enum class ease_t : uint8_t {
    Linear,
    In,
    Out,
    InOut,
};

float ease(float t, ease_t type = ease_t::InOut);

uint32_t rgb565to888(uint16_t col);
uint32_t rgb565to888full(uint16_t col);
uint16_t rgb888to565(uint32_t col);

}  // namespace gfx