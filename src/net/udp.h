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

#include <avr/pgmspace.h>

#include "mac.h"
#include "ip.h"
#include "util.h"

#ifndef _UDP_H_
#define _UDP_H_

/**
 * @define:     UDP_DEFAULT_HEADER
 * @brief:      The header length of an UDP header
 */
#define UDP_DEFAULT_HEADER_LENGTH 8

/**
 * @define:     UDP_MAX_BINDINGS
 * @brief:      The maximum number of UDP port bindings
 */
#define UDP_MAX_BINDINGS 10

/**
 * @struct:		udp_header_t
 * @brief:      UDP network packet header
 */
struct udp_header_t {
    struct ip_header_t ip;

    uint16_t src_port;
    uint16_t dest_port;

    uint16_t length;
    uint16_t checksum;
} __attribute__((__packed__));

/**
 * @type:       udp_inbound_t
 * @brief:      Callback type for UDP bindings.
 *              These functions are called when
 *              there is inbound data on the bound port.
 */
typedef uint16_t (*udp_inbound_t)(struct udp_header_t* udp_header, uint8_t* data);

/**
 * @struct:     udp_bind_t
 * @brief:      UDP port binding structure. Holds the
 *              inbound callback for a specific port.
 */
struct udp_bind_t {
    uint16_t port;
    udp_inbound_t callback;
};

/**
 * @function:   udp_checksum
 * @param:      The length of the header including data
 * @param:      Pointer to the first byte of the data
 * @param:      IP source address
 * @param:      IP destination address
 * @return:     The two-byte checksum of the UDP packet
 * @brief:      Calculates the checksum of an UDP packet
 */
extern uint16_t udp_checksum(uint16_t length, uint8_t* packet, const ip_addr_t src_addr, const ip_addr_t dest_addr);

/**
 * @function:   udp_decode
 * @param:      The length of the received packet
 * @param:      Pointer to the first byte of the received data
 * @return:     The length of an (optional) message to be transmitted
 * @brief:      This functions decodes an incoming UDP packet and checks if the port has been bound.
 *              If so, the function attached will be called to process the received data within the UDP packet.
 */
extern uint16_t udp_decode(uint16_t length, uint8_t* packet);

/**
 * @function:   udp_bind
 * @param:      Port to bind.
 * @param:      Function to call when there's a packet received at the specified port
 * @return:     True if the port has been succesfully bound.
 * @brief:      Binds a port with the specified callback function.
 */
extern bool udp_bind(uint16_t port, udp_inbound_t callback);

/**
 * @function:   udp_unbind
 * @param:      The port number to unbind
 * @result:     True if port has been succesfully unbound
 * @brief:      Removes binding with its callback function
 */
extern bool udp_unbind(uint16_t port);

/**
 * @function:   udp_print_header
 * @param:      UDP packet header.
 * @brief:      Prints header content to stdout.
 */
#ifdef WITH_DEBUG
extern void udp_print_header(struct udp_header_t* udp_header);
#endif

/* !_UDP_H */
#endif
