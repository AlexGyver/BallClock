#pragma once
#include <Arduino.h>

#include "DaySeconds.h"
#include "StampUtils.h"
#include "stamp_zone.h"

#define UNIX_ALG_0 0       // ~402us / ~94B Flash (AVR)
#define UNIX_ALG_1 1       // ~298us / ~138B Flash (AVR)
#define UNIX_ALG_2 2       // ~216us / ~584B Flash (AVR)
#define UNIX_ALG_3 3       // ~297us / ~178B Flash (AVR)
#define UNIX_ALG_TIME_T 4  // ~246us / ~842B Flash (AVR)

#ifndef UNIX_ALG
#define _UNIX_ALG UNIX_ALG_3
#endif

#if _UNIX_ALG == UNIX_ALG_TIME_T
#include <time.h>
#endif

class Datime {
   public:
    // год
    uint16_t year = 2000;

    // месяц (1.. 12)
    uint8_t month = 1;

    // день месяца (1.. 28-31)
    uint8_t day = 1;

    // час (0.. 23)
    uint8_t hour = 0;

    // минута (0.. 59)
    uint8_t minute = 0;

    // секунда (0.. 59)
    uint8_t second = 0;

    // день недели (1 пн.. 7 вс)
    uint8_t weekDay = 1;

    // день года (1.. 365-366)
    uint16_t yearDay = 1;

    // ========= CONSTRUCTOR =========
    Datime() {}
    Datime(const Datime& dat) = default;
    Datime(const char* str) {
        parse(str);
    }
    Datime(uint32_t unix) {
        set(unix);
    }
    Datime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
        set(year, month, day, hour, minute, second);
    }
    Datime(uint16_t yh, uint16_t mm, uint16_t ds) {
        set(yh, mm, ds);
    }

    Datime& operator=(uint32_t unix) {
        set(unix);
        return *this;
    }

    // ============= SET =============
    // установить время (год, месяц, день, час, минута, секунда)
    void set(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
        this->year = year;
        this->month = month;
        this->day = day;
        this->hour = hour;
        this->minute = minute;
        this->second = second;
        updateDays();
    }

    // установить время (год, месяц, день) или (час, минута, секунда)
    void set(uint16_t yh, uint16_t mm, uint16_t ds) {
        if (yh >= 24) {
            year = yh;
            month = mm;
            day = ds;
        } else {
            hour = yh;
            minute = mm;
            second = ds;
        }
        updateDays();
    }

    // установить из unix времени и глобального часового пояса setStampZone
    void set(uint32_t unix) {
        if (!unix) return;
        unix += getStampZone() * 60l;

#if _UNIX_ALG == UNIX_ALG_0
        this->second = unix % 60ul;
        unix /= 60ul;
        this->minute = unix % 60ul;
        unix /= 60ul;
        this->hour = unix % 24ul;
        unix /= 24ul;
        this->weekDay = (unix + 3) % 7 + 1;
        uint32_t z = unix + 719468;
        uint8_t era = z / 146097ul;
        uint16_t doe = z - era * 146097ul;
        uint16_t yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
        uint16_t y = yoe + era * 400;
        uint16_t doy = doe - (yoe * 365 + yoe / 4 - yoe / 100);
        uint16_t mp = (doy * 5 + 2) / 153;
        this->day = doy - (mp * 153 + 2) / 5 + 1;
        this->month = mp + (mp < 10 ? 3 : -9);
        y += (this->month <= 2);
        this->year = y;
        this->yearDay = StampUtils::dateToYearDay(this->day, this->month, this->year);

#elif _UNIX_ALG == UNIX_ALG_1
        int32_t days, rem;
        int years400, years100, years4, remainingyears;

        days = unix / 86400ul - 11017;
        rem = unix % 86400ul;
        if (rem < 0) {
            rem += 86400ul;
            days--;
        }

        this->hour = rem / 3600ul;
        rem %= 3600ul;
        this->minute = rem / 60ul;
        this->second = rem % 60ul;

        if ((this->weekDay = ((3 + days) % 7)) < 0) this->weekDay += 7;
        if (!this->weekDay) this->weekDay = 7;

        years400 = days / 146097L;
        days -= years400 * 146097L;
        if (days < 0) {
            days += 146097L;
            years400--;
        }

        years100 = days / 36524L;
        if (years100 == 4) years100--;
        days -= years100 * 36524L;
        years4 = days / 1461L;
        days -= years4 * 1461L;
        remainingyears = days / 365L;
        if (remainingyears == 4) remainingyears--;
        days -= remainingyears * 365L;
        this->year = 2000 + years400 * 400 + years100 * 100 + years4 * 4 + remainingyears;
        bool yearleap = remainingyears == 0 && (years4 != 0 || years100 == 0);

        this->yearDay = days + 31 + 28 + yearleap;
        if (this->yearDay >= 365u + yearleap) {
            this->yearDay -= 365u + yearleap;
            this->year++;
        }
        this->yearDay++;

        this->month = 2;
        while (1) {
            uint8_t dm = StampUtils::daysInMonth(this->month + 1);  // from 0. Feb 28 here
            if (this->month == 1) dm++;                             // 1 Feb
            if (days < dm) break;
            days -= dm;
            if (++this->month >= 12) this->month = 0;
        }
        this->month++;
        this->day = days + 1;

#elif _UNIX_ALG == UNIX_ALG_2
        int32_t fract;
        uint16_t days, n, leapyear, years;
        ldiv_t lresult;
        div_t result;
        if (unix < 946684800) unix = 946684800;

        unix -= 946684800;              // to 2000-01-01 UTC
        days = unix / 86400UL;          // 38753+
        fract = unix - days * 86400UL;  // 86400

        lresult = ldiv(fract, 60L);  // 86400
        this->second = lresult.rem;
        result = div((int)lresult.quot, 60);  // 1440
        this->minute = result.rem;
        this->hour = result.quot;

        n = days + 6;
        n %= 7;
        this->weekDay = n;
        if (!this->weekDay) this->weekDay = 7;

        lresult = ldiv((long)days, 36525L);  // 38753+
        years = 100 * lresult.quot;
        lresult = ldiv(lresult.rem, 1461L);  // 36525
        years += 4 * lresult.quot;
        days = lresult.rem;  // <- 1461
        if (years > 100) days++;

        leapyear = 1;
        if (years == 100) leapyear = 0;
        n = 364 + leapyear;

        if (days > n) {
            days -= leapyear;
            leapyear = 0;
            result = div(days, 365);  // 1461
            years += result.quot;
            days = result.rem;  // <- 365
        }
        this->year = 100 + years + 1900;
        this->yearDay = days + 1;

        n = 59 + leapyear;
        if (days < n) {
            result = div(days, 31);  // 1461
            this->month = result.quot;
            this->day = result.rem;
        } else {
            days -= n;
            result = div(days, 153);  // 1461
            this->month = 2 + result.quot * 5;
            result = div(result.rem, 61);  // 153
            this->month += result.quot * 2;
            result = div(result.rem, 31);  // 61
            this->month += result.quot;
            this->day = result.rem;
        }

        this->day++;
        this->month++;

#elif _UNIX_ALG == UNIX_ALG_3
        unix -= 946684800;
        this->second = unix % 60ul;
        unix /= 60ul;
        this->minute = unix % 60ul;
        unix /= 60ul;
        this->hour = unix % 24ul;

        uint16_t days = unix / 24ul;
        this->weekDay = (days + 5) % 7 + 1;

        bool leap = 0;
        for (this->year = 0;; this->year++) {
            leap = !(this->year & 3);
            if (days < 365u + leap) break;
            days -= 365u + leap;
        }
        this->year += 2000;
        this->yearDay = days + 1;

        for (this->month = 1; this->month < 12; this->month++) {
            uint8_t dm = StampUtils::daysInMonth(this->month);
            if (leap && this->month == 2) dm++;
            if (days < dm) break;
            days -= dm;
        }
        this->day = days + 1;

#elif _UNIX_ALG == UNIX_ALG_TIME_T
        time_t t = unix - 946684800;
        tm tt;
        gmtime_r(&t, &tt);
        this->year = tt.tm_year + 1900;
        this->month = tt.tm_mon + 1;
        this->day = tt.tm_mday;
        this->hour = tt.tm_hour;
        this->minute = tt.tm_min;
        this->second = tt.tm_sec;
        this->weekDay = tt.tm_wday;
        if (!this->weekDay) this->weekDay = 7;
        this->yearDay = tt.tm_yday + 1;
#endif
    }

    // =========== EXPORT ============
    // вывести в секунды с начала текущих суток
    uint32_t daySeconds() const {
        uint32_t sec = second;
        if (minute) sec += minute * 60;
        if (hour) sec += hour * 3600ul;
        return sec;
    }

    // вывести в unix-секунды
    uint32_t getUnix() const {
        return StampUtils::dateToUnix(day, month, year, hour, minute, second, getStampZone());
    }

    // ========== TO STRING ==========
    // вывести дату в формате "dd.mm.yyyy". Вернёт указатель на конец строки
    char* dateToChar(char* buf) const {
        buf[0] = day / 10 + '0';
        buf[1] = day % 10 + '0';
        buf[2] = '.';
        buf[3] = month / 10 + '0';
        buf[4] = month % 10 + '0';
        buf[5] = '.';
        itoa(year, buf + 6, DEC);
        return buf + 10;
    }

    // вывести дату в формате "dd.mm.yyyy"
    String dateToString() const {
        char buf[10];
        dateToChar(buf);
        return buf;
    }

    // вывести время в формате "hh:mm:ss". Вернёт указатель на конец строки
    char* timeToChar(char* buf) const {
        buf[0] = hour / 10 + '0';
        buf[1] = hour % 10 + '0';
        buf[2] = ':';
        buf[3] = minute / 10 + '0';
        buf[4] = minute % 10 + '0';
        buf[5] = ':';
        buf[6] = second / 10 + '0';
        buf[7] = second % 10 + '0';
        buf[8] = 0;
        return buf + 8;
    }

    // вывести время в формате "hh:mm:ss"
    String timeToString() const {
        char buf[8];
        timeToChar(buf);
        return buf;
    }

    // вывести в формате dd.mm.yyyy hh:mm:ss. Вернёт указатель на конец строки
    char* toChar(char* buf, char div = ' ') const {
        char* s = buf;
        s = dateToChar(s);
        s[0] = div;
        s = timeToChar(s + 1);
        return s;
    }
    String toString(char div = ' ') const {
        char buf[20];
        toChar(buf, div);
        return buf;
    }

    // ========== PARSE ==========
    // yyyy-mm-dd
    bool parseDate(const char* s) {
        if (strlen(s) < 10) return 0;
        year = atoi(s);
        s = strchr(s, '-');
        if (!s) return 0;
        month = atoi(++s);
        s = strchr(s, '-');
        if (!s) return 0;
        day = atoi(++s);
        return 1;
    }

    // hh:mm:ss
    bool parseTime(const char* s) {
        if (strlen(s) < 8) return 0;
        hour = atoi(s);
        s = strchr(s, ':');
        if (!s) return 0;
        minute = atoi(++s);
        s = strchr(s, ':');
        if (!s) return 0;
        second = atoi(++s);
        return 1;
    }

    // hh:mm:ss или yyyy-mm-dd или yyyy-mm-ddThh:mm:ss
    bool parse(const char* s) {
        uint16_t len = strlen(s);
        if (len == 19 && s[10] == 'T') {  // dateTtime
            if (!parseDate(s)) return 0;
            if (!parseTime(s + 11)) return 0;
        } else if (len == 10) {  // date
            if (!parseDate(s)) return 0;
        } else if (len == 8) {  // time
            if (!parseTime(s)) return 0;
        } else {
            return 0;
        }
        return 1;
    }

    // <day_week>, dd <month> yyyy hh:mm:ss
    bool parseHTTP(const char* s) {
        char* comma = strchr(s, ',');
        if (!comma) return 0;
        s = comma + 2;
        day = atoi(s);
        month = 0;
        switch (s[3]) {
            case 'J':
                month = (s[4] == 'a') ? 1 : ((s[5] == 'n') ? 6 : 7);
                break;
            case 'F':
                month = 2;
                break;
            case 'A':
                month = (s[5] == 'r') ? 4 : 8;
                break;
            case 'M':
                month = (s[5] == 'r') ? 3 : 5;
                break;
            case 'S':
                month = 9;
                break;
            case 'O':
                month = 10;
                break;
            case 'N':
                month = 11;
                break;
            case 'D':
                month = 12;
                break;
            default:
                return 0;
        }
        year = atoi(s + 7);
        hour = atoi(s + 12);
        minute = atoi(s + 15);
        second = atoi(s + 18);
        return 1;
    }

    // =========== COMPARE ===========
    bool operator==(uint32_t u) {
        return getUnix() == u;
    }
    bool operator!=(uint32_t u) {
        return getUnix() != u;
    }
    bool operator>(uint32_t u) {
        return getUnix() > u;
    }
    bool operator>=(uint32_t u) {
        return getUnix() >= u;
    }
    bool operator<(uint32_t u) {
        return getUnix() < u;
    }
    bool operator<=(uint32_t u) {
        return getUnix() <= u;
    }

    bool operator==(DaySeconds ds) const {
        return daySeconds() == ds.seconds;
    }
    bool operator!=(DaySeconds ds) const {
        return daySeconds() != ds.seconds;
    }
    bool operator>(DaySeconds ds) const {
        return daySeconds() > ds.seconds;
    }
    bool operator>=(DaySeconds ds) const {
        return daySeconds() >= ds.seconds;
    }
    bool operator<(DaySeconds ds) const {
        return daySeconds() < ds.seconds;
    }
    bool operator<=(DaySeconds ds) const {
        return daySeconds() <= ds.seconds;
    }

    operator uint32_t() const {
        return getUnix();
    }

    // одинаковое время
    bool equals(const Datime& dt) const {
        return (year == dt.year) && (month == dt.month) && (day == dt.day) && (hour == dt.hour) && (minute == dt.minute) && (second == dt.second);
    }

    // високосный ли год
    bool isLeap() const {
        return StampUtils::isLeap(year);
    }

    // дата и время корректны
    bool valid() const {
        return (year >= 2000) && (month >= 1 && month <= 12) && (day >= 1 && day <= 31) && (hour <= 23) && (minute <= 59) && (second <= 59);
    }

    // дата 01.01.2000
    bool isY2K() const {
        return year == 2000 && month == 1 && day == 1;
    }

    // день года как индекс массива от 0 до 365 независимо от високосного года. 29 февраля имеет индекс 59
    uint16_t dayIndex() const {
        return ((yearDay >= 60) ? (isLeap() ? yearDay : yearDay + 1) : yearDay) - 1;
    }

    // ============= ADD =============
    // добавить секунды
    void addSeconds(uint32_t s) {
        if (!s) return;
        if (s == 1) {
            second++;
            _update();
        } else {
            s += second;
            ldiv_t res;
            res = ldiv(s, 60L);
            second = res.rem;
            addMinutes(res.quot);
        }
    }

    // добавить минуты
    void addMinutes(uint32_t m) {
        if (!m) return;
        if (m == 1) {
            minute++;
            _update();
        } else {
            m += minute;
            ldiv_t res;
            res = ldiv(m, 60L);
            minute = res.rem;
            addHours(res.quot);
        }
    }

    // добавить часы
    void addHours(uint32_t h) {
        if (!h) return;
        if (h == 1) {
            hour++;
            _update();
        } else {
            h += hour;
            ldiv_t res;
            res = ldiv(h, 24L);
            hour = res.rem;
            addDays(res.quot);
        }
    }

    // добавить дни
    void addDays(uint32_t d) {
        while (d) {
            d--;
            day++;
            weekDay++;
            yearDay++;
            _update();
        }
    }

    // ============= NEXT =============
    // следующая секунда
    void nextSecond() {
        addSeconds(1);
    }

    // следующая минута (xx:xx:00)
    void nextMinute() {
        addMinutes(1);
        second = 0;
    }

    // следующий час (xx:00:00)
    void nextHour() {
        addHours(1);
        second = minute = 0;
    }

    // следующий день (00:00:00)
    void nextDay() {
        addDays(1);
        second = minute = hour = 0;
    }

    // следующий месяц (1 число 00:00:00)
    void nextMonth() {
        month++;
        day = 1;
        second = minute = hour = 0;
        if (month > 12) {
            year++;
            month = 1;
        }
        updateDays();
    }

    // обновить weekDay и yearDay исходя из текущей даты (после ручного изменения)
    void updateDays() {
        weekDay = StampUtils::dateToWeekDay(day, month, year);
        yearDay = StampUtils::dateToYearDay(day, month, year);
    }

   private:
    void _update() {
        if (second > 59) {
            second = 0;
            minute++;
        }
        if (minute > 59) {
            minute = 0;
            hour++;
        }
        if (hour > 23) {
            hour = 0;
            weekDay++;
            yearDay++;
            day++;
        }
        if (weekDay > 7) weekDay = 1;
        if (day > StampUtils::daysInMonth(month, year)) {
            month++;
            day = 1;
        }
        if (month > 12) {
            year++;
            month = 1;
            yearDay = 1;
        }
    }
};