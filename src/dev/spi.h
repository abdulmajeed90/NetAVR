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

#ifndef _SPI_H_
#define _SPI_H_

/**
 * @function:   spi_init
 * @brief:      Used to initialise SPI interface as master
 */
extern void spi_init(void);

/**
 * @function:   spi_wait
 * @brief:      Wait for the spi interface to become ready.
 */
extern void spi_wait(void);

/**
 * @function:   spi_write_byte
 * @brief:      Writes a byte over the SPI interface
 */
extern void spi_write_byte(uint8_t data);

/**
 * @function:   spi_read_byte
 * @brief:      Reads a byte from the SPI interface
 */
extern uint8_t spi_read_byte(void);

/**
 * @function:   spi_write_word
 * @brief:      Writes a word over the SPI interface
 */
extern void spi_write_word(uint16_t data);

/**
 * @function:   spi_read_word
 * @brief:      Reads a word from the SPI interface
 */
extern uint16_t spi_read_word(void);

/* !_SPI_H_ */
#endif
