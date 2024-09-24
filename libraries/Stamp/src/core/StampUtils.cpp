#include "StampUtils.h"

static const uint8_t dim_table[] PROGMEM = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

uint32_t StampUtils::timeToSeconds(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    return (hours * 60 + minutes) * 60ul + seconds;
}

bool StampUtils::isLeap(uint16_t year) {
    return !(year & 3) && (!(year % 400) || (year % 100));
}

uint8_t StampUtils::daysInMonth(uint8_t month) {
    return pgm_read_byte(&dim_table[month - 1]);
}

uint8_t StampUtils::daysInMonth(uint8_t month, uint16_t year) {
    return (month == 2 && StampUtils::isLeap(year)) ? (daysInMonth(month) + 1) : daysInMonth(month);
}

uint16_t StampUtils::daysToMonth(uint8_t month, uint16_t year) {
    uint16_t days = 0;
    for (uint8_t i = 1; i < month; i++) days += StampUtils::daysInMonth(i, year);
    return days;
}

uint16_t StampUtils::dateToYearDay(uint8_t day, uint8_t month, uint16_t year) {
    return day + StampUtils::daysToMonth(month, year);
}

uint16_t StampUtils::dateToDays2000(uint8_t day, uint8_t month, uint16_t year) {
    uint16_t yday = StampUtils::dateToYearDay(day, month, year);
    if (year >= 2000) year -= 2000;
    return yday + 365 * year + (year + 3) / 4 - 1;
}

uint8_t StampUtils::dateToWeekDay(uint8_t day, uint8_t month, uint16_t year) {
    return (StampUtils::dateToDays2000(day, month, year) + 5) % 7 + 1;
}

uint32_t StampUtils::dateToUnix(uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute, uint8_t second, int16_t zone) {
    return 946684800ul + (StampUtils::dateToDays2000(day, month, year)) * 86400ul + StampUtils::timeToSeconds(hour, minute, second) - zone * 60;
}