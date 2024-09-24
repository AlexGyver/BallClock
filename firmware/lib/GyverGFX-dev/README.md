[![latest](https://img.shields.io/github/v/release/GyverLibs/GyverGFX.svg?color=brightgreen)](https://github.com/GyverLibs/GyverGFX/releases/latest/download/GyverGFX.zip)
[![PIO](https://badges.registry.platformio.org/packages/gyverlibs/library/GyverGFX.svg)](https://registry.platformio.org/libraries/gyverlibs/GyverGFX)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD%24%E2%82%AC%20%D0%9F%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B0%D1%82%D1%8C-%D0%B0%D0%B2%D1%82%D0%BE%D1%80%D0%B0-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/GyverGFX?_x_tr_sl=ru&_x_tr_tl=en)  

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# GyverGFX
Движок двухмерной графики для дисплеев и матриц

### Совместимость
Совместима со всеми Arduino платформами (используются Arduino-функции)

## Содержание
- [Использование](#usage)
- [Пример](#example)
- [Версии](#versions)
- [Установка](#install)
- [Баги и обратная связь](#feedback)

<a id="usage"></a>

## Использование


#### Встроенные иконки
В библиотеке идёт набор из 155 иконок 8х8, файл *fonts/icons8x8.h*. Выводятся следующим образом:
```cpp
obj.drawBytes_P(gfx_icon::alarm, 8);
obj.drawBytes_P(gfx_icon::email, 8);
```

### RunningGFX
Асинхронная бегущая строка. Не хранит строку внутри себя, поэтому по очевидным причинам поддерживает работу с:
- Глобально созданными `String` - строками
- Глобально созданными `PROGMEM` - строками
- Глобально созданными `char[]` - строками
- Строковыми константами (они изначально являются глобальными)

Особенности:
- Для автоматического обновления строки в классе должен быть реализован метод `update()`
- Строку можно изменять во время движения: `setText()` не запускает движение заново, поэтому при обновлении String-строки можно вызвать setText для автоматического пересчёта длины
- Настройки бегущей строки (масштаб, режим текста) не влияют на дисплей, точнее автоматически устанавливаются обратно
- Один объект управляет одной строкой. если нужно несколько одновременно бегущих строк - создай несколько обьектов, пример ниже

```cpp
```

<a id="example"></a>

## Пример

```cpp
```

```cpp
// пример RunningGFX
// RunningGFX работает с классом, который наследует GyverGFX. Например библиотека GyverMAX7219

MAX7219<4, 1, 5> mtrx;  // одна матрица (1х1), пин CS на D5
RunningGFX run1(&mtrx);
RunningGFX run2(&mtrx);
const char pstr_g[] PROGMEM = "Global pgm string";
String str_g = "123";

void setup() {
  mtrx.begin();       // запускаем
  mtrx.setBright(5);  // яркость 0..15

  run1.setText("hello");  // строковая константа
  // run1.setText(str_g); // глобальная стринга
  run1.setSpeed(15);
  run1.setWindow(0, 8, 0);
  run1.start();

  run2.setText_P(pstr_g); // глобальная PGM строка
  run2.setSpeed(10);
  run2.setWindow(8, 16, 0);
  run2.start();
}

void loop() {
  run1.tick();
  run2.tick();
}
```

<a id="versions"></a>

## Версии
- 2.0.0

<a id="install"></a>

## Установка
- Библиотеку можно найти по названию **GyverGFX** и установить через менеджер библиотек в:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Скачать библиотеку](https://github.com/GyverLibs/GyverGFX/archive/refs/heads/main.zip) .zip архивом для ручной установки:
    - Распаковать и положить в *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Распаковать и положить в *C:\Program Files\Arduino\libraries* (Windows x32)
    - Распаковать и положить в *Документы/Arduino/libraries/*
    - (Arduino IDE) автоматическая установка из .zip: *Скетч/Подключить библиотеку/Добавить .ZIP библиотеку…* и указать скачанный архив
- Читай более подробную инструкцию по установке библиотек [здесь](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Обновление
- Рекомендую всегда обновлять библиотеку: в новых версиях исправляются ошибки и баги, а также проводится оптимизация и добавляются новые фичи
- Через менеджер библиотек IDE: найти библиотеку как при установке и нажать "Обновить"
- Вручную: **удалить папку со старой версией**, а затем положить на её место новую. "Замену" делать нельзя: иногда в новых версиях удаляются файлы, которые останутся при замене и могут привести к ошибкам!

<a id="feedback"></a>

## Баги и обратная связь
При нахождении багов создавайте **Issue**, а лучше сразу пишите на почту [alex@alexgyver.ru](mailto:alex@alexgyver.ru)  
Библиотека открыта для доработки и ваших **Pull Request**'ов!

При сообщении о багах или некорректной работе библиотеки нужно обязательно указывать:
- Версия библиотеки
- Какой используется МК
- Версия SDK (для ESP)
- Версия Arduino IDE
- Корректно ли работают ли встроенные примеры, в которых используются функции и конструкции, приводящие к багу в вашем коде
- Какой код загружался, какая работа от него ожидалась и как он работает в реальности
- В идеале приложить минимальный код, в котором наблюдается баг. Не полотно из тысячи строк, а минимальный код
