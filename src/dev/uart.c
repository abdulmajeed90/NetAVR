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

#include "uart.h"

/**
 * @function:   uart_init
 * @param:      Transmission baudrate
 * @param:      enable receiver
 * @param:      enable transmitter
 * @brief:      Initializes UART interface
 */
void
uart_init(uint32_t baudrate, bool rx, bool tx)
{
    cli();

    // Calculate baudrate register value
    uint16_t rate = (F_CPU / (uint32_t)(8 * baudrate)) - 1;

    // Set doublespeed mode
    UCSRA |= (1 << U2X);

    // Set baudrate register
    UBRRH = (uint8_t)(rate >> 8);
    UBRRL = (uint8_t)(rate);

    if(tx) {  // Enable transmiter
        UCSRB |= (1 << TXEN);
    }

    if(rx) {  // Enable receiver
        UCSRB |= (1 << RXEN);
    }

    sei();
}

/**
 * @function:   uart_write
 * @param:      Amount of data to be written.
 * @param:      Local data buffer to be read from.
 * @return:     Amount of data actually written.
 * @brief:      Writes a chunk of data over the UART
 *              interface.
 */
size_t
uart_write(size_t length, const uint8_t* data)
{
    size_t i = 0;

    for(; i < length; i++) {
        uart_write_byte(data[i]);
    }

    return length - i;
}

/**
 * @function:   uart_read
 * @param:      Maximum length of data to be read.
 * @param:      Local data buffer to be written to.
 * @return:     Amount of data actually read.
 * @brief:      Reads a chunk of data from the UART
 *              interface.
 */
size_t
uart_read(size_t max_length, uint8_t* data)
{
    size_t i = 0;

    for(; i < max_length; i++) {
        if((data[i] = uart_read_byte()) == '\0') {
            break;
        }
    }

    if(data[i] != '\0') {
        data[i] = '\0';
    }

    return i;
}

/**
 * @function:   uart_write_byte
 * @param:      Byte to be send.
 * @brief:      Sends a single byte over the UART
 *              interface.
 */
void
uart_write_byte(uint8_t byte)
{
    while(!(UCSRA & (1 << UDRE))) {
        continue;
    }

    UDR = byte;
}

/**
 * @function:   uart_read_byte
 * @return:     Byte read.
 * @brief:      Reads a single byte from the UART
 *              interface.
 */
uint8_t
uart_read_byte(void)
{
    while(!(UCSRA & (1 << RXC))) {
        continue;
    }

    return UDR;
}
