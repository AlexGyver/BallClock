#pragma once
// движок бегущей строки для GyverGFX
#include <GyverGFX.h>

#define RG_IDLE 0
#define RG_MOVE 1
#define RG_FINISH 2

class RunningGFX : public gfx::Core {
   public:
    RunningGFX(GyverGFX& gfx) : RunningGFX(&gfx) {}
    RunningGFX(GyverGFX* gfx) : gfx::Core(gfx->width(), gfx->height()), _gfx(gfx) {}

    // установить текст String
    void setText(String& str) {
        setText(str.c_str());
    }

    // установить текст const char*
    void setText(const char* str) {
        _str = str;
        _pgm = false;
    }

    // установить текст F("")
    void setText(const __FlashStringHelper* str) {
        setText_P((PGM_P)str);
    }

    // установить текст из PROGMEM (глобальный)
    void setText_P(PGM_P str) {
        _str = str;
        _pgm = true;
    }

    // установить окно (x0, x1, y)
    void setWindow(int x0, int x1, int y) {
        setTextBound(x0, x1);
        setCursorY(y);
    }

    // установить цвет фона для текста
    void setBackground(uint16_t color) {
        _back = color;
    }

    // использовать фон для текста
    void useBackground(bool use) {
        _use_back = use;
    }

    // установить скорость (шагов в секунду) умолч. 20
    void setSpeed(uint16_t stepSec) {
        if (!stepSec) stepSec = 1;
        _prd = 1000 / stepSec;
    }

    // установить размер шага в пикселях
    void setStep(uint16_t step) {
        _step = step;
    }

    // запустить бегущую строку с начала
    void start() {
        if (_str) {
            _len = _pgm ? textWidth_P(_str) : textWidth(_str);
            _restart();
        } else {
            stop();
        }
    }

    // остановить бегущую строку
    void stop() {
        _run = false;
    }

    // продолжить движение с момента остановки
    void resume() {
        _tmr = millis();
        _run = true;
    }

    // строка движется
    bool running() {
        return _run;
    }

    // тикер. Вернёт 0 в холостом, 1 при новом шаге, 2 при завершении движения
    // Можно передать false чтобы дисплей не обновлялся сам
    uint8_t tick(bool update = true) {
        delay(0);
        if (!_str) return 0;
        if (_run && (uint16_t)((uint16_t)millis() - _tmr) >= _prd) {
            resume();
            return tickManual(update);
        }
        return RG_IDLE;
    }

    // сдвинуть строку на 1 шаг. Можно передать false чтобы дисплей не обновлялся сам
    uint8_t tickManual(bool update = true) {
        if (!_str) return 0;
        gfx::Core coret = *_gfx;
        *((gfx::Core*)_gfx) = *((gfx::Core*)this);

        if (_use_back) {
            _gfx->setColor(_back);
            _gfx->rect(_bound0, _cy, _bound1, _cy + getLineHeight(), GFX_FILL);
        }
        _gfx->setColor(getColor());

        if (_pgm) _gfx->print((const __FlashStringHelper*)_str);
        else _gfx->print(_str);

        *((gfx::Core*)_gfx) = coret;

        if (update) _gfx->update();
        _cx -= _step;
        if (_cx <= _bound0 - (int16_t)_len) {
            _restart();
            return RG_FINISH;
        }
        return RG_MOVE;
    }

   private:
    GyverGFX* _gfx = nullptr;
    const char* _str = nullptr;
    uint16_t _tmr = 0, _prd = 50;
    uint16_t _len = 0, _step = 1, _back = 0;
    bool _pgm = 0, _run = 0, _use_back = true;

    void _restart() {
        setCursorX(getTextBoundX1() + 1);
        resume();
    }
};