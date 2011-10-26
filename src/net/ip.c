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

#include "ip.h"

/**
 * @var:        static ip_addr_t
 * @brief:      Network IP address
 */
static ip_addr_t ip_host_addr;

/**
 * @var:        static ip_addr_t
 * @brief:      Network's default router IP address
 */
static ip_addr_t ip_default_router;

/**
 * @var:        static ip_addr_t
 * @brief:      Network address subnetmask
 */
static ip_addr_t ip_netmask;

/**
 * @function:   ip_checksum
 * @param:      The length of the header including the data
 * @param:      Pointer to the first byte of data
 * @return:     The checksum of the IP-header
 * @brief:      Calculates 16 bit CRC checksum over the IP header.
 */
uint16_t
ip_checksum(uint16_t length, uint8_t* packet)
{
    uint32_t sum = 0;
    uint16_t i;

    // make 16 bit words out of every two adjacent 8 bit words in the packet
    // and add them up
    for(i = 0; i < length; i += 2) {
        sum += ((packet[i] << 8) & 0xFF00) + packet[i + 1];
    }

    // Apply padding when needed
    if(length % 2) {
        sum += ((packet[length] << 8)  & 0xFF00);
    }

    // take only 16 bits out of the 32 bit sum and add up the carries
    while(sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    // one's complement the result
    sum = ~sum;

    return (uint16_t)(sum);
}

/**
 * @function:   ip_decode
 * @param:      The length of the received data in bytes
 * @param:      Pointer to the first byte data
 * @return:     Pointer to the IP-header representation of the byte-array
 * @brief:      This function converts an block of data into a ip_header_t type.
 *              Using this, you can manipulate the data that is sended in an easy
 *              way without knowing exactly which bytes you have to manipulate.
 */
struct ip_header_t*
ip_decode(uint16_t length, uint8_t* packet) {
    // Check the length to be sure is a valid packet, if not, discard the package
    if(length < sizeof(struct ip_header_t)) {
        return NULL;
    }

    // Convert the array to an IP-header
    struct ip_header_t* ip_header = ((struct ip_header_t*) &packet[0]);

    // Return header
    return ip_header;
}

/**
 * @function:   ip_addr_is_empty
 * @param:      Network IP address
 * @return:     True when address is empty
 * @brief:      Checks if the given IP address has only empty fields.
 */
inline bool
ip_addr_is_empty(const ip_addr_t ip_addr)
{
    return (!(ip_addr[0] | ip_addr[1] | ip_addr[2] | ip_addr[3]));
}

/**
 * @function:   ip_addr_compare
 * @param:      Network IP address A
 * @param:      Network IP address B
 * @return:     True when both addresses are the same.
 * @brief:      Compares two IP addresses.
 */
inline bool
ip_addr_compare(const ip_addr_t ip_addr_a, const ip_addr_t ip_addr_b)
{
    return ((ip_addr_a[0] == ip_addr_b[0]) &&
            (ip_addr_a[1] == ip_addr_b[1]) &&
            (ip_addr_a[2] == ip_addr_b[2]) &&
            (ip_addr_a[3] == ip_addr_b[3]));
}

/**
 * @function:   ip_addr_compare
 * @param:      Network IP address A
 * @param:      Network IP address B
 * @param:      Network address subnetmask
 * @return:     True when both addresses are the same.
 * @brief:      Compares two IP addresses with netmasks.
 *              The mask is used to mask out the bits that are to be compared.
 */
inline bool
ip_mask_compare(const ip_addr_t ip_addr_a, const ip_addr_t ip_addr_b, const ip_mask_t netmask)
{
    return (((ip_addr_a[0] & netmask[0]) == (ip_addr_b[0] & netmask[0])) &&
            ((ip_addr_a[1] & netmask[1]) == (ip_addr_b[1] & netmask[1])) &&
            ((ip_addr_a[2] & netmask[2]) == (ip_addr_b[2] & netmask[2])) &&
            ((ip_addr_a[3] & netmask[3]) == (ip_addr_b[3] & netmask[3])));
}

/**
 * @function:   ip_set_host_addr
 * @param:      Network IP address
 * @brief:      Sets local network IP address
 */
inline void
ip_set_host_addr(ip_addr_t ip_addr)
{
    memcpy(ip_host_addr, ip_addr, 4);
}

/**
 * @function:   ip_get_host_addr
 * @return:     Network IP address
 * @brief:      Returns local network IP address
 */
inline uint8_t*
ip_get_host_addr(void)
{
    return ip_host_addr;
}

/**
 * @function:   ip_set_netmask
 * @param:      Network address subnetmask
 * @brief:      Sets local network address subnetmask
 */
inline void
ip_set_netmask(ip_mask_t netmask)
{
    memcpy(ip_netmask, netmask, 4);
}

/**
 * @function:   ip_get_netmask
 * @return:     Network address subnetmask
 * @brief:      Returns local network address subnetmask
 */
inline uint8_t*
ip_get_netmask(void)
{
    return ip_netmask;
}

/**
 * @function:   ip_set_default_router
 * @param:      Network default router address
 * @brief:      Sets local network default router address
 */
inline void
ip_set_default_router(ip_addr_t ip_addr)
{
    memcpy(ip_default_router, ip_addr, 4);
}

/**
 * @function:   ip_get_default_router
 * @return:     Network default router address
 * @brief:      Returns local network default router address
 */
inline uint8_t*
ip_get_default_router(void)
{
    return ip_default_router;
}

/**
 * @function:   ip_print_header
 * @param:      IP packet header.
 * @brief:      Prints header content to stdout.
 */
#ifdef WITH_DEBUG
void
ip_print_header(struct ip_header_t* ip_header)
{
    printf_P(PSTR("IP header\n"));

    printf_P(PSTR(" Source: %u.%u.%u.%u\n"), ip_header->src_addr[0], ip_header->src_addr[1], ip_header->src_addr[2], ip_header->src_addr[3]);
    printf_P(PSTR(" Destination: %u.%u.%u.%u\n\n"), ip_header->dest_addr[0], ip_header->dest_addr[1], ip_header->dest_addr[2], ip_header->dest_addr[3]);

    printf_P(PSTR(" Version: %u\n"),	(ip_header->version & 0xF0) >> 4);
    printf_P(PSTR(" Header length: %u\n"), ip_header->version & 0x0F);
    printf_P(PSTR(" Services: %u\n"), 	 ip_header->services);
    printf_P(PSTR(" Length: %u\n"),	 	htons(ip_header->length));
    printf_P(PSTR(" ID: %u\n"),	 		htons(ip_header->id));
    printf_P(PSTR(" Flag DF: %s\n"),	((htons(ip_header->offset) & 0x4000) ? "Set" : "Clear"));
    printf_P(PSTR(" Flag MF: %s\n"),	((htons(ip_header->offset) & 0x2000) ? "Set" : "Clear"));
    printf_P(PSTR(" Offset: %u\n"), 	 	htons(ip_header->offset) & 0x1FFF);
    printf_P(PSTR(" TTL: %u\n"), 		 	ip_header->ttl);
    printf_P(PSTR(" Checksum: %u\n"), 	htons(ip_header->checksum));

    switch(ip_header->protocol) {
        case IP_PROTOCOL_ICMP:
            printf_P(PSTR(" Protocol: ICMP\n"));
            break;

        case IP_PROTOCOL_IGMP:
            printf_P(PSTR(" Protocol: IGMP\n"));
            break;

        case IP_PROTOCOL_TCP:
            printf_P(PSTR(" Protocol: TCP\n"));
            break;

        case IP_PROTOCOL_UDP:
            printf_P(PSTR(" Protocol: UDP\n"));
            break;

        case IP_PROTOCOL_OSPF:
            printf_P(PSTR(" Protocol: OSPF\n"));
            break;

        case IP_PROTOCOL_SCTP:
            printf_P(PSTR(" Protocol: SCTP\n"));
            break;

        default:
            printf_P(PSTR(" Protocol: %u\n"), ip_header->protocol);
            break;
    }

    printf_P(PSTR("\n"));
}
#endif
