[![latest](https://img.shields.io/github/v/release/GyverLibs/Stamp.svg?color=brightgreen)](https://github.com/GyverLibs/Stamp/releases/latest/download/Stamp.zip)
[![PIO](https://badges.registry.platformio.org/packages/gyverlibs/library/Stamp.svg)](https://registry.platformio.org/libraries/gyverlibs/Stamp)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD%24%E2%82%AC%20%D0%9F%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B0%D1%82%D1%8C-%D0%B0%D0%B2%D1%82%D0%BE%D1%80%D0%B0-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/Stamp?_x_tr_sl=ru&_x_tr_tl=en)

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# Stamp
Библиотека для хранения и манипуляции со временем
- Более удобная в использовании, чем встроенная time.h
- Используются более быстрые алгоритмы преобразования, чем в time.h
- Хранит дату и время в 4 байт переменной (секунды с 01.01.2000)
- Преобразует в год, месяц, день, часы, минуты, секунды, день недели, день года
- Может считать и поддерживать время на базе millis()
- Парсинг из строк
- Работает ~до 2106 года

### Совместимость
Совместима со всеми Arduino платформами (используются Arduino-функции)

## Содержание
- [Документация](#reference)
- [Пример](#example)
- [Версии](#versions)
- [Установка](#install)
- [Баги и обратная связь](#feedback)

<a id="reference"></a>

## Документация
### TimeZone
```cpp
void setStampZone(int zone);    // установить глобальную часовую зону в часах или минутах
int getStampZone();             // получить глобальную часовую зону в минутах
```

### Макросы
```cpp
BUILD_STAMP_LOCAL   // локальное время компиляции (с часовой зоной компьютера)
```

### DaySeconds
Хранит время в секундах с начала текущих суток
```cpp
DaySeconds(uint8_t hour, uint8_t minute, uint8_t second);

void set(uint8_t hour, uint8_t minute, uint8_t second);

uint32_t seconds = 0;
```

### Datime
Хранит дату и время в отдельных переменных:
```cpp
uint16_t year;      // год
uint8_t month;      // месяц (1.. 12)
uint8_t day;        // день месяца (1.. 28-31)
uint8_t hour;       // час (0.. 23)
uint8_t minute;     // минута (0.. 59)
uint8_t second;     // секунда (0.. 59)
uint8_t weekDay;    // день недели (1 пн.. 7 вс) ISO 8601
uint16_t yearDay;   // день года (1.. 365-366)

// указанные диапазоны справедливы для всех остальных функций библиотеки!
```

```cpp
Datime() {}
Datime(const char* str);
Datime(uint32_t unix);
Datime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
Datime(uint16_t yh, uint16_t mm, uint16_t ds);

Datime& operator=(uint32_t unix);

// ============= SET =============
// установить время (год, месяц, день, час, минута, секунда)
void set(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);

// установить время (год, месяц, день) или (час, минута, секунда)
void set(uint16_t yh, uint16_t mm, uint16_t ds);

// установить из unix времени и глобального часового пояса setStampZone
void set(uint32_t unix);

// =========== EXPORT ============
// вывести в секунды с начала текущих суток
uint32_t daySeconds();

// вывести в unix-секунды
uint32_t getUnix();

// ========== TO STRING ==========
// вывести дату в формате "dd.mm.yyyy". Вернёт указатель на конец строки
char* dateToChar(char* buf);
String dateToString();

// вывести время в формате "hh:mm:ss". Вернёт указатель на конец строки
char* timeToChar(char* buf);
String timeToString();

// вывести в формате dd.mm.yyyy hh:mm:ss. Вернёт указатель на конец строки
char* toChar(char* buf, char div = ' ');
String toString(char div = ' ');

// ========== PARSE ==========
// yyyy-mm-dd
bool parseDate(const char* s);

// hh:mm:ss
bool parseTime(const char* s);

// hh:mm:ss или yyyy-mm-dd или yyyy-mm-ddThh:mm:ss
bool parse(const char* s);

// <day_week>, dd <month> yyyy hh:mm:ss
bool parseHTTP(const char* s);

// =========== COMPARE ===========
// одинаковое время
bool equals(const Datime& dt);

bool operator==(uint32_t u);
bool operator!=(uint32_t u);
bool operator>(uint32_t u);
bool operator>=(uint32_t u);
bool operator<(uint32_t u);
bool operator<=(uint32_t u);

bool operator==(DaySeconds ds);
bool operator!=(DaySeconds ds);
bool operator>(DaySeconds ds);
bool operator>=(DaySeconds ds);
bool operator<(DaySeconds ds);
bool operator<=(DaySeconds ds);

// =========== MISC ===========
// високосный ли год
bool isLeap();

// дата и время корректны
bool valid();

// дата 01.01.2000
bool isY2K();

// день года как индекс массива от 0 до 365 независимо от високосного года. 29 февраля имеет индекс 59
uint16_t dayIndex();

// ============= ADD =============
// добавить секунды
void addSeconds(uint32_t s);

// добавить минуты
void addMinutes(uint32_t m);

// добавить часы
void addHours(uint32_t h);

// добавить дни
void addDays(uint32_t d);

// ============= NEXT =============
// следующая секунда
void nextSecond();

// следующая минута (xx:xx:00)
void nextMinute();

// следующий час (xx:00:00)
void nextHour();

// следующий день (00:00:00)
void nextDay();

// следующий месяц (1 число 00:00:00)
void nextMonth();

// обновить weekDay и yearDay исходя из текущей даты (после ручного изменения)
void updateDays();

// ============= АЛГОРИТМ =============
// Алгоритм преобразования времени задаётся перед подключением библиотеки
// для исследования и просто так чтобы было
#define UNIX_ALG UNIX_ALG_0       // ~402us и ~94B Flash (AVR)
#define UNIX_ALG UNIX_ALG_1       // ~298us и ~138B Flash (AVR)
#define UNIX_ALG UNIX_ALG_2       // ~216us и ~584B Flash (AVR)
#define UNIX_ALG UNIX_ALG_3       // ~297us и ~178B Flash (AVR)
#define UNIX_ALG UNIX_ALG_TIME_T  // ~246us и ~842B Flash (AVR)
```

> Все добавления времени в Datime выполняются напрямую, без конвертации в unix, т.е. довольно быстро.

### Stamp
Хранит дату и время в одной переменной `uint32_t` - UNIX время

```cpp
Stamp;
Stamp(const char* str);
Stamp(uint32_t unix);
Stamp(const Datime& dt);
Stamp(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
Stamp(uint16_t yh, uint16_t mm, uint16_t ds);

uint32_t unix = 0;

// =========== SET TIME ============
// установить время из Datime
void set(const Datime& dt);

// установить время (год, месяц, день, час, минута, секунда)
void set(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);

// установить время (год, месяц, день) или (час, минута, секунда)
void set(uint16_t yh, uint16_t mm, uint16_t ds);

// =========== ADD ============
// добавить секунды
void addSeconds(uint32_t s);

// добавить минуты
void addMinutes(uint32_t m);

// добавить часы
void addHours(uint32_t h);

// добавить дни
void addDays(uint32_t d);

// =========== PARSE ============
// hh:mm:ss или yyyy-mm-dd или yyyy-mm-ddThh:mm:ss
bool parse(const char* s);

// <day_week>, dd <month> yyyy hh:mm:ss
bool parseHTTP(const char* s);

// =========== OVERLOAD ===========
// получить время в секундах
uint32_t getUnix();

void operator+=(uint32_t t);
void operator-=(uint32_t t);

// STAMP CORE

// =========== GET TIME ===========
// экспортировать в Datime
Datime get();

// экспортировать в переменную типа Datime
void get(Datime& dt);

// ============ TO STRING ============
// вывести дату в формате "dd.mm.yyyy"
char* dateToChar(char* buf);
String dateToString();

// вывести время в формате "hh:mm:ss"
char* timeToChar(char* buf);
String timeToString();

char* toChar(char* buf, char div = ' ');
String toString(char div = ' ');

// ============ EXPORT DAY ============

// получить секунды с начала текущих суток (локальное время)
uint32_t daySeconds();

// ============ EXPORT EPOCH ============

// получить секунды с epoch
uint32_t toSeconds();

// получить минуты с epoch
uint32_t toMinutes();

// получить часы с epoch
uint32_t toHours();

// получить сутки с epoch
uint32_t toDays();

// ============ DATIME ============

// получить текущие секунды
uint8_t second();

// получить текущие минуты
uint8_t minute();

// получить текущие часы
uint8_t hour();

// получить текущий день месяца
uint8_t day();

// получить текущий месяц
uint8_t month();

// получить текущий год
uint16_t year();

// получить текущий день недели
uint8_t weekDay();

// получить текущий день года
uint8_t yearDay();

bool operator==(uint32_t u);
bool operator!=(uint32_t u);
bool operator>(uint32_t u);
bool operator>=(uint32_t u);
bool operator<(uint32_t u);
bool operator<=(uint32_t u);

bool operator==(DaySeconds ds);
bool operator!=(DaySeconds ds);
bool operator>(DaySeconds ds);
bool operator>=(DaySeconds ds);
bool operator<(DaySeconds ds);
bool operator<=(DaySeconds ds);
```

### StampTicker
Тот же `Stamp`, но сохраняет счёт времени на базе `millis()` после синхронизации. Сигналит раз в секунду в 0 миллисекунд новой секунды. Имеет тикер, нужно вызывать его в `loop()`.

```cpp
// установить unix и миллисекунды
StampTicker(uint32_t unix = 0, uint16_t ms = 0);

// установить unix и миллисекунды
void update(uint32_t unix, uint16_t ms = 0);

// пропустить отставшие секунды (вызывать после update)
void skipTicks();

// время синхронизировано
bool synced();

// время синхронизировано
operator bool();

// секундный флаг
bool newSecond();

// подключить функцию-обработчик новой секунды (вида void f())
void attachSecond(SecondHandler handler);

// отключить функцию-обработчик новой секунды
void detachSecond();

// тикер, вызывать в loop. Вернёт true на новой секунде
bool tick();

// получить текущий unix
uint32_t getUnix();

// получить миллисекунды текущей секунды
uint16_t ms();

// получить миллисекунды с epoch
uint64_t unixMs();
```

### StampSync
Тот же `Stamp`, но сохраняет счёт времени на базе `millis()` после синхронизации. Работает без тикера.

```cpp
StampSync(uint32_t unix = 0, uint16_t ms = 0);

// установить unix и миллисекунды
void update(uint32_t unix, uint16_t ms = 0);

// время синхронизировано
bool synced();

// время синхронизировано
operator bool();

// получить текущий unix
uint32_t getUnix();

// получить миллисекунды текущей секунды
uint16_t ms();

// получить миллисекунды с epoch
uint64_t unixMs();
```

### Утилиты
Набор функций для работы со временем и датой

```cpp
// время в секунды
uint32_t StampUtils::timeToSeconds(uint8_t hours, uint8_t minutes, uint8_t seconds);

// високосный год
bool StampUtils::isLeap(uint16_t year);

// дней в месяце без учёта года (февраль 28)
uint8_t StampUtils::daysInMonth(uint8_t month);

// дней в месяце с учётом високосного года
uint8_t StampUtils::daysInMonth(uint8_t month, uint16_t year);

// дней года к месяцу (янв 0, фев 31, март 59/60...)
uint16_t StampUtils::daysToMonth(uint8_t month, uint16_t year);

// дата в день текущего года (начиная с 1)
uint16_t StampUtils::dateToYearDay(uint8_t day, uint8_t month, uint16_t year);

// дата в день недели (пн 1.. вс 7)
uint8_t StampUtils::dateToWeekDay(uint8_t day, uint8_t month, uint16_t year);

// дата в количество дней с 01.01.2000 (начиная с 0)
uint16_t StampUtils::dateToDays2000(uint8_t day, uint8_t month, uint16_t year);

// дата в unix время, zone в минутах
uint32_t StampUtils::dateToUnix(uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute, uint8_t seconds, int16_t zone = 0);

```

<a id="example"></a>

## Пример
### Конвертация

```cpp
Stamp s(2023, 5, 8, 16, 26, 0);
Serial.println(s.getUnix());
Serial.println(s.toString());

Datime d = s.get();
Serial.println(d.year);
Serial.println(d.month);
Serial.println(d.day);

d.year = 2022;  // изменили год
s.set(d);       // обновили Stamp
Serial.println(s.toString());
```

### Тикер
```cpp
StampTicker st;


void setup() {
    Serial.begin(115200);
    st.update(1695146928);
}

void loop() {
    // каждую секунду
    if (st.tick()) {
        Serial.println(st.toString());
        
        // проверка совпадения времени ("будильник")
        DaySeconds ds(5, 5, 5);  // 5 часов, 5 минут, 5 секунд
        if (st == ds) {}  // можно сравнивать напрямую
    }
}
```

<a id="versions"></a>
## Версии
- v1.0
- v1.1 - добавлено много новых инструментов и возможностей
- v1.2.2 - оптимизация, рефакторинг, улучшен StampTicker

<a id="install"></a>
## Установка
- Библиотеку можно найти по названию **Stamp** и установить через менеджер библиотек в:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Скачать библиотеку](https://github.com/GyverLibs/Stamp/archive/refs/heads/main.zip) .zip архивом для ручной установки:
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