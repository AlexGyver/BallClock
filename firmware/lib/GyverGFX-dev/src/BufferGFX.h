#pragma once
#include <Arduino.h>
#include <GyverGFX.h>

// TODO запоминать графницы fastSet()

class BufferGFX : public GyverGFX, public gfx::Buffer {
   public:
    BufferGFX(uint16_t width, uint16_t height) : GyverGFX(width, height), gfx::Buffer(width, height) {
        _buffer = new uint8_t[width * _hbytes];
        clear();
    }
    ~BufferGFX() {
        delete[] _buffer;
    }

    // 0, 1, 2, 3 по часовой начиная с верха
    void setRotation(uint8_t rot) {
        _rot = rot;
        if (_rot == 1 || _rot == 3) GyverGFX::size(_height, _width);
        else GyverGFX::size(_width, _height);
    }

    BufferGFX(BufferGFX& b) {
        move(b);
    }
    void operator=(BufferGFX& b) {
        move(b);
    }

#if __cplusplus >= 201103L
    BufferGFX(BufferGFX&& b) noexcept {
        move(b);
    }
    void operator=(BufferGFX&& b) noexcept {
        move(b);
    }
#endif

    void move(BufferGFX& b) {
        delete[] _buffer;
        _buffer = b._buffer;
        b._buffer = nullptr;
    }

   protected:
    using gfx::Buffer::bufGet;
    using gfx::Buffer::bufSet;

   private:
    uint8_t _rot = 0;
    uint8_t _uy0, _uy1;//todo
    uint16_t _ux0, _ux1;

    using gfx::Buffer::_buffer;
    using gfx::Buffer::_hbytes;
    using gfx::Buffer::_height;
    using gfx::Buffer::_width;
    using gfx::Buffer::bref;

    void _rotXY(int& x, int& y) {
        switch (_rot) {
            case 1: {
                int yy = y;
                y = x;
                x = _width - yy - 1;
            } break;
            case 2:
                x = _width - x - 1;
                y = _height - y - 1;
                break;
            case 3: {
                int yy = y;
                y = _height - x - 1;
                x = yy;
            } break;
        }
    }

    // установить пиксель цветом getColor(), всегда вызывается внутри дисплея
    void fastSet(int x, int y) {
        if (_buffer) {
            _rotXY(x, y);
            gfx::Buffer::bufSet(x, y, getColor());
        }
    }

    // получить пиксель, всегда вызывается внутри дисплея
    uint16_t fastGet(int x, int y) {
        if (_buffer) {
            _rotXY(x, y);
            return gfx::Buffer::bufGet(x, y);
        }
        return 0;
    }

    // вывести столбик 8 бит (MSB снизу) цветом getColor(). Всегда внутри дисплея, (y) кратен 8
    void fastByte(int x, int y, uint8_t mask) {
        getColor() ? bref(x, y) |= mask : bref(x, y) &= ~mask;
    }

    // заливка всего экрана
    void fastFill(uint16_t color) {
        if (_buffer) memset(_buffer, color, _width * _hbytes);
    }

    /*
    // begin/end
    void fastByte(int x, int y, uint8_t data) {
        if (sending) y = _y * 8, x = _x;

        // send(data);

        if (sending) {
            if (sending == 2 && ++_x > _t1) _x = _t0, _y++;
            else if (sending == 1 && ++_y > _t1) _x++, _y = _t0;
        }
    }
    void beginSend(int x0, int y0, int x1, int y1, bool horizontal) {
        sending = horizontal + 1;
        if (sending == 2) t0 = x0, _t1 = x1;
        else _t0 = y0, _t1 = y1;
        _x = x0, _y = y0;
    }

    void endSend() {
        sending = 0;
    }
    uint8_t sending = 0;  // 1 vert, 2 horiz
    int16_t _x, _y, _t0, _t1;
    */
};