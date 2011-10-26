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

#include "icmp.h"

/**
 * @function:   icmp_decode
 * @param:      uint16_t, Length of the packet received.
 * @param:      uint8_t *, Pointer to the first byte of the packet.
 * @return:     uint16_t, Size of the new reply packet to be transmitted.
 * @brief:      Decodes a received packet into a ICMP-packet and
 *              runs nessesary actions. Which eventually will create
 *              an appropriate reply.
 */
uint16_t
icmp_decode(uint16_t length, uint8_t* packet)
{
    // Check if packet length is ok
    if(length < sizeof(struct icmp_header_t)) {
        return 0;
    }

    // Convert array to header
    struct icmp_header_t* icmp_header = (struct icmp_header_t*) packet;

    // Switch to check the types of the ICMP message
    switch(icmp_header->type) {
            // If the type is 8, the message is an echo request and a message should send back to the requester
        case ICMP_TYPE_ECHO_REQUEST:
            if(icmp_header->code == 0) {
                return icmp_echo_reply(length, packet);
            }

            break;
    }

    return 0;
}

/**
 * @function:   icmp_checksum
 * @param:      uint16_t, Length of the ICMP-header.
 * @param:      uint8_t *, Pointer to the first byte of the header.
 * @brief:      Calculates the checksum over an ICMP-header, the length of the header
 *              is 8 most of the times because a header without any data has a size of 8
 *              bytes.
 */
uint16_t
icmp_checksum(uint16_t length, uint8_t* packet)
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
 * @function:   icmp_echo_reply
 * @param:      uint16_t, Length of the packet received.
 * @param:      uint8_t *, Pointer to the first byte of the packet.
 * @return:     uint16_t, Size of the new reply packet to be transmitted.
 * @brief:      Creates a message that responds to a echo request message from an random host.
 */
uint16_t
icmp_echo_reply(uint16_t length, uint8_t* packet)
{
    // Create header overlay
    struct icmp_header_t* icmp_header = (struct icmp_header_t*)(packet);

    // Set new destination and source mac address
    memcpy(icmp_header->ip.mac.dest_addr, icmp_header->ip.mac.src_addr, 6);
    memcpy(icmp_header->ip.mac.src_addr, mac_get_host_addr(), 6);

    // Set new destination and source ip address
    memcpy(icmp_header->ip.dest_addr, icmp_header->ip.src_addr, 4);
    memcpy(icmp_header->ip.src_addr, ip_get_host_addr(), 4);

    // Change the ICMP type from echo-request to echo-reply
    icmp_header->code = ICMP_TYPE_ECHO_REPLY;

    // Change the ICMP code from echo-request to echo-reply
    icmp_header->type = ICMP_CODE_ECHO_REPLY;

    // Recalculate the ICMP checksum
    icmp_header->checksum = 0;	// First, it should be set to 0
    icmp_header->checksum = htons(icmp_checksum(ICMP_DEFAULT_HEADER_LENGTH + 56, &packet[MAC_DEFAULT_HEADER_LENGTH + IP_DEFAULT_HEADER_LENGTH]));

    // Return the size of the packet for transmission
    return sizeof(struct icmp_header_t);
}

/**
 * @function:   icmp_print_header
 * @param:      struct icmp_header_t *, ICMP packet header.
 * @brief:      Prints header content to stdout.
 */
#ifdef WITH_DEBUG
void
icmp_print_header(struct icmp_header_t* icmp_header)
{
    printf_P(PSTR("ICMP header\n"));

    switch(icmp_header->type) {
        case ICMP_TYPE_ECHO_REPLY:
            printf_P(PSTR(" Type: Reply\n"));
            break;

        case ICMP_TYPE_ECHO_REQUEST:
            printf_P(PSTR(" Type: Request\n"));
            break;

        default:
            printf_P(PSTR(" Type: Other [%u]\n"), icmp_header->type);
            break;
    }

    printf_P(PSTR(" Code: %u\n"), icmp_header->code);

    printf_P(PSTR(" ID: %u\n"), htons(icmp_header->id));
    printf_P(PSTR(" Sequence: %u\n"), htons(icmp_header->sequence));
    printf_P(PSTR(" Checksum: %u\n\n"), htons(icmp_header->checksum));
}
#endif
