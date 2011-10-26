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
#include <stdlib.h>
#include <stdbool.h>

#include <avr/pgmspace.h>

#include "dev/eth.h"
#include "lib/clock.h"
#include "lib/timer.h"

#include "mac.h"
#include "arp.h"
#include "ip.h"
#include "icmp.h"
#include "udp.h"
#include "util.h"

#ifndef _NET_H_
#define _NET_H_

struct net_status_t {
    bool link;

    uint32_t packets_sent;
    uint32_t bytes_sent;

    uint32_t packets_received;
    uint32_t bytes_received;
};

/**
 * @function:   net_init
 * @param:      Ethernet controller hardware mac address
 * @param:      Network IP address
 * @param:      Network address subnetmask
 * @param:      Network's default router IP address
 * @brief:      Initialises the network stack
 */
extern void net_init(mac_addr_t mac_address, ip_addr_t ip, ip_mask_t netmask, ip_addr_t default_router);

/**
 * @function:   net_get_status
 * @return:     Interface status and statistics
 * @brief:      Returns current interface status
 *              and statistics. Containing link
 *              status and transmission statistics.
 */
extern const struct net_status_t* net_get_status(void);

/**
 * @function:   net_periodic
 * @brief:      Run this function periodicly to handle
 *              incomming network traffic. Be aware that
 *              this might take some time to complete.
 *              Therefor it is not wise to run this from
 *              an interrupt routine, forexample that of
 *              a hardware timer.
 */
extern void net_periodic(void);

/**
 * @function:   net_decode
 * @param:      The packet length
 * @param:      Pointer to the first byte of the packat
 * @return:     The length of a possible new packet
 */
extern uint16_t	net_decode(uint16_t length, uint8_t* packet);

/**
 * @function:   net_debug
 * @param:      The packet length
 * @param:      Pointer to the first byte of the packat
 * @brief:      Printing the packet content in human readable
 *              format over stdout.
 */
#ifdef WITH_DEBUG
extern void net_debug(uint16_t count, uint16_t length, uint8_t* packet);
#endif

/* !_NET_H */
#endif
