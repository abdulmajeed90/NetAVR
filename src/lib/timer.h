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

#include "clock.h"

#ifndef _TIMER_H_
#define _TIMER_H_

/**
 * @type:       timer_callback_t
 * @brief:      Timer event callback function.
 */
typedef bool (*timer_callback_t)(void);

/**
 * @struct:     timer_t
 * @brief:      Timer entry representation.
 */
struct timer_t
{
	clock_timestamp_t start;
	clock_timestamp_t interval;
	
	timer_callback_t callback;

	struct timer_t * prev;
	struct timer_t * next;
};

/**
 * @function:   timer_set
 * @param:      timer_callback_t, timer event callback function.
 * @param:      clock_timestamp_t, timer execution interval.
 * @brief:      Creates a new periodical timer instance for the given
 *              callback function. If the function has an existing timer.
 *              the timer will be replaced with the new interval.
 */
extern void timer_set (timer_callback_t callback, clock_timestamp_t interval);

/**
 * @function:   timer_clear
 * @param:      timer_callback_t, timer event callback function.
 * @brief:      Clears the timer instance for the given callback function.
 */
extern void timer_clear (timer_callback_t callback);

/**
 * @function:   timer_reset
 * @param:      timer_callback_t, timer event callback function.
 * @brief:      Resets the timer instance for the given callback function.
 */
extern void timer_reset (timer_callback_t callback);

/**
 * @function:   timer_restart
 * @param:      timer_callback_t, timer event callback function.
 * @brief:      Restarts the timer instance for the given callback function.
 */
extern void timer_restart (timer_callback_t callback);

/**
 * @function:   timer_get
 * @param:      timer_callback_t, timer event callback function.
 * @return:     timer_t *, timer instance if exists at all.
 * @brief:      Returns the timer instance for the given callback function.
 */
extern struct timer_t * timer_get (timer_callback_t callback);

/**
 * @function:   timer_periodic
 * @brief:      Should be called periodicly to execute the callback functions
 *              of the expired timers. Be aware that this might take some time
 *              to complete if the timer contains a large callback function.
 *              Therefor it is not wise to run this from an interrupt routine,
 *              forexample that of a hardware timer.
 */
extern void timer_periodic (void);

/* !_TIMER_H_ */
#endif
