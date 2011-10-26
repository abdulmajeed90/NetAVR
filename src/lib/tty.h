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
#include <stdio.h>
#include <stdbool.h>

#include "dev/uart.h"

#ifndef _TTY_H_
#define _TTY_H_

/**
 * @function:   tty_init
 * @param:      Transmission baudrate
 * @brief:      Initializes TTY layer
 */
extern void tty_init (uint32_t baudrate);

/**
 * @function:   tty_write
 * @param:      Character to be writen
 * @param:      Stream receiving the character
 * @brief:      Sends a singe character over the
 *              TTY interface.
 */
extern int tty_write (char c, FILE * stream);

/**
 * @function:   tty_read
 * @param:      Character to be writen
 * @param:      Standart in stream
 * @return:     0 - data has been send
 * @brief:      Sends a singe character over the
 *              TTY interface.
 */
extern int tty_read (FILE * stream);

/* !_TTY_H_ */
#endif
