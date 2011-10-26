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

#include "clock.h"

// Time containers
static volatile clock_timestamp_t timestamp;
static volatile clock_microtime_t microtime;

/**
 * @function:   clock_init
 * @brief:      Initializes clock timer.
 */
void
clock_init(void)
{
    // Reset clock
    timestamp = 0;
    microtime = 0;

    // Configure hardware timer for 1 ms CTC
#if defined(__AVR_ATmega32__)
    OCR0 = 249;
    TCCR0 = (1 << WGM01) | (1 << CS00) | (1 << CS01);
    TIMSK |= (1 << OCIE0);
#else
#error *** Check timer settings in clock.c *** 
#endif

    // Enable interrupts
    sei();
}

/**
 * @function:   clock_tick
 * @brief:      Periodic clock function
 *              should be called every 1 ms.
 */
inline void
clock_tick(void)
{
    microtime++;

    if(microtime == 1000) {
        microtime = 0;
        timestamp++;
    }
}

/**
 * @function:   clock_set_time
 * @param:      New time
 * @brief:      Set time to specified
 *              value.
 */
void
clock_set_time(clock_timestamp_t time)
{
    cli();

    timestamp = time;
    microtime = 0;

    sei();
}

/**
 * @function:   clock_time
 * @return:     Current unix timestamp
 * @brief:      Gets the current time.
 */
clock_timestamp_t
clock_time(void)
{
    return timestamp;
}

/**
 * @function:   clock_microtime
 * @return:     Current unix timestamp miliseconds
 * @brief:      Gets the current time.
 */
clock_microtime_t
clock_microtime(void)
{
    return microtime;
}

/**
 * @ISR:        TIMER0_COMP_vect
 * @brief:      1 ms periodic clock interrupt
 */
ISR(TIMER0_COMP_vect)
{
    clock_tick();
}
