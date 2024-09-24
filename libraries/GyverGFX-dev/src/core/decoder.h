#pragma once
#include <Arduino.h>

namespace gfx {

class Decoder {
   public:
    // получить индекс символа в таблице шрифта. -1 - пропустить вывод
    int16_t getIndex(uint8_t data) {
        if (data < 33 || data == 127) return -1;

        if (data > 127) {
            if (data > 191) {
                _prev = data;
                data = 0;
            } else if (_prev == 208 && data == 129) {
                data = 192;  // Ё
                _prev = 0;
            } else if (_prev == 209 && data == 145) {
                data = 193;  // ё
                _prev = 0;
            }
        }
        if (!data) return -1;

        data -= 33;
        switch (data) {
            case 0 ... 93:
                return data;
            case 95 ... 110:
                return data + 142 - 95;
            case 111 ... 142:
                return data + 94 - 111;
            case 143 ... 158:
                return data + 126 - 143;
            default:  // other
                return data - 1;
        }
        /*
        char    data        index
        !-~     0-93        0-93
        A-Я     111-142     94-125
        а-п     143-158     126-141
        р-я     95-110      142-157
        Ё       159         158
        ё       160         159
        */
    }

   private:
    uint8_t _prev = 0;
};

}  // namespace gfx