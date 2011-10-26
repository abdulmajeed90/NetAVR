/**
 * Copyright 2011 Roy van Dam <roy@8bit.cx>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright notice, this list
 *       of conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ''AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "date.h"

// Days of the week
const char _day_0[] PROGMEM = "Sunday";
const char _day_1[] PROGMEM = "Monday";
const char _day_2[] PROGMEM = "Tuesday";
const char _day_3[] PROGMEM = "Wednesday";
const char _day_4[] PROGMEM = "Thursday";
const char _day_5[] PROGMEM = "Friday";
const char _day_6[] PROGMEM = "Saturday";

// Store in program space
PGM_P _days[] PROGMEM = {
    _day_0, _day_1, _day_2,
    _day_3, _day_4, _day_5,
    _day_6
};

// Days of the week, abbreviations
const char _day_short_0[] PROGMEM = "Sun";
const char _day_short_1[] PROGMEM = "Mon";
const char _day_short_2[] PROGMEM = "Tue";
const char _day_short_3[] PROGMEM = "Wed";
const char _day_short_4[] PROGMEM = "Thu";
const char _day_short_5[] PROGMEM = "Fri";
const char _day_short_6[] PROGMEM = "Sat";

// Store in program space
PGM_P _days_short[] PROGMEM = {
    _day_short_0, _day_short_1, _day_short_2,
    _day_short_3, _day_short_4, _day_short_5,
    _day_short_6
};

// Months of the year
const char _month_00[] PROGMEM = "January";
const char _month_01[] PROGMEM = "February";
const char _month_02[] PROGMEM = "March";
const char _month_03[] PROGMEM = "April";
const char _month_04[] PROGMEM = "May";
const char _month_05[] PROGMEM = "June";
const char _month_06[] PROGMEM = "July";
const char _month_07[] PROGMEM = "August";
const char _month_08[] PROGMEM = "September";
const char _month_09[] PROGMEM = "October";
const char _month_10[] PROGMEM = "November";
const char _month_11[] PROGMEM = "December";

// Store in program space
const char* _months[] PROGMEM = {
    _month_00, _month_01, _month_02,
    _month_03, _month_04, _month_05,
    _month_06, _month_07, _month_08,
    _month_09, _month_10, _month_11
};

// Months of the year, abbreviations
const char _month_short_00[] PROGMEM = "Jan";
const char _month_short_01[] PROGMEM = "Feb";
const char _month_short_02[] PROGMEM = "Mar";
const char _month_short_03[] PROGMEM = "Apr";
const char _month_short_04[] PROGMEM = "May";
const char _month_short_05[] PROGMEM = "Jun";
const char _month_short_06[] PROGMEM = "Jul";
const char _month_short_07[] PROGMEM = "Aug";
const char _month_short_08[] PROGMEM = "Sep";
const char _month_short_09[] PROGMEM = "Oct";
const char _month_short_10[] PROGMEM = "Nov";
const char _month_short_11[] PROGMEM = "Dec";

// Store in program space
const char* _months_short[] PROGMEM = {
    _month_short_00, _month_short_01, _month_short_02,
    _month_short_03, _month_short_04, _month_short_05,
    _month_short_06, _month_short_07, _month_short_08,
    _month_short_09, _month_short_10, _month_short_11
};

// Month day count lookup table
const uint8_t _month_tab[2][12] PROGMEM = {
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

/**
 * @function:   year_is_leap
 * @param:      uint16_t, year.
 * @return:     bool, true if year is leap.
 * @brief:      Determins if the given year is leap.
 */
inline bool
year_is_leap(uint16_t year)
{
    return (!((year) % 4) && (((year) % 100) || !((year) % 400)));
}

/**
 * @function:   year_day_count
 * @param:      uint16_t, year.
 * @return:     uint16_t, amount of days the year has.
 * @brief:      Returns the amount of days in given year.
 */
inline uint16_t
year_day_count(uint16_t year)
{
    return (year_is_leap(year) ? 366 : 365);
}

/**
 * @function:   month_day_count
 * @param:      uint16_t, year.
 * @param:      uint8_t, month.
 * @return:     uint8_t, amount of days in the given month of the year.
 * @brief:      Returns the amount of day in the given month and year.
 */
inline uint8_t
month_day_count(uint16_t year, uint8_t month)
{
    return pgm_read_byte(&(_month_tab[year_is_leap(year)][month]));
}

/**
 * @function:   time_decode
 * @param:      struct datetime_t *, structual date time representation.
 * @param:      timestamp_t, unix timestamp representation of time.
 * @brief:      Parses a unix timestamp into a structual representation.
 */
void
time_decode(struct datetime_t* datetime, timestamp_t timestamp)
{
    uint32_t time = (uint32_t)(timestamp % 86400UL);
    uint32_t date = (uint32_t)(timestamp / 86400UL);
    uint16_t year = 1970;

    datetime->sec  = (time % 60);
    datetime->min  = ((time % 3600) / 60);
    datetime->hour = (time / 3600);

    datetime->day_of_week = (date + 4) % 7;

    while(date >= (uint32_t) year_day_count(year)) {
        date -= year_day_count(year++);
    }

    datetime->year = year - 1900;
    datetime->day_of_year = date;
    datetime->month = 0;

    while(date >= (uint32_t) month_day_count(year, datetime->month)) {
        date -= month_day_count(year, datetime->month++);
    }

    datetime->day_of_month = date + 1;
    datetime->daylight_saving = false;
}

/**
 * @function:   timestamp_t
 * @param:      struct datetime_t *, structual date time representation.
 * @return:     timestamp_t, unix timestamp representation of time.
 * @brief:      Encodes a structual representation into a unix timestamp.
 */
timestamp_t
time_encode(struct datetime_t* datetime)
{
    // XXX: Still to be implemented!
    return 0;
}

/**
 * @function:   time_string
 * @param:      char *, target string buffer.
 * @param:      size_t, maximum size of the resulting datetime string.
 * @param:      const char *, datetime string format.
 * @param:      struct datetime_t *, structual date time representation.
 * @return:     size_t, lenght of the resulting string.
 * @brief:      Formats a datetime point into a human readable string.
 */
size_t
time_string(char* buffer, size_t max_length, const char* format, const struct datetime_t* datetime)
{
    // XXX: Still to be implemented!
    return 0;
}
