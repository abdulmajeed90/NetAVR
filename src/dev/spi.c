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

#include "spi.h"

/**
 * @function:   spi_init
 * @brief:      Used to initialise SPI interface as master
 */
void
spi_init(void)
{
    // Already initialized?
    if(SPCR & (1 << SPE)) {
        return;
    }

    // SPI port mapping
    #if defined(__AVR_ATmega128__)
        DDRB |= (1 << DDB2);	// MOSI
        DDRB &= ~(1 << DDB3);	// MISO
        DDRB |= (1 << DDB1);	// SCK
    #elif defined(__AVR_ATmega16__) || defined (__AVR_ATmega164__) || defined (__AVR_ATmega164A__) || \
          defined(__AVR_ATmega32__) || defined (__AVR_ATmega324__) || defined (__AVR_ATmega324A__) || \
          defined(__AVR_ATmega64__) || defined (__AVR_ATmega644__) || defined (__AVR_ATmega644A__)
        DDRB |= (1 << DDB5);	// MOSI
        DDRB &= ~(1 << DDB6);	// MISO
        DDRB |= (1 << DDB7);	// SCK
    #elif defined(__AVR_ATmega8__)
        DDRB |= (1 << DDB3);	// MOSI
        DDRB &= ~(1 << DDB4);	// MISO
        DDRB |= (1 << DDB5);	// SCK
    #else
        #error *** Check IO settings in spi.c *** 
    #endif

    // Configure the SPI interface as master
    SPCR  = (1 << SPE) | (1 << MSTR);
    SPSR |= (1 << SPI2X);
}

/**
 * @function:   spi_wait
 * @brief:      Wait for the spi interface to become ready.
 */
inline void
spi_wait(void)
{
    // XXX: Not implemented
}

/**
 * @function:   spi_write_byte
 * @brief:      Writes a byte over the SPI interface
 */
void
spi_write_byte(uint8_t data)
{
    SPDR = data;

    while(!(SPSR & (1 << SPIF)));
}

/**
 * @function:   spi_read_byte
 * @brief:      Reads a byte from the SPI interface
 */
uint8_t
spi_read_byte(void)
{
    SPDR = 0x00;
    while(!(SPSR & (1 << SPIF)));
    return SPDR;
}

/**
 * @function:   spi_write_word
 * @brief:      Writes a word over the SPI interface
 */
void
spi_write_word(uint16_t data)
{
    spi_write_byte(data << 8);
    spi_write_byte(data);
}

/**
 * @function:   spi_read_word
 * @brief:      Reads a word from the SPI interface
 */
uint16_t
spi_read_word(void)
{
    uint16_t result = 0;

    result  = spi_read_byte() << 8;
    result |= spi_read_byte();

    return result;
}
