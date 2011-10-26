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
#include <string.h>
#include <stdio.h>

#include <avr/pgmspace.h>

#include "util.h"

#ifndef _MAC_H_
#define _MAC_H_

// MAC packet type numbers
#define MAC_TYPE_ARP 0x0806
#define MAC_TYPE_IP4 0x0800
#define MAC_TYPE_IP6 0x86dd

#define MAC_DEFAULT_HEADER_LENGTH 14

/**
 * @type: six byte mac address
 *        representation.
 */
typedef uint8_t mac_addr_t[6];

/**
 * @var:        static const mac_addr_t
 * @brief:      constant mac broadcast address.
 */
static const mac_addr_t mac_broadcast_addr = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

/**
 * @struct:		mac_header_t
 * @brief:      MAC network packet header
 */
struct mac_header_t {
    mac_addr_t dest_addr;
    mac_addr_t src_addr;
    uint16_t type;
} __attribute__((__packed__));

/**
 * @function:   mac_set_host_addr
 * @param:      Local hardware mac address.
 * @brief:      Sets local hardware mac address.
 *              Be aware that this does not directly
 *              write the address to the ethernet
 *              controller. It is only a software
 *              representation.
 */
extern void mac_set_host_addr(mac_addr_t mac_addr);

/**
 * @function:   mac_get_host_addr
 * @return:     Local hardware mac address.
 * @brief:      Returns local hardware mac address.
 */
extern uint8_t* mac_get_host_addr(void);

/**
 * @function:   mac_print_header
 * @param:      MAC packet header.
 * @brief:      Prints header content to stdout.
 */
#ifdef WITH_DEBUG
extern void mac_print_header(struct mac_header_t* mac_header);
#endif

/* !_MAC_H_ */
#endif
