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

#ifndef _CTRL_H_
#define _CTRL_H_

// Control macro
#define CTRL(x) (x - 'A' + 1)

// Control chars
#define SOH  'A' // Start of heading
#define STX  'B' // Start of text
#define ETX  'C' // End of text
#define ETO  'D' // End of transmission
#define ENQ  'E' // Enquery
#define ACK  'F' // Acknowledge
#define BEL  'G' // Bell
#define BS   'H' // Back space
#define HT   'I' // Horizontal tabulation
#define LF   'J' // Line feed
#define VT   'K' // Vertical tabulation
#define FF   'L' // Form feed
#define CR   'M' // Carriage return
#define SO   'N' // Shift out
#define SI   'O' // Shift in
#define DLE  'P' // Data link escape
#define DC1  'Q' // Device control 1
#define DC2  'R' // Device control 2
#define DC3  'S' // Device control 3
#define DC4  'T' // Device control 4
#define NAK  'U' // Negative acknowledge
#define SYN  'V' // Synchronous idle
#define ETB  'W' // End of transmission block
#define CAN  'X' // Cancel
#define EM   'Y' // End of medium
#define SUB  'Z' // Substitute
#define ESC  '[' // Escape

/* !_CTRL_H_ */
#endif
