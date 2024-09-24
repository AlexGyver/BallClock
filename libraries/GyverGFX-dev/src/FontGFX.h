#pragma once
#include <Arduino.h>

struct gfx_pack_font_t {
    const uint8_t* index_index;
    const uint16_t* index;
    const uint8_t* bitpack;
    uint8_t lineHeight;
    uint8_t indexLength;

    inline uint8_t getLineHeight() const {
        return pgm_read_byte(&lineHeight);
    }
};

struct gfx_map_font_t {
    const uint8_t* bitmap;
    uint8_t from;
    uint8_t to;
    uint8_t width;
    uint8_t lineHeight;

    static gfx_map_font_t fromPGM(const gfx_map_font_t* map) {
        gfx_map_font_t font;
        memcpy_P(&font, map, sizeof(gfx_map_font_t));
        return font;
    }

    inline uint8_t getLineHeight() const {
        return pgm_read_byte(&lineHeight);
    }
    inline uint8_t getWidth() const {
        return pgm_read_byte(&width);
    }
};

namespace gfx {

struct GlyphPack {
    GlyphPack(const gfx_pack_font_t* fontpgm, int16_t idx) {
        gfx_pack_font_t font;
        memcpy_P(&font, fontpgm, sizeof(gfx_pack_font_t));
        if (!font.index_index) return;

#ifdef ESP32
        const void* index_p = memchr(font.index_index, idx, font.indexLength);
#else
        const void* index_p = memchr_P(font.index_index, idx, font.indexLength);
#endif

        if (!index_p) return;
        uint16_t index = pgm_read_word(&font.index[(const uint8_t*)index_p - font.index_index]);
        data = font.bitpack + index;
        offset = pgm_read_byte(data++);
        width = pgm_read_byte(data++);
        height = pgm_read_byte(data++);
    }

    operator bool() const {
        return data;
    }

    const uint8_t* data = nullptr;
    uint8_t offset = 0;
    uint8_t width = 0;
    uint8_t height = 0;
};

}  // namespace gfx