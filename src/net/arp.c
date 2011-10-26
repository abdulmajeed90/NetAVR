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

#include "arp.h"

/**
 * @var:        static uint8_t
 * @brief:      Elapsed runtime.
 */
static uint8_t arp_time;

/**
 * @var:        static struct arp_entry_t array
 * @brief:      ARP table containing the ARP entries.
 */
static struct arp_entry_t arp_table [ARP_TABLE_SIZE];

/**
 * @function:   arp_init
 * @brief:      Used to initialise the ARP functions
 */
void
arp_init(void)
{
    uint8_t i = 0;

    for(; i < ARP_TABLE_SIZE; i++) {
        memset(arp_table[i].ip_addr, 0, 4);
    }
}

/**
 * @function:   arp_update
 * @param:      ip_addr_t, IP address of the remote node.
 * @param:      mac_addr_t, MAC address of the remote node.
 * @brief:      Will update or create ARP table entry for
 *              the given IP and MAC address.
 */
void
arp_update(ip_addr_t ip_addr, mac_addr_t mac_addr)
{
    register struct arp_entry_t* entry;
    uint8_t i;

    // Walk through the ARP mapping table and try to find an entry to
    // update. If none is found, the IP -> MAC address mapping is
    // inserted in the ARP table.
    for(i = 0; i < ARP_TABLE_SIZE; i++) {
        entry = &arp_table[i];

        // Only check those entries that are actually in use.
        if(ip_addr_is_empty(entry->ip_addr)) {
            continue;
        }

        // Check if the source IP address of the incoming packet matches
        // the IP address in this ARP table entry.
        if(ip_addr_compare(entry->ip_addr, ip_addr)) {
            // An old entry found, update this and return.
            memcpy(entry->mac_addr, mac_addr, 6);
            entry->time = arp_time;

            return;
        }
    }

    // If we get here, no existing ARP table entry was found, so we	create one.
    // First, we try to find an unused entry in the ARP table.
    for(i = 0; i < ARP_TABLE_SIZE; i++) {
        entry = &arp_table[i];

        if(ip_addr_is_empty(entry->ip_addr)) {
            break;
        }
    }

    // If no unused entry is found, we try to find the oldest entry and	throw it away.
    if(i == ARP_TABLE_SIZE) {
        uint8_t max_aged_time = 0;
        uint8_t max_aged_entry = 0;

        for(i = 0; i < ARP_TABLE_SIZE; i++) {
            entry = &arp_table[i];

            if((arp_time - entry->time) > max_aged_time) {
                max_aged_time = (arp_time - entry->time);
                max_aged_entry = i;
            }
        }

        entry = &arp_table[max_aged_entry];
    }

    // Fill the ARP table entry with the new information.
    memcpy(entry->ip_addr, ip_addr, 4);
    memcpy(entry->mac_addr, mac_addr, 6);
    entry->time = arp_time;
}

/**
 * @function:   arp_flush
 * @return:     bool, always true to restart the timer callback.
 * @brief:      Used to periodically flush expired ARP table
 *              entries. Sould be called every ~10 seconds.
 */
bool
arp_flush(void)
{
    struct arp_entry_t* entry;
    uint8_t i = 0;

    // Increment ARP time
    arp_time++;

    // Iterate trough ARP table
    for(; i < ARP_TABLE_SIZE; i++) {
        entry = &arp_table[i];

        // Remove address when outdated
        if(!ip_addr_is_empty(entry->ip_addr) && (arp_time - entry->time >= ARP_ENTRY_MAX_AGE)) {
            memset(entry->ip_addr, 0, 4);
        }
    }

    // Yeah we do want to run this again ;)
    return true;
}

/**
 * @function:   arp_decode
 * @param:      uint16_t, Length of the packet received.
 * @param:      uint8_t *, Pointer to the first byte of the packet.
 * @return:     uint16_t, Size of the new reply packet to be transmitted.
 * @brief:      Decodes a received packet into a ARP-packet and
 *              runs nessesary actions. Which eventually will create
 *              an appropriate reply.
 */
uint16_t
arp_decode(uint16_t length, uint8_t* packet)
{
    // Packet is valid ARP packet
    if(length < sizeof(struct arp_header_t)) {
        return 0;
    }

    // Assign ARP header pointer
    struct arp_header_t* arp_header = ((struct arp_header_t*) &packet[0]);

    // Select ARP operation
    switch(htons(arp_header->opcode)) {
        case ARP_OPCODE_REQUEST:

            // If it asked for our address, we send out a reply.
            if(!ip_addr_compare(arp_header->ip_dest_addr, ip_get_host_addr())) {
                break;
            }

            // First, we register the one who made the request in our ARP
            // table, since it is likely that we will do more communication
            // with this host in the future.
            arp_update(arp_header->ip_src_addr, arp_header->mac_src_addr);

            // Set new destination and source mac address
            memcpy(arp_header->mac_dest_addr, arp_header->mac_src_addr, 6);
            memcpy(arp_header->mac.dest_addr, arp_header->mac_src_addr, 6);
            memcpy(arp_header->mac_src_addr, mac_get_host_addr(), 6);
            memcpy(arp_header->mac.src_addr, mac_get_host_addr(), 6);

            // Set new destination and source ip address
            memcpy(arp_header->ip_dest_addr, arp_header->ip_src_addr, 4);
            memcpy(arp_header->ip_src_addr, ip_get_host_addr(), 4);

            // Set the opcode to reply
            arp_header->opcode = htons((uint16_t)ARP_OPCODE_REPLY);

            // return packet size
            return sizeof(struct arp_header_t);
            break;

        case ARP_OPCODE_REPLY:

            // We insert or update the ARP table if it was meant for us.
            if(ip_addr_compare(arp_header->ip_dest_addr, ip_get_host_addr())) {
                arp_update(arp_header->ip_src_addr, arp_header->mac_src_addr);
            }

            // XXX: Unqueue packets for received destination.
            //      See the queue_get_table and unqueue_packet funcions
            break;
    }

    return 0;
}

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
uint16_t
arp_encode(uint16_t length, uint8_t* packet)
{
    struct arp_entry_t* entry;
    ip_addr_t dest_ip_addr;
    uint8_t i;

    // Create IP header structure
    struct ip_header_t* ip_header = (struct ip_header_t*)(packet);

    // Find the destination IP address in the ARP table and construct
    // the Ethernet header. If the destination IP addres isn't on the
    // local network, we use the default router's IP address instead.

    // If not ARP table entry is found, we overwrite the original IP
    // packet with an ARP request for the IP address.

    // First check if destination is a local broadcast.
    if(ip_addr_compare(ip_header->dest_addr, ip_broadcast_addr)) {
        // Set destination mac address
        memcpy(ip_header->mac.dest_addr, mac_broadcast_addr, 6);
    } else {
        // Check if the destination address is on the local network.
        // If the destination address was not on the local network,
        // we need to use the default router's IP address instead of
        // the destination address when determining the MAC address.
        if(!ip_mask_compare(ip_header->dest_addr, ip_get_host_addr(), ip_get_netmask())) {
            memcpy(dest_ip_addr, ip_get_default_router(), 4);
        } else {
            memcpy(dest_ip_addr, ip_header->dest_addr, 4);
        }

        // Lookup destination address
        for(i = 0; i < ARP_TABLE_SIZE; i++) {
            entry = &arp_table[i];

            if(ip_addr_compare(dest_ip_addr, entry->ip_addr)) {
                break;
            }
        }

        // If the destination address was not in our ARP table
        // we queue the packet and overwrite the packet with an
        // ARP request.
        if(i == ARP_TABLE_SIZE) {
            // Queue packet for later transmission
            queue_packet(length, packet);

            // Assign ARP header pointer
            struct arp_header_t* arp_header = ((struct arp_header_t*) &packet[0]);

            // Set destination and source mac address
            memset(arp_header->mac.dest_addr, 0xFF, 6);
            memset(arp_header->mac_dest_addr, 0x00, 6);
            memcpy(arp_header->mac.src_addr, mac_get_host_addr(), 6);
            memcpy(arp_header->mac_src_addr, mac_get_host_addr(), 6);

            // Set destination and source ip address
            memcpy(arp_header->ip_dest_addr, dest_ip_addr, 4);
            memcpy(arp_header->ip_src_addr, ip_get_host_addr(), 4);

            // Set the opcode to reply
            arp_header->opcode = htons((uint16_t) ARP_OPCODE_REQUEST);

            // Set hardware type
            arp_header->hardware_type = htons((uint16_t) ARP_HARDWARE_TYPE);
            arp_header->hardware_length = 6;

            // Set packet protocol
            arp_header->protocol_type = htons((uint16_t) MAC_TYPE_IP4);
            arp_header->protocol_length = 4;

            // Set packet type to ARP
            arp_header->mac.type = htons((uint16_t) MAC_TYPE_ARP);

            // Return packet size
            return sizeof(struct arp_header_t);
        }

        // Set destination mac address
        memcpy(ip_header->mac.dest_addr, entry->mac_addr, 6);
    }

    // Set source mac address
    memcpy(ip_header->mac.src_addr, mac_get_host_addr(), 6);

    // Set packet type to IP
    ip_header->mac.type = htons((uint16_t) MAC_TYPE_IP4);

    // Return packet size
    return (length);
}

/**
 * @function:   arp_print_header
 * @param:      struct arp_header_t *, ARP packet header.
 * @brief:      Prints header content to stdout.
 */
#ifdef WITH_DEBUG
void
arp_print_header(struct arp_header_t* arp_header)
{
    printf_P(PSTR("ARP header\n"));

    printf_P(PSTR(" Source: %02X:%02X:%02X:%02X:%02X:%02X -> %u.%u.%u.%u\n"),
             arp_header->mac_src_addr[0], arp_header->mac_src_addr[1], arp_header->mac_src_addr[2],
             arp_header->mac_src_addr[3], arp_header->mac_src_addr[4], arp_header->mac_src_addr[5],
             arp_header->ip_src_addr[0], arp_header->ip_src_addr[1], arp_header->ip_src_addr[2], arp_header->ip_src_addr[3]);

    printf_P(PSTR(" Destination: %02X:%02X:%02X:%02X:%02X:%02X -> %u.%u.%u.%u\n\n"),
             arp_header->mac_dest_addr[0], arp_header->mac_dest_addr[1], arp_header->mac_dest_addr[2],
             arp_header->mac_dest_addr[3], arp_header->mac_dest_addr[4], arp_header->mac_dest_addr[5],
             arp_header->ip_dest_addr[0], arp_header->ip_dest_addr[1], arp_header->ip_dest_addr[2], arp_header->ip_dest_addr[3]);

    printf_P(PSTR(" Hardware type: %u\n"), htons(arp_header->hardware_type));
    printf_P(PSTR(" Hardware length: %u\n"), arp_header->hardware_length);
    printf_P(PSTR(" Protocol type: 0x%04X\n"), htons(arp_header->protocol_type));
    printf_P(PSTR(" Protocol length: %u\n"), arp_header->protocol_length);

    switch(htons(arp_header->opcode)) {
        case ARP_OPCODE_REQUEST:
            printf_P(PSTR(" Operation: Request\n"));
            break;

        case ARP_OPCODE_REPLY:
            printf_P(PSTR(" Operation: Reply\n"));
            break;

        default:
            printf_P(PSTR(" Operation: Invalid [%u]\n"), htons(arp_header->opcode));
            break;
    }

    printf_P(PSTR("\n"));
}
#endif
