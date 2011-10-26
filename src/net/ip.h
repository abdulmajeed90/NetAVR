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

#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <avr/pgmspace.h>

#include "mac.h"
#include "util.h"

#ifndef _IP_H_
#define _IP_H_

// IP Protocol Contants
#define IP_PROTOCOL_ICMP  1
#define IP_PROTOCOL_IGMP  2
#define IP_PROTOCOL_TCP   6
#define IP_PROTOCOL_UDP   17
#define IP_PROTOCOL_OSPF  89
#define IP_PROTOCOL_SCTP  132

#define IP_DEFAULT_HEADER_LENGTH 20

/**
 * @type: four byte ip address
 *        representation
 */
typedef uint8_t ip_addr_t[4];

/**
 * @type: four byte netmask
 *        representation
 */
typedef uint8_t ip_mask_t[4];

/**
 * @var:        static const ip_addr_t
 * @brief:      Constant IP broadcast address.
 */
static const ip_addr_t ip_broadcast_addr = {0xFF, 0xFF, 0xFF, 0xFF};

/**
 * A structure to represent the IP-header,
 * the MAC header is also included into the
 * structure.
 */
struct ip_header_t {
    struct mac_header_t mac;

    uint8_t version;
    uint8_t services;
    uint16_t length;
    uint16_t id;
    uint16_t offset;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;

    ip_addr_t src_addr;
    ip_addr_t dest_addr;
} __attribute__((__packed__));

/**
 * @function:   ip_checksum
 * @param:      The length of the header including the data
 * @param:      Pointer to the first byte of data
 * @return:     The checksum of the IP-header
 * @brief:      Calculates 16 bit CRC checksum over the IP header.
 */
extern uint16_t ip_checksum(uint16_t length, uint8_t* packet);

/**
 * @function:   ip_decode
 * @param:      The length of the received data in bytes
 * @param:      Pointer to the first byte data
 * @return:     Pointer to the IP-header representation of the byte-array
 * @brief:      This function converts an block of data into a ip_header_t type.
 *              Using this, you can manipulate the data that is sended in an easy
 *              way without knowing exactly which bytes you have to manipulate.
 */
extern struct ip_header_t* ip_decode(uint16_t length, uint8_t* packet);

/**
 * @function:   ip_addr_is_empty
 * @param:      Network IP address
 * @return:     True when address is empty
 * @brief:      Checks if the given IP address has only empty fields.
 */
extern bool ip_addr_is_empty(const ip_addr_t ip_addr);

/**
 * @function:   ip_addr_compare
 * @param:      Network IP address A
 * @param:      Network IP address B
 * @return:     True when both addresses are the same.
 * @brief:      Compares two IP addresses.
 */
extern bool ip_addr_compare(const ip_addr_t ip_addr_a, const ip_addr_t ip_addr_b);

/**
 * @function:   ip_addr_compare
 * @param:      Network IP address A
 * @param:      Network IP address B
 * @param:      Network address subnetmask
 * @return:     True when both addresses are the same.
 * @brief:      Compares two IP addresses with netmasks.
 *              The mask is used to mask out the bits that are to be compared.
 */
extern bool ip_mask_compare(const ip_addr_t ip_addr_a, const ip_addr_t ip_addr_b, const ip_mask_t netmask);

/**
 * @function:   ip_set_host_addr
 * @param:      Network IP address
 * @brief:      Sets local network IP address
 */
extern void ip_set_host_addr(ip_addr_t ip_addr);

/**
 * @function:   ip_get_host_addr
 * @return:     Network IP address
 * @brief:      Returns local network IP address
 */
extern uint8_t* ip_get_host_addr(void);

/**
 * @function:   ip_set_netmask
 * @param:      Network address subnetmask
 * @brief:      Sets local network address subnetmask
 */
extern void ip_set_netmask(ip_mask_t netmask);

/**
 * @function:   ip_get_netmask
 * @return:     Network address subnetmask
 * @brief:      Returns local network address subnetmask
 */
extern uint8_t* ip_get_netmask(void);

/**
 * @function:   ip_set_default_router
 * @param:      Network default router address
 * @brief:      Sets local network default router address
 */
extern void ip_set_default_router(ip_addr_t ip_addr);

/**
 * @function:   ip_get_default_router
 * @return:     Network default router address
 * @brief:      Returns local network default router address
 */
extern uint8_t* ip_get_default_router(void);

/**
 * @function:   ip_print_header
 * @param:      IP packet header.
 * @brief:      Prints header content to stdout.
 */
#ifdef WITH_DEBUG
extern void ip_print_header(struct ip_header_t* ip_header);
#endif

/* !_IP_H */
#endif
