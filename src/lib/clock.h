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

#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef _CLOCK_H_
#define _CLOCK_H_

/**
 * @type:       clock_timestamp_t
 * @brief:      Time representation in secconds.
 */
typedef uint32_t clock_timestamp_t;

/**
 * @type:       clock_timestamp_t
 * @brief:      Time representation in milisecconds.
 */
typedef uint16_t clock_microtime_t;

/**
 * Structual representation of a single time point.
 */
struct clock_time_t {
    clock_timestamp_t timestamp;
    clock_microtime_t microtime;
};

/**
 * @function:   clock_init
 * @brief:      Initializes clock timer.
 */
extern void clock_init(void);

/**
 * @function:   clock_tick
 * @brief:      Periodic clock function
 *              sould be called every 1 ms.
 */
extern void clock_tick(void);

/**
 * @function:   clock_set_time
 * @param:      New time
 * @brief:      Set time to specified
 *              value.
 */
extern void clock_set_time(clock_timestamp_t time);

/**
 * @function:   clock_time
 * @return:     Current unix timestamp
 * @brief:      Gets the current time.
 */
extern clock_timestamp_t clock_time(void);

/**
 * @function:   clock_microtime
 * @return:     Current unix timestamp miliseconds
 * @brief:      Gets the current time.
 */
extern clock_microtime_t clock_microtime(void);

/* !_CLOCK_H_ */
#endif
