#pragma once
#include <Arduino.h>

#ifndef GFX_NO_PRINT
#include <Print.h>

#include "core/decoder.h"
#include "default/font3x5.h"
#include "default/font4x6.h"
#include "default/font5x8.h"
#endif

#include "core/buffer.h"
#include "core/core.h"
#include "core/reader.h"
#include "core/sizes.h"
#include "core/utils.h"
#include "default/icons8x8.h"

#ifndef GFX_NO_PACK
#include "core/unpacker.h"
#endif

#define GFX_FILL 0
#define GFX_STROKE 1
#define GFX_FULL 2

#ifndef GFX_NO_PRINT
class GyverGFX : public gfx::Core, public Print {
#else
class GyverGFX : public gfx::Core {
#endif
   public:
    GyverGFX() {}
    GyverGFX(uint16_t w, uint16_t h) : gfx::Core(w, h) {}

#pragma region INTERFACE
    // =================== INTERFACE ===================

   protected:
    // установить пиксель цветом getColor(), всегда вызывается внутри дисплея
    virtual void fastSet(int x, int y) = 0;

    // получить пиксель, всегда вызывается внутри дисплея (нужно только для fill)
    virtual uint16_t fastGet(int x, int y) { return 0; }

    // четырёхугольник заливка цветом getColor(), всегда вызывается внутри дисплея
    virtual void fastRect(int x0, int y0, int x1, int y1) {
        uint16_t h = y1 - y0 + 1;
        uint8_t shift = y0 & 0b111;
        y0 -= shift;

        beginSend(x0, y0 >> 3, x1, y1 >> 3, true);

        while (h) {
            uint8_t b = 0xff;
            if (shift) {
                uint8_t curh = (h + shift > 8) ? (8 - shift) : h;
                b = _fillByte(shift, curh);
                h -= curh;
                shift = 0;
            } else {
                if (h < 8) {
                    b = _fillByte(0, h);
                    h = 0;
                } else {
                    h -= 8;
                }
            }
            for (int x = x0; x <= x1; x++) fastByte(x, y0, b);
            y0 += 8;
        }

        endSend();
    }

    // вывести столбик 8 бит (MSB снизу) цветом getColor(). Всегда внутри дисплея, (y) кратен 8
    virtual void fastByte(int x, int y, uint8_t data) {
        for (uint8_t i = 0; i < 8; i++) {
            if (y >= _h) break;
            if (data & 1) fastSet(x, y);
            data >>= 1;
            y++;
        }
    }

    // заливка всего экрана
    virtual void fastFill(uint16_t color) {
        uint16_t t = _color;
        _color = color;
        fastRect(0, 0, _w - 1, _h - 1);
        _color = t;
    }

    // вызывается перед отрисовкой символов, прямоугольников и битмапов, y это номер байта. Всегда внутри дисплея
    virtual void beginSend(int x0, int y0, int x1, int y1, bool horizontal) {}

    // окончание отправки битмапа
    virtual void endSend() {}

    // горизонтальная линия цветом getColor(), всегда слева направо и внутри дисплея
    // virtual void fastLineH(int y, int x0, int x1) {
    // fastRect(x0, y, x1, y);
    // for (int x = x0; x <= x1; x++) fastSet(x, y);
    // }

    // вертикальная линия цветом getColor(), всегда сверху вниз и внутри дисплея
    // virtual void fastLineV(int x, int y0, int y1) {
    // fastRect(x, y0, x, y1);
    // for (int y = y0; y <= y1; y++) fastSet(x, y);
    // }

   public:
    // обновить
    virtual void update() {}

#pragma region PIXEL
    // =================== PIXEL ===================
   public:
    // установить пиксель цветом setColor() (безопасно)
    void set(int x, int y) {
        if (x >= 0 && x < _w && y >= 0 && y < _h) fastSet(x, y);
    }

    // прочитать пиксель (безопасно)
    uint16_t get(int x, int y) {
        return (x >= 0 && x < _w && y >= 0 && y < _h) ? fastGet(x, y) : 0;
    }

    // залить экран
    void fill(uint16_t color) {
        fastFill(color);
    }

    // очистить экран
    void clear() {
        fastFill(0);
    }

#pragma region LINE
    // =================== LINE ===================

    // горизонтальная линия
    void lineH(int y, int x0, int x1) {
        if (x0 == x1) {
            set(x0, y);
        } else if (y >= 0 && y < _h && _checkBounds(x0, x1, _w - 1)) {
            fastRect(x0, y, x1, y);  // fastLineH(y, x0, x1);
        }
    }

    // горизонтальная толстая линия
    void lineH(int y, int x0, int x1, uint8_t stroke) {
        if (!stroke) return;
        if (stroke == 1) lineH(y, x0, x1);
        else rect(x0, y - stroke / 2, x1, y + stroke / 2, GFX_FILL);
    }

    // вертикальная линия
    void lineV(int x, int y0, int y1) {
        if (y0 == y1) {
            set(x, y0);
        } else if (x >= 0 && x < _w && _checkBounds(y0, y1, _h - 1)) {
            fastRect(x, y0, x, y1);  // fastLineV(x, y0, y1);
        }
    }

    // вертикальная толстая линия
    void lineV(int x, int y0, int y1, uint8_t stroke) {
        if (!stroke) return;
        if (stroke == 1) lineV(x, y0, y1);
        else rect(x - stroke / 2, y0, x + stroke / 2, y1, GFX_FILL);
    }

    // линия
    void line(int x0, int y0, int x1, int y1) {
        if (x0 == x1) {
            lineV(x0, y0, y1);
        } else if (y0 == y1) {
            lineH(y0, x0, x1);
        } else {
            _line(x0, y0, x1, y1);
        }
    }

    // толстая линия
    void line(int x0, int y0, int x1, int y1, uint8_t stroke, bool round = true) {
        if (!stroke) return;
        if (x0 == x1) {
            lineV(x0, y0, y1, stroke);
        } else if (y0 == y1) {
            lineH(y0, x0, x1, stroke);
        } else {
            if (stroke == 1) {
                _line(x0, y0, x1, y1);
            } else {
                _lineThick(x0, y0, x1, y1, stroke);
            }
        }
        if (round) {
            circle(x0, y0, stroke / 2);
            circle(x1, y1, stroke / 2);
        }
    }

    // рисовать линию из текущего setCursor и переместить курсор туда на x1 y1
    void lineTo(int x1, int y1) {
        line(_cx, _cy, x1, y1);
        _cx = x1;
        _cy = y1;
    }

    // рисовать толстую линию из текущего курсора и переместить курсор туда
    void lineTo(int x1, int y1, uint8_t stroke) {
        line(_cx, _cy, x1, y1, stroke);
        _cx = x1;
        _cy = y1;
    }

#pragma region RECT
    // ================== RECT ==================

    // прямоугольник. Левый верхний, правый нижний угол включая
    void rect(int x0, int y0, int x1, int y1, uint8_t mode = GFX_FILL) {
        switch (mode) {
            case GFX_FILL:
            case GFX_FULL:
                if (!_checkBounds(y0, y1, _h - 1)) return;
                if (!_checkBounds(x0, x1, _w - 1)) return;
                fastRect(x0, y0, x1, y1);
                break;

            case GFX_STROKE:
                if (x0 > x1) _swap(x0, x1);
                if (y0 > y1) _swap(y0, y1);
                lineV(x0, y0, y1);
                if (x0 != x1) lineV(x1, y0, y1);
                if (x1 - x0 >= 2) {
                    lineH(y0, x0 + 1, x1 - 1);
                    lineH(y1, x0 + 1, x1 - 1);
                }
                break;
        }
    }

    // прямоугольник с толстой обводкой. Левый верхний, правый нижний угол включая
    void rect(int x0, int y0, int x1, int y1, uint8_t mode, uint8_t stroke) {
        switch (mode) {
            case GFX_FILL:
                rect(x0, y0, x1, y1, GFX_FILL);
                break;

            case GFX_STROKE:
                if (stroke) {
                    uint8_t s2 = stroke / 2;
                    uint8_t left = stroke - s2 - 1;
                    rect(x0 - left, y0 - left, x0 + s2, y1 + left, GFX_FILL);
                    rect(x1 - s2, y0 - left, x1 + left, y1 + left, GFX_FILL);
                    rect(x0 + s2, y0 - left, x1 - s2, y0 + s2, GFX_FILL);
                    rect(x0 + s2, y1 - s2, x1 - s2, y1 + left, GFX_FILL);
                }
                break;

            case GFX_FULL:
                if (stroke > 1) {
                    uint8_t left = stroke - (stroke / 2) - 1;
                    rect(x0 - left, y0 - left, x1 + left, y1 + left, GFX_FILL);
                } else {
                    rect(x0, y0, x1, y1, GFX_FILL);
                }
                break;
        }
    }

    // прямоугольник ширина/высота
    void rectWH(int x, int y, int w, int h, uint8_t mode = GFX_FILL) {
        if (w > 0 && h > 0) rect(x, y, x + w - 1, y + h - 1, mode);
    }

    // прямоугольник ширина/высота с толстой обводкой
    void rectWH(int x, int y, int w, int h, uint8_t mode, uint8_t stroke) {
        switch (mode) {
            case GFX_FILL:
                rectWH(x, y, w, h, GFX_FILL);
                break;

            case GFX_STROKE:
            case GFX_FULL:
                if (w > 0 && h > 0) rect(x, y, x + w - 1, y + h - 1, mode, stroke);
                break;
        }
    }

    // прямоугольник из центра ширина/высота
    void rectCenter(int x, int y, int w, int h, uint8_t mode = GFX_FILL) {
        rectWH(x - w / 2, y - h / 2, w, h, mode);
    }

    // прямоугольник из центра ширина/высота с толстой обводкой
    void rectCenter(int x, int y, int w, int h, uint8_t mode, uint8_t stroke) {
        rectWH(x - w / 2, y - h / 2, w, h, mode, stroke);
    }

    // скруглённый прямоугольник
    void roundRect(int x0, int y0, int x1, int y1, int r, uint8_t mode = GFX_FILL) {
        if (x0 > x1) _swap(x0, x1);
        if (y0 > y1) _swap(y0, y1);
        if (x1 - x0 + 1 < r * 2) r = (x1 - x0 + 1) / 2;
        if (y1 - y0 + 1 < r * 2) r = (y1 - y0 + 1) / 2;

        switch (mode) {
            case GFX_FILL:
            case GFX_FULL:
                _corners(x0 + r, y0 + r, x1 - x0 - r * 2, y1 - y0 - r * 2, r, GFX_FILL);
                rect(x0, y0 + r + 1, x0 + r, y1 - r - 1, GFX_FILL);
                rect(x1, y0 + r + 1, x1 - r, y1 - r - 1, GFX_FILL);
                rect(x0 + r + 1, y0, x1 - r - 1, y1, GFX_FILL);
                break;

            case GFX_STROKE:
                _corners(x0 + r, y0 + r, x1 - x0 - r * 2, y1 - y0 - r * 2, r, GFX_STROKE);
                lineH(y0, x0 + r + 1, x1 - r - 1);
                lineH(y1, x0 + r + 1, x1 - r - 1);
                lineV(x0, y0 + r + 1, y1 - r - 1);
                lineV(x1, y0 + r + 1, y1 - r - 1);
                break;
        }
    }

    // скруглённый прямоугольник с толстой обводкой
    void roundRect(int x0, int y0, int x1, int y1, int r, uint8_t mode, uint8_t stroke) {
        switch (mode) {
            case GFX_FILL:
                roundRect(x0, y0, x1, y1, r, GFX_FILL);
                break;

            case GFX_STROKE:
                if (stroke) {
                    for (uint8_t i = 0; i < stroke; i++) {
                        _corners(x0 + r, y0 + r, x1 - x0 - r * 2, y1 - y0 - r * 2, r - stroke / 2 + i, GFX_STROKE);
                    }
                    int s2 = stroke / 2;
                    int left = stroke - s2;
                    rect(x0 + r + 1, y0 - left + 1, x1 - r - 1, y0 + s2, GFX_FILL);
                    rect(x0 + r + 1, y1 - s2, x1 - r - 1, y1 + left - 1, GFX_FILL);
                    rect(x0 - left + 1, y0 + r + 1, x0 + s2, y1 - r - 1, GFX_FILL);
                    rect(x1 - s2, y0 + r + 1, x1 + left - 1, y1 - r - 1, GFX_FILL);
                }
                break;

            case GFX_FULL: {
                if (stroke > 1) {
                    uint8_t left = stroke - (stroke / 2) - 1;
                    roundRect(x0 - left, y0 - left, x1 + left, y1 + left, r, GFX_FILL);
                } else {
                    roundRect(x0, y0, x1, y1, r, GFX_FILL);
                }
            } break;
        }
    }

    // скруглённый прямоугольник ширина/высота
    void roundRectWH(int x, int y, int w, int h, int r, uint8_t mode = GFX_FILL) {
        if (w > 0 && h > 0) roundRect(x, y, x + w - 1, y + h - 1, r, mode);
    }

    // скруглённый прямоугольник ширина/высота с толстой обводкой
    void roundRectWH(int x, int y, int w, int h, int r, uint8_t mode, uint8_t stroke) {
        switch (mode) {
            case GFX_FILL:
                roundRectWH(x, y, w, h, r, GFX_FILL);
                break;

            case GFX_STROKE:
            case GFX_FULL:
                if (w > 0 && h > 0) roundRect(x, y, x + w - 1, y + h - 1, r, mode, stroke);
                break;
        }
    }

    // скруглённый прямоугольник из центра
    void roundRectCenter(int x, int y, int w, int h, int r, uint8_t mode = GFX_FILL) {
        roundRectWH(x - w / 2, y - h / 2, w, h, r, mode);
    }

    // скруглённый прямоугольник из центра с толстой обводкой
    void roundRectCenter(int x, int y, int w, int h, int r, uint8_t mode, uint8_t stroke) {
        roundRectWH(x - w / 2, y - h / 2, w, h, r, mode, stroke);
    }

#pragma region CIRCLE
    // ================== CIRCLE ==================
    // окружность
    void circle(int x0, int y0, int r, uint8_t mode = GFX_FILL) {
        // Barrera 4, http://willperone.net/Code/codecircle.php
        if (r < 0) return;

        int x = 0, px = -1, py = r, y = r, d = -(r >> 1);
        switch (r) {
            case 0:
                set(x0, y0);
                return;
            case 1:
            case 2:
                d = 0;
                break;
        }
        while (x <= y) {
            if (mode == GFX_STROKE) {
                set(x0 - y, y0 + x);
                set(x0 - y, y0 - x);
                set(x0 - x, y0 + y);
                set(x0 - x, y0 - y);
                set(x0 + x, y0 + y);
                set(x0 + x, y0 - y);
                set(x0 + y, y0 + x);
                set(x0 + y, y0 - x);
            } else {
                if (py != y) {
                    lineV(x0 - py, y0 - x, y0 + x);
                    lineV(x0 + py, y0 - x, y0 + x);
                }
                if (px != x) {
                    lineV(x0 - x, y0 - y, y0 + y);
                    if (x) lineV(x0 + x, y0 - y, y0 + y);
                }
                px = x;
                py = y;
            }
            d += (d <= 0) ? (++x) : -(--y);
        }
    }

    // окружность с толстой обводкой
    void circle(int x0, int y0, int r, uint8_t mode, uint8_t stroke) {
        switch (mode) {
            case GFX_FILL:
                circle(x0, y0, r, GFX_FILL);
                break;

            case GFX_STROKE:
                r -= stroke / 2;
                while (stroke--) {
                    circle(x0, y0, r, GFX_STROKE);
                    r++;
                }
                break;

            case GFX_FULL:
                if (stroke) circle(x0, y0, r - stroke / 2 + stroke - 1, GFX_FILL);
                else circle(x0, y0, r, GFX_FILL);
                break;
        }
    }

#pragma region ARC
    // ================== ARC ==================
    // void arc(int x0, int y0, int r, int start, int end, uint8_t fill = GFX_FILL) {
    // if (start == end) return;
    // if (abs(start - end) >= 360) return circle(x0, y0, r, fill);
    // while (start >= 360 || end >= 360) start -= 360, end -= 360;
    // while (start <= -360 || end <= -360) start += 360, end += 360;
    // if (start > end) _swap(start, end);

    // todo

    // uint8_t mask = 0;
    // int ss = start / 45;
    // int es = end / 45;
    // bool sf = (ss * 45) == start;
    // bool ef = (es * 45) == end;

    // if (ss == es) {
    // } else if (es - ss == 7) {
    // } else {
    //     for (int i = 0; i < es - ss; i++) {
    //         bitSet(mask, (ss + i + 8) & 0b111);
    //     }
    //     _circleMask(x0, y0, r, fill, mask);
    // }
    // }

#pragma region BEZIER
    // ================== BEZIER ==================

    // кривая Безье. Массив в формате [x0, y0... xn, yn], size - количество точек (размер массива / 2), количество точек отрисовки будет равно 2^dense
    template <typename T>
    void bezier(const T *arr, uint8_t size, uint8_t dense) {
        if (!arr || !size) return;
        int a[size * 2];
        int prev[2] = {arr[0], arr[1]};
        set(arr[0], arr[1]);

        for (int i = 0; i < (1 << dense) + 1; i++) {
            for (int j = 0; j < size * 2; j++) a[j] = arr[j];
            for (int j = (size - 1) * 2 - 1; j > 0; j -= 2)
                for (int k = 0; k <= j; k++)
                    a[k] += (((long)(a[k + 2] - a[k]) * i) >> dense);

            if (prev[0] != a[0] || prev[1] != a[1]) {
                _line(prev[0], prev[1], a[0], a[1], false);
                prev[0] = a[0];
                prev[1] = a[1];
            }
        }
    }

    // толстая кривая Безье. Массив в формате [x0, y0... xn, yn], size - количество точек (размер массива / 2), количество точек отрисовки будет равно 2^dense
    template <typename T>
    void bezier(const T *arr, uint8_t size, uint8_t dense, uint8_t stroke) {
        if (!arr || !size) return;
        int a[size * 2];
        int prev[2] = {arr[0], arr[1]};
        // set(arr[0], arr[1]);

        for (int i = 0; i < (1 << dense) + 1; i++) {
            for (int j = 0; j < size * 2; j++) a[j] = arr[j];
            for (int j = (size - 1) * 2 - 1; j > 0; j -= 2)
                for (int k = 0; k <= j; k++)
                    a[k] += (((long)(a[k + 2] - a[k]) * i) >> dense);

            if (prev[0] != a[0] || prev[1] != a[1]) {
                line(prev[0], prev[1], a[0], a[1], stroke, false);
                prev[0] = a[0];
                prev[1] = a[1];
            }
        }
    }

#pragma region FLOOD
    // ================== FLOOD ==================
    // заливка установленным цветом, 8 бит координаты
    void flood8(uint8_t x, uint8_t y) {
        if (x < (uint8_t)_w && y < (uint8_t)_h && fastGet(x, y) != getColor()) {
            _flood<uint8_t>(x, y);
        }
    }

    // заливка установленным цветом, 16 бит координаты
    void flood16(uint16_t x, uint16_t y) {
        if (x < (uint16_t)_w && y < (uint16_t)_h && fastGet(x, y) != getColor()) {
            _flood<uint16_t>(x, y);
        }
    }

#pragma region IMAGE
    // ================== IMAGE ==================

    // вывести битмап формата gfximage_t (сжатый или обычный битмап)
    void drawImage(int x, int y, gfximage_t *img, bool pgm = true) {
        gfx::Reader reader(img, pgm);
#ifndef GFX_NO_PACK
        reader.read() ? _drawBitpack(x, y, reader) : _drawBitmap(x, y, reader);
#else
        if (!reader.read()) _drawBitmap(x, y, reader);
#endif
    }

    // вывести битмап формата gfxpack_t (сжатый битмап)
    void drawBitpack(int x, int y, gfxpack_t *bitpack, bool pgm = true) {
#ifndef GFX_NO_PACK
        gfx::Reader reader(bitpack, pgm);
        _drawBitpack(x, y, reader);
#endif
    }

    // вывести битмап формата gfxmap_t (столбики 8 пикселей (MSB снизу), столбцами сверху вниз слева направо)
    void drawBitmap(int x, int y, gfxmap_t *bitmap, bool pgm = true) {
        gfx::Reader reader(bitmap, pgm);
        _drawBitmap(x, y, reader);
    }

#pragma region BUFFER
    // ================== BUFFER ==================
    void drawBuffer(int x, int y, gfx::Buffer &buffer) {
        gfx::Reader reader(buffer.buffer(), false);
        _bitmap(x, y, buffer.bufWidth(), buffer.bufHeight(), reader);
    }

#pragma region WRITE
    // ================== WRITE ===================
#ifndef GFX_NO_PRINT
    size_t write(uint8_t data) {
        if (data == '\r') {
            return 1;
        }
        if (data == '\n') {
            newLine();
            return 1;
        }
        if (_wrap && data == ' ' && _cx == getTextBoundX0()) {
            return 1;
        }
        if (_cx >= getTextBoundX1() || _cy >= _h) {
            return 1;
        }
        if (data == ' ') {
            _cx += getSpaceSize();
            return 1;
        }

        int16_t idx = decoder.getIndex(data);
        if (idx < 0) return 1;

#ifndef GFX_NO_PACK_FONT
        if (_pack_font) {
#ifndef GFX_NO_PACK
            gfx::GlyphPack g(_pack_font, idx);
            if (!g || !_checkTextBounds(g.width)) return 1;
            gfx::Reader reader(g.data, true);
            _drawBitpack(_cx, _cy + g.offset * getScaleY(), gfx::Unpacker(reader, g.width, g.height), true);
            _cx += g.width * getScaleX();
#endif
        } else
#endif
            if (_map_font) {
            gfx_map_font_t g = gfx_map_font_t::fromPGM(_map_font);
            if (!_checkTextBounds(g.width) || idx < g.from || idx > g.to) return 1;

            int16_t bx = _cx;
            int16_t by = _cy;
            uint8_t prev = 0;
            uint8_t chunks = (g.lineHeight + 7) >> 3;
            g.bitmap += (idx - g.from) * g.width * chunks;
            for (uint8_t y = 0; y < chunks; y++) {
                _cx = bx;
                beginSend(_g_max(_cx, _bound0), _g_max(_cy, 0) >> 3, _g_min(_cx + g.width * getScaleX() - 1, _bound1 - 1), _g_min(_cy + getScaleY() * 8 - 1, _h - 1) >> 3, false);
                for (uint8_t x = 0; x < g.width; x++) {
                    uint8_t b = pgm_read_byte(g.bitmap + x * chunks + y);
                    _drawByte(b, prev);
                    prev = b;
                }
                endSend();
                _cy += getScaleY() * 8;
            }
            _cy = by;
        }

        _cx += getIntervalSize();
        return 1;
    }
#endif

#pragma region BYTE
    // ================== BYTE ==================

    // вывести столбик-байт в текущий курсор с учётом масштаба, сглаживания, режима буфера и инверсии, автоматически перенесёт курсор
    void drawByte(uint8_t data) {
        _drawByte(data, 0);
    }
    void drawBytes_P(const uint8_t *bytes, uint16_t len) {
        drawBytes(bytes, len, true);
    }
    void drawBytes(const uint8_t *bytes, uint16_t len, bool pgm = false) {
        gfx::Reader reader(bytes, pgm);
        while (len--) {
            reader.read();
            _drawByte(reader.current(), reader.prev());
        }
    }

    // иконка формата gfxicon_t PROGMEM, первый байт - длина
    void drawIcon(gfxicon_t *icon, bool pgm = true) {
        drawBytes(icon + 1, gfx::iconWidth(icon, pgm), pgm);
    }

#pragma region PRIVATE
    // ================== PRIVATE ==================
   private:
#ifndef GFX_NO_PRINT
    gfx::Decoder decoder;

    bool _checkTextBounds(int glyphw) {
        if (_cy + getLineHeight() < 0) {
            return 0;
        }
        int x = _cx + glyphw * getScaleX();
        if (x < getTextBoundX0()) {
            _cx = x + getIntervalSize();
            return 0;
        }
        if (x >= getTextBoundX1() && _wrap) newLine();
        return 1;
    }
#endif

#pragma region CIRCLE
    // =================== CIRCLE ===================
    // окружность с маской по половине четвертей
    // void _circleMask(int x0, int y0, int r, uint8_t mode, uint8_t mask) {
    //     if (r < 0) return;
    //     int x = 0, y = r, d = -(r >> 1);
    //     switch (r) {
    //         case 0:
    //             return set(x0, y0);
    //         case 1:
    //         case 2:
    //             d = 0;
    //             break;
    //     }
    //     while (x <= y) {
    //         if (mode == GFX_STROKE) {
    //             if (mask & bit(0)) set(x0 + y, y0 - x);
    //             if (mask & bit(1)) set(x0 + x, y0 - y);
    //             if (mask & bit(2)) set(x0 - x, y0 - y);
    //             if (mask & bit(3)) set(x0 - y, y0 - x);
    //             if (mask & bit(4)) set(x0 - y, y0 + x);
    //             if (mask & bit(5)) set(x0 - x, y0 + y);
    //             if (mask & bit(6)) set(x0 + x, y0 + y);
    //             if (mask & bit(7)) set(x0 + y, y0 + x);
    //         } else {
    //             if (mask & bit(0)) lineV(x0 + y, y0 - x, y0);
    //             if (mask & bit(1)) lineV(x0 + x, y0 - y, y0);
    //             if (mask & bit(2)) lineV(x0 - x, y0 - y, y0);
    //             if (mask & bit(3)) lineV(x0 - y, y0 - x, y0);
    //             if (mask & bit(4)) lineV(x0 - y, y0, y0 + x);
    //             if (mask & bit(5)) lineV(x0 - x, y0, y0 + y);
    //             if (mask & bit(6)) lineV(x0 + x, y0, y0 + y);
    //             if (mask & bit(7)) lineV(x0 + y, y0, y0 + x);
    //         }
    //         d += (d <= 0) ? (++x) : -(--y);
    //     }
    // }

    void _corners(int x0, int y0, int w, int h, int r, uint8_t mode) {
        if (r <= 0) return;
        int x = 0, px = -1, py = r, y = r, d = -(r >> 1);
        switch (r) {
            case 1:
            case 2:
                d = 0;
                break;
        }
        while (x <= y) {
            if (mode == GFX_STROKE) {
                set(x0 - x, y0 - y);
                set(x0 - y, y0 - x);
                set(x0 - y, y0 + x + h);
                set(x0 - x, y0 + y + h);
                set(x0 + x + w, y0 + y + h);
                set(x0 + y + w, y0 + x + h);
                set(x0 + y + w, y0 - x);
                set(x0 + x + w, y0 - y);
            } else {
                if (py != y) {
                    lineV(x0 - py, y0 - x, y0);
                    lineV(x0 - py, y0 + h, y0 + x + h);

                    lineV(x0 + py + w, y0 - x, y0);
                    lineV(x0 + py + w, y0 + h, y0 + x + h);
                }
                if (px != x) {
                    lineV(x0 - x, y0 - y, y0);
                    lineV(x0 - x, y0 + h, y0 + y + h);

                    lineV(x0 + x + w, y0 - y, y0);
                    lineV(x0 + x + w, y0 + h, y0 + y + h);
                }
                px = x;
                py = y;
            }
            d += (d <= 0) ? (++x) : -(--y);
        }
    }

#pragma region LINE
    // =================== LINE ===================
    void _line(int x0, int y0, int x1, int y1, bool first = true) {
        // http://members.chello.at/~easyfilter/bresenham.html
        int dx = x1 - x0;
        int dy = y1 - y0;
        int sx = 1, sy = 1;
        if (dx < 0) dx = -dx, sx = -1;
        if (dy < 0) dy = -dy, sy = -1;
        int err = dx - dy;
        int e2 = 0;
        if (first) set(x0, y0);
        while (x0 != x1 || y0 != y1) {
            e2 = err * 2;
            if (e2 > -dy) {
                err -= dy;
                x0 += sx;
            }
            if (e2 < dx) {
                err += dx;
                y0 += sy;
            }
            set(x0, y0);
        }
    }
    void _lineAdd(int x0, int y0, int x1, int y1) {
        int dx = x1 - x0;
        int dy = y1 - y0;
        int sx = 1, sy = 1;
        if (dx < 0) dx = -dx, sx = -1;
        if (dy < 0) dy = -dy, sy = -1;
        int dx2 = dx * 2;
        int dy2 = dy * 2;
        set(x0, y0);
        if (dx > dy) {
            int err = dy2 - dx;
            while (x0 != x1) {
                x0 += sx;
                if (err >= 0) {
                    set(x0, y0);
                    y0 += sy;
                    set(x0 - sx, y0);
                    err -= dx2;
                }
                err += dy2;
                set(x0, y0);
            }
        } else {
            int err = dx2 - dy;
            while (y0 != y1) {
                y0 += sy;
                if (err >= 0) {
                    set(x0, y0);
                    x0 += sx;
                    set(x0, y0 - sy);
                    err -= dy2;
                }
                err += dx2;
                set(x0, y0);
            }
        }
    }
    inline bool _lineChunk(int &a0, int &a1, int sa, int &b0, int &b1, int sb, int &err, int s1, int s2) {
        bool move = 0;
        a0 += sa;
        a1 += sa;
        if (err >= 0) {
            b0 += sb;
            b1 += sb;
            err += s1;
            move = 1;
        }
        err += s2;
        return move;
    }
    void _lineThick(int x0, int y0, int x1, int y1, uint8_t stroke) {
        // https://github.com/ArminJo/Arduino-BlueDisplay/blob/master/src/LocalGUI/ThickLine.hpp
        int dx = y1 - y0;
        int dy = x0 - x1;
        int sx = 1, sy = 1;
        if (dx < 0) dx = -dx, sx = -1;
        if (dy < 0) dy = -dy, sy = -1;

        int dx2 = dx * 2;
        int dy2 = dy * 2;
        int err = (dx > dy) ? (dy2 - dx) : (dx2 - dy);

        int t = err;
        for (int i = stroke / 2; i > 0; i--) {
            if (dx > dy) _lineChunk(x0, x1, -sx, y0, y1, -sy, t, -dx2, dy2);
            else _lineChunk(y0, y1, -sy, x0, x1, -sx, t, -dy2, dx2);
        }
        _line(x0, y0, x1, y1);

        t = err;
        for (int i = stroke; i > 1; i--) {
            bool ret = (dx > dy) ? _lineChunk(x0, x1, sx, y0, y1, sy, t, -dx2, dy2) : _lineChunk(y0, y1, sy, x0, x1, sx, t, -dy2, dx2);
            ret ? _lineAdd(x0, y0, x1, y1) : _line(x0, y0, x1, y1);
        }
    }

#pragma region BITMAP
    // =================== BITMAP ===================
    void _drawBitmap(int x, int y, gfx::Reader &reader) {
        uint16_t wh[2];
        reader.read(wh);
        _bitmap(x, y, wh[0], wh[1], reader);
    }

    void _bitmap(int x, int y, int16_t w, int16_t h, gfx::Reader &reader) {
        if (!w || !h || x + w <= 0 || y + h <= 0 || x >= _w || y >= _h) return;

        int16_t chunks = (h + 8 - 1) >> 3;
        if (x < 0) {
            x = -x;
            reader.bytes += x * chunks;
            w -= x;
            x = 0;
        }
        beginSend(x, _g_max(y, 0) >> 3, _g_min(x + w - 1, _w - 1), _g_min(y + h - 1, _h - 1) >> 3, false);

        uint8_t shift = y & 0b111;
        bool extra = (((h + shift) + 8 - 1) >> 3) != chunks;  // real chunks != chunks
        y -= shift;

        for (int16_t xx = 0; xx < w; xx++) {
            if (x >= _w) return;
            for (int16_t ch = 0; ch < chunks; ch++) {
                reader.read();
                if (x >= 0 && y >= 0 && y < _h) {
                    if (shift) {
                        if (ch) fastByte(x, y, (reader.current() << shift) | (reader.prev() >> (8 - shift)));
                        else fastByte(x, y, reader.current() << shift);
                    } else {
                        fastByte(x, y, reader.current());
                    }
                }
                y += 8;
            }
            if (extra && x >= 0 && y >= 0 && y < _h) fastByte(x, y, reader.current() >> (8 - shift));
            x++;
            y -= chunks * 8;
        }

        endSend();
    }

#pragma region BITPACK
    // =================== BITPACK ===================
#ifndef GFX_NO_PACK
    void _drawBitpack(int x, int y, gfx::Reader &reader) {
        uint16_t wh[2];
        reader.read(wh);
        _drawBitpack(x, y, gfx::Unpacker(reader, wh[0], wh[1]));
    }

    void _drawBitpack(int x, int y, gfx::Unpacker pack, bool bounds = false) {
        uint16_t cur = 0;
        while (true) {
            delay(0);
            if (!pack.width) break;
            gfx::Unpacker::Chunk chunk = pack.readChunk();
            while (chunk.size) {
                if (cur + chunk.size >= pack.height) {
                    chunk.size -= pack.height - cur;
                    if (chunk.value && (bounds ? _inBounds(x) : true)) {
#ifndef GFX_NO_SCALE
                        if (_scaleX == 1 && _scaleY == 1) {
                            lineV(x, y + cur, y + pack.height - 1);
                        } else {
                            rect(x, y + cur * _scaleY, x + _scaleX - 1, y + pack.height * _scaleY - 1);
                        }
#else
                        lineV(x, y + cur, y + pack.height() - 1);
#endif
                    }
                    cur = 0;
                    x += getScaleX();
                    pack.width--;
                } else {
                    if (chunk.value && (bounds ? _inBounds(x) : true)) {
#ifndef GFX_NO_SCALE
                        if (_scaleX == 1 && _scaleY == 1) {
                            lineV(x, y + cur, y + cur + chunk.size - 1);
                        } else {
                            rect(x, y + cur * _scaleY, x + _scaleX - 1, y + (cur + chunk.size) * _scaleY - 1);
                        }
#else
                        lineV(x, y + cur, y + cur + chunk.size - 1);
#endif
                    }
                    cur += chunk.size;
                    chunk.size = 0;
                }
            }
        }
    }
#endif

#pragma region BYTE
    // =================== BYTE ===================
    void _byte(int x, int y, uint8_t b) {
        if (y >= 0 && y < _h) fastByte(x, y, b);
    }

    void _drawByte(uint8_t data, uint8_t prev) {
#ifndef GFX_NO_SCALE
        if (_scaleX == 1 && _scaleY == 1) {
#endif
            if (_inBounds(_cx)) {
                uint8_t shift = _cy & 0b111;
                if (shift) {
                    _byte(_cx, _cy - shift, data << shift);
                    _byte(_cx, _cy - shift + 8, data >> (8 - shift));
                } else {
                    _byte(_cx, _cy, data);
                }
            }
            _cx++;

#ifndef GFX_NO_SCALE
        } else {
            // make frame
            union {
                uint64_t raw = 0;
                uint8_t bytes[8];
            } frame;

            uint8_t mask = (1 << _scaleY) - 1;
            uint8_t temp = data;
            uint8_t i = 8;
            while (i--) {
                frame.raw <<= _scaleY;
                if (temp & (1 << 7)) {
                    frame.raw |= mask;
                }
                temp <<= 1;
            }

            // write frame
            i = _scaleX;
            uint8_t shift = _cy & 0b111;
            if (shift) frame.raw <<= shift;

            while (i--) {
                if (_inBounds(_cx)) {
                    for (uint8_t j = 0; j < _scaleY + !!shift; j++) {
                        _byte(_cx, _cy - shift + j * 8, frame.bytes[j]);
                    }
                }
                _cx++;
            }

// smooth
#ifndef GFX_NO_SMOOTH
            if (_smooth && prev) {
                uint8_t pchunk = 0;
                for (uint8_t i = 0; i < 8; i++) {
                    uint8_t chunk = ((prev & 1) << 1) | (data & 1);
                    data >>= 1;
                    prev >>= 1;
                    if (i) {
                        uint8_t block = (pchunk << 2) | chunk;
                        if (block == 0b1001 || block == 0b0110) {
                            int16_t x = _cx - _scaleX;
                            int16_t y = _cy + i * _scaleY - 1;
                            for (uint8_t yy = _scaleY - 1, xx = 0; yy > 0; yy--, xx++) {
                                if (_inBounds(x + xx)) {
                                    (block == 0b1001) ? lineV(x + xx, y - yy + 1, y) : lineV(x + xx, y, y + yy);
                                }
                                if (_inBounds(x - 1 - xx)) {
                                    (block == 0b1001) ? lineV(x - 1 - xx, y, y + yy) : lineV(x - 1 - xx, y + 1 - yy, y);
                                }
                            }
                        }
                    }
                    pchunk = chunk;
                }
            }
#endif
        }
#endif  // GFX_NO_SCALE
    }

#pragma region FLOOD
    // =================== FLOOD ===================
    template <typename T>
    void _flood(T x, T y) {
        T y0 = y;
        while (y0 > 0) {
            if (fastGet(x, y0 - 1) == getColor()) break;
            y0--;
        }
        while (y < T(_h - 1)) {
            if (fastGet(x, y + 1) == getColor()) break;
            y++;
        }

        fastRect(x, y0, x, y);  // fastLineV(x, y0, y);

        while (y >= y0) {
            if (x > 0 && fastGet(x - 1, y) != getColor()) {
                _flood(T(x - 1), y);
            }
            if (x < T(_w - 1) && fastGet(x + 1, y) != getColor()) {
                _flood(T(x + 1), y);
            }
            if (!y) break;
            y--;
        }
    }

    inline uint8_t _fillByte(uint8_t from, uint8_t len) {
        return ((1 << len) - 1) << from;
    }
};