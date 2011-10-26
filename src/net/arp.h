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

#include "mac.h"
#include "tcp.h"
#include "ip.h"
#include "queue.h"

#ifndef _ARP_H_
#define _ARP_H_

// ARP Settings
#define ARP_TABLE_SIZE    10
#define ARP_ENTRY_MAX_AGE 120
#define ARP_HARDWARE_TYPE 1

// ARP Opcodes
#define ARP_OPCODE_REQUEST 1
#define ARP_OPCODE_REPLY   2

/**
 * @struct:		arp_header_t
 * @brief:      ARP network packet header
 */
struct arp_header_t
{
	struct mac_header_t mac;
	
	uint16_t hardware_type;
	uint16_t protocol_type;
	
	uint8_t hardware_length;
	uint8_t protocol_length;
	
	uint16_t opcode;

	mac_addr_t mac_src_addr;
	ip_addr_t ip_src_addr;
	
	mac_addr_t mac_dest_addr;
	ip_addr_t ip_dest_addr;
} __attribute__((__packed__));

/**
 * @struct:     arp_entry_t
 * @brief:      Represents an ARP table entry.
 */
struct arp_entry_t
{
	uint8_t time;
	mac_addr_t mac_addr;
	ip_addr_t ip_addr;
};

/**
 * @function:   arp_init
 * @brief:      Used to initialise the ARP functions
 */
extern void arp_init (void);

/**
 * @function:   arp_update
 * @param:      ip_addr_t, IP address of the remote node.
 * @param:      mac_addr_t, MAC address of the remote node.
 * @brief:      Will update or create ARP table entry for
 *              the given IP and MAC address.
 */
extern void arp_update (ip_addr_t ip_addr, mac_addr_t mac_addr);

/**
 * @function:   arp_flush
 * @return:     bool, always true to restart the timer callback.
 * @brief:      Used to periodically flush expired ARP table
 *              entries. Sould be called every ~10 seconds.
 */
extern bool arp_flush (void);

/**
 * @function:   arp_decode
 * @param:      uint16_t, Length of the packet received.
 * @param:      uint8_t *, Pointer to the first byte of the packet.
 * @return:     uint16_t, Size of the new reply packet to be transmitted.
 * @brief:      Decodes a received packet into a ARP-packet and
 *              runs nessesary actions. Which eventually will create
 *              an appropriate reply.
 */
extern uint16_t arp_decode (uint16_t length, uint8_t * packet);

/**
 * @function:   arp_encode
 * @param:      uint16_t, Length of the packet.
 * @param:      uint8_t *, Pointer to the first byte of the packet.
 * @return:     uint16_t, Size of the packet to be transmitted.
 * @brief:      Populates an IP packet with ARP address data. In case
 *              the node can not be found in the ARP table a new ARP
 *              request packet is created, or if the destination is
 *              outside the local networkmask the packet is forwared
 *              to the defaut router.
 */
extern uint16_t arp_encode (uint16_t length, uint8_t * packet);

/**
 * @function:   arp_print_header
 * @param:      struct arp_header_t *, ARP packet header.
 * @brief:      Prints header content to stdout.
 */
#ifdef WITH_DEBUG
extern void arp_print_header (struct arp_header_t * arp_header);
#endif

/* !_ARP_H */
#endif
