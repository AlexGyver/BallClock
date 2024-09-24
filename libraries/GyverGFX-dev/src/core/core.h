#pragma once
#include <Arduino.h>

#include "FontGFX.h"
#include "decoder.h"
#include "utils.h"

#define FONT_SPACE_SHIFT 2
#define FONT_INTERVAL_SHIFT 4
#define FONT_VSPACE_SHIFT 4

namespace gfx {

class Core {
   public:
    Core() {}
    Core(uint16_t w, uint16_t h) {
        size(w, h);
    }

    // установить размер
    void size(uint16_t w, uint16_t h) {
        _w = w;
        _h = h;
#ifndef GFX_NO_PRINT
        resetTextBound();
#endif
    }

    // получить ширину gfx
    inline uint16_t width() const {
        return _w;
    }

    // получить высоту gfx
    inline uint16_t height() const {
        return _h;
    }

    // установить цвет 565
    void setColor(uint16_t color) {
        _color = color;
    }

    // установить цвет 888
    void setColor24(uint32_t color) {
        _color = rgb888to565(color);
    }

    // получить цвет 565
    inline uint16_t getColor() const {
        return _color;
    }

    // получить цвет 888
    uint32_t getColor24() const {
        return rgb565to888(_color);
    }

    // получить цвет 888
    uint32_t getColor24full() const {
        return rgb565to888full(_color);
    }

    // получить границу вывода 0
    inline int getTextBoundX0() const {
#ifndef GFX_NO_PRINT
        return _bound0;
#else
        return 0;
#endif
    }

    // получить границу вывода 1
    inline int getTextBoundX1() const {
#ifndef GFX_NO_PRINT
        return _bound1;
#else
        return _w - 1;
#endif
    }

    // установить масштаб текста (1-7)
    void setScale(uint8_t scale) {
        setScale(scale, scale);
    }

    // установить масштаб текста (1-7)
    void setScale(uint8_t scaleX, uint8_t scaleY) {
#ifndef GFX_NO_SCALE
        _scaleX = constrain(scaleX, 1, 7);
        _scaleY = constrain(scaleY, 1, 7);
#endif
    }

    // получить масштаб текста X (1-7)
    inline uint8_t getScaleX() const {
#ifndef GFX_NO_SCALE
        return _scaleX;
#else
        return 1;
#endif
    }

    // получить масштаб текста Y (1-7)
    inline uint8_t getScaleY() const {
#ifndef GFX_NO_SCALE
        return _scaleY;
#else
        return 1;
#endif
    }

    // установить курсор в 0, 0
    void home() {
        setCursor(0, 0);
    }

    // установить курсор xy
    void setCursor(int x, int y) {
        _cx = x;
        _cy = y;
    }

    // установить курсор x
    void setCursorX(int x) {
        _cx = x;
    }

    // установить курсор y
    void setCursorY(int y) {
        _cy = y;
    }

    // получить курсор x
    inline int getCursorX() const {
        return _cx;
    }

    // получить курсор y
    inline int getCursorY() const {
        return _cy;
    }

    // включить сглаживание стандартного шрифта и байтов (умолч. true)
    void setSmooth(bool smooth) {
#ifndef GFX_NO_SMOOTH
        _smooth = smooth;
#endif
    }

    // получить сглаживание стандартного шрифта и байтов
    inline bool getSmooth() const {
#ifndef GFX_NO_SMOOTH
        return _smooth;
#else
        return 0;
#endif
    }

#ifndef GFX_NO_PRINT
   public:
    // =================== FONT CONFIG ===================
    // подключить шрифт
    void setFont(const gfx_pack_font_t &font) {
#ifndef GFX_NO_PACK_FONT
        _pack_font = &font;
        _map_font = nullptr;
#endif
    }

    // подключить шрифт
    void setFont(const gfx_map_font_t &font) {
        _map_font = &font;
#ifndef GFX_NO_PACK_FONT
        _pack_font = nullptr;
#endif
    }

    // получить высоту строк текущего шрифта с учётом масштаба
    uint8_t getLineHeight() const {
#ifndef GFX_NO_PACK_FONT
        if (_pack_font) {
            return _pack_font->getLineHeight() * getScaleY();
        } else
#endif
            if (_map_font) {
            uint8_t h = _map_font->getLineHeight();
            return (h >= 8 ? h : h + 1) * getScaleY();
            // return ((((1 << FONT_VSPACE_SHIFT) - 1 + h) >> FONT_VSPACE_SHIFT) + h) * getScaleY();
        }
        return 0;
    }

    // получить размер пробела между буквами с учётом масштаба
    uint16_t getIntervalSize() const {
        uint8_t v = getLineHeight() >> FONT_INTERVAL_SHIFT;
        return v ? v : 1;
    }

    // получить размер пробела между словами с учётом масштаба
    uint16_t getSpaceSize() const {
#ifndef GFX_NO_PACK_FONT
        if (_pack_font) return (_pack_font->getLineHeight() >> FONT_SPACE_SHIFT) * getScaleX();
        else
#endif
            if (_map_font)
            return _map_font->getWidth() * getScaleX();
        return 0;
    }

    // перевести строку
    void newLine() {
        _cx = _bound0;
        _cy += getLineHeight();
    }

    // установить автоматический перенос текста (умолч. false)
    void setTextWrap(bool mode) {
        _wrap = mode;
    }

    // получить автоматический перенос текста
    inline bool getTextWrap() const {
        return _wrap;
    }

    // установить окно вывода текста по х
    void setTextBound(int x0, int x1) {
        if (_checkBounds(x0, x1, _w - 1)) {
            _bound0 = x0;
            _bound1 = x1;
        }
    }

    // сбросить границы вывода текста до (0, ширина)
    void resetTextBound() {
        _bound0 = 0;
        _bound1 = _w - 1;
    }

    // поулчить ширину текста в пикселях при текущих настройках шрифта
    uint16_t textWidth(const String &s) const {
        return textWidth(s.c_str(), false);
    }
    uint16_t textWidth(const char *s) const {
        return textWidth(s, false);
    }
    uint16_t textWidth_P(PGM_P s) const {
        return textWidth(s, true);
    }
    uint16_t textWidth(const __FlashStringHelper *s) const {
        return textWidth((const char *)s, true);
    }
    uint16_t textWidth(const char *str, bool pgm) const {
        Decoder decoder;
        uint16_t w = 0;
        uint8_t interval = getIntervalSize();
        uint8_t space = getSpaceSize();

        while (1) {
            char data = pgm ? pgm_read_byte(str++) : *(str++);
            if (!data) break;

            if (data == ' ') {
                w += space;
                continue;
            }
            if (data == '\n' || data == '\r') {
                continue;
            }

            int idx = decoder.getIndex(data);
            if (idx < 0) continue;

#ifndef GFX_NO_PACK_FONT
            if (_pack_font) {
                gfx::GlyphPack g(_pack_font, idx);
                if (!g) continue;
                w += g.width * getScaleX();
            } else
#endif
                if (_map_font) {
                w += _map_font->getWidth() * getScaleX();
            } else {
                continue;
            }
            w += interval;
        }
        return w;
    }

   protected:  // font
#ifndef GFX_NO_PACK_FONT
    const gfx_pack_font_t *_pack_font = nullptr;
#endif

#ifdef GFX_NO_DEFAULT_FONT
    const gfx_map_font_t *_map_font = nullptr;
#else
    const gfx_map_font_t *_map_font = &gfx_font_5x8;
#endif

    int16_t _bound0 = 0, _bound1 = 0;
    bool _wrap = false;
#endif  // GFX_NO_PRINT

   protected:  // base
    int16_t _w = 0, _h = 0;
    int16_t _cx = 0, _cy = 0;
    uint16_t _color = 1;
#ifndef GFX_NO_SCALE
    uint8_t _scaleX = 1, _scaleY = 1;
#endif
#ifndef GFX_NO_SMOOTH
    bool _smooth = true;
#endif

    // поменять
    void _swap(int &a, int &b) {
        int c = a;
        a = b;
        b = c;
    }

    // поменять и ограничить пределом
    bool _checkBounds(int &v0, int &v1, int max) {
        if (v0 > v1) _swap(v0, v1);
        if (v1 < 0 || v0 > max) return 0;
        if (v0 < 0) v0 = 0;
        if (v1 > max) v1 = max;
        return 1;
    }

    inline bool _inBounds(int x) {
        return x >= getTextBoundX0() && x <= getTextBoundX1();
    }

    int16_t _g_min(int16_t a, int16_t b) {
        return a < b ? a : b;
    }
    int16_t _g_max(int16_t a, int16_t b) {
        return a > b ? a : b;
    }
};

}  // namespace gfx