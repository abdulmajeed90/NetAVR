/**
 * Copyright 2011 Roy van Dam <roy@8bit.cx>
 * Copyright 2011 Ramon Kleiss <ramonkleiss@gmail.com>
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

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>

#include <avr/pgmspace.h>

#include "mac.h"
#include "ip.h"
#include "util.h"

#ifndef _ICMP_H_
#define _ICMP_H_

// ICMP Contants
#define	ICMP_TYPE_ECHO_REPLY		0
#define ICMP_CODE_ECHO_REPLY		0

#define	ICMP_TYPE_ECHO_REQUEST		8
#define ICMP_CODE_ECHO_REQUEST		0

#define ICMP_DEFAULT_HEADER_LENGTH	8

/**
 * @struct:		icmp_header_t
 * @brief:      ICMP network packet header
 */
struct icmp_header_t {
    struct ip_header_t ip;		//< IP header structure

    uint8_t		type;			//< The type of the message
    uint8_t		code;			//< The code of the message

    uint16_t	checksum;		//< The header checksum
    uint16_t	id;				//< The message identification no.
    uint16_t	sequence;		//< The sequence no. of the message sent
    uint8_t		padding[56];	//< Padding to ensure sending of message
} __attribute__((__packed__));

/**
 * @function:   icmp_decode
 * @param:      uint16_t, Length of the packet received.
 * @param:      uint8_t *, Pointer to the first byte of the packet.
 * @return:     uint16_t, Size of the new reply packet to be transmitted.
 * @brief:      Decodes a received packet into a ICMP-packet and
 *              runs nessesary actions. Which eventually will create
 *              an appropriate reply.
 */
extern uint16_t	icmp_decode(uint16_t length, uint8_t* packet);

/**
 * @function:   icmp_checksum
 * @param:      uint16_t, Length of the ICMP-header.
 * @param:      uint8_t *, Pointer to the first byte of the header.
 * @brief:      Calculates the checksum over an ICMP-header, the length of the header.
 *              is 8 most of the times because a header without any data has a size of 8
 *              bytes.
 */
extern uint16_t	icmp_checksum(uint16_t length, uint8_t* packet);

/**
 * @function:   icmp_echo_reply
 * @param:      uint16_t, Length of the packet received.
 * @param:      uint8_t *, Pointer to the first byte of the packet.
 * @return:     uint16_t, Size of the new reply packet to be transmitted.
 * @brief:      Creates a message that responds to a echo request message from an random host.
 */
extern uint16_t icmp_echo_reply(uint16_t length, uint8_t* packet);

/**
 * @function:   icmp_print_header
 * @param:      struct icmp_header_t *, ICMP packet header.
 * @brief:      Prints header content to stdout.
 */
#ifdef WITH_DEBUG
extern void icmp_print_header(struct icmp_header_t* icmp_header);
#endif

/* !_ICMP_H_ */
#endif
