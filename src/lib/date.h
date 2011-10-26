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

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

#include <avr/pgmspace.h>

#include "clock.h"

#ifndef _DATE_H_
#define _DATE_H_

/**
 * @type:       timestamp_t
 * @brief:      Time representation inheritance
 *              of the realtime clock.
 */
typedef clock_timestamp_t timestamp_t;

/**
 * @struct:     datetime_t
 * @brief:      Structural human representation of a
 *              date and time point.
 */
struct datetime_t {
    uint8_t  sec;
    uint8_t  min;
    uint8_t	 hour;

    uint8_t  day_of_week;
    uint8_t  day_of_month;
    uint16_t day_of_year;

    uint8_t  month;
    uint16_t year;

    bool daylight_saving;
};

/**
 * @function:   year_is_leap
 * @param:      uint16_t, year.
 * @return:     bool, true if year is leap.
 * @brief:      Determins if the given year is leap.
 */
extern bool year_is_leap(uint16_t year);

/**
 * @function:   year_day_count
 * @param:      uint16_t, year.
 * @return:     uint16_t, amount of days the year has.
 * @brief:      Returns the amount of days in given year.
 */
extern uint16_t year_day_count(uint16_t year);

/**
 * @function:   month_day_count
 * @param:      uint16_t, year.
 * @param:      uint8_t, month.
 * @return:     uint8_t, amount of days in the given month of the year.
 * @brief:      Returns the amount of day in the given month and year.
 */
extern uint8_t month_day_count(uint16_t year, uint8_t month);

/**
 * @function:   time_decode
 * @param:      struct datetime_t *, structual date time representation.
 * @param:      timestamp_t, unix timestamp representation of time.
 * @brief:      Parses a unix timestamp into a structual representation.
 */
extern void time_decode(struct datetime_t* datetime, timestamp_t timestamp);

/**
 * @function:   timestamp_t
 * @param:      struct datetime_t *, structual date time representation.
 * @return:     timestamp_t, unix timestamp representation of time.
 * @brief:      Encodes a structual representation into a unix timestamp.
 */
extern timestamp_t time_encode(struct datetime_t* datetime);

/**
 * @function:   time_string
 * @param:      char *, target string buffer.
 * @param:      size_t, maximum size of the resulting datetime string.
 * @param:      const char *, datetime string format.
 * @param:      struct datetime_t *, structual date time representation.
 * @return:     size_t, lenght of the resulting string.
 * @brief:      Formats a datetime point into a human readable string.
 */
extern size_t time_string(char* buffer, size_t max_length, const char* format, const struct datetime_t* datetime);

/* !_DATE_H_ */
#endif
