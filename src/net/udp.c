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

#include "udp.h"

/**
 * @var:        udp_bindings
 * @brief:      UDP port binding list.
 */
static struct udp_bind_t* udp_bindings[UDP_MAX_BINDINGS];

/**
 * @function:   udp_checksum
 * @param:      The length of the header including data
 * @param:      Pointer to the first byte of the data
 * @return:     The two-byte checksum of the UDP packet
 * @brief:      udp_checksum () calculates the checksum of an UDP packet
 */
uint16_t
udp_checksum(uint16_t length, uint8_t* packet, const ip_addr_t src_addr, const ip_addr_t dest_addr)
{
    uint16_t i = 0;
    uint32_t sum = 0;

    // The protocol number and the length of the packet
    sum = length + IP_PROTOCOL_UDP;

    // Make 16 bit words out of every two adjecent 8 bit words and
    // calculate the sum of all 16 bit words
    for(i = 0; i < length; i += 2) {
        sum += ((packet[i] << 8) & 0xFF00) + packet[i + 1];
    }

    // Apply padding when needed
    if(length % 2) {
        sum += ((packet[length] << 8)  & 0xFF00);
    }

    // Add the pseudo header IP source and destination address
    for(i = 0; i < 4; i += 2) {
        sum += (src_addr[i] << 8) + src_addr[i + 1];
        sum += (dest_addr[i] << 8) + dest_addr[i + 1];
    }

    // Take only 16 bits out of the 32 bit sum and add up the carries
    while(sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    // One's complement the result
    sum = ~sum;

    // Return the checksum
    return (uint16_t)(sum);
}

/**
 * @function:   udp_decode
 * @param:      The length of the received packet
 * @param:      Pointer to the first byte of the received data
 * @return:     The length of an (optional) message to be transmitted
 * @brief:      This functions decodes an incoming UDP packet and checks whether an socket is opened on that port.
 *              If so, the function attached to the socket will be called to process the received data within the UDP packet.
 */
uint16_t
udp_decode(uint16_t length, uint8_t* packet)
{
    uint8_t id = 0;
    uint16_t checksum = 0;

    // Check packet length
    if(length < sizeof(struct udp_header_t)) {
        return 0;
    }

    // Create UDP header structure
    struct udp_header_t* udp_header = (struct udp_header_t*)(packet);

    // Search for corresponding port binding
    for(; (id < UDP_MAX_BINDINGS) && (udp_bindings[id]->port != htons(udp_header->dest_port)); id++) {
        continue;
    }

    // Corresponding binding has been found?
    if(id == UDP_MAX_BINDINGS) {
        return 0;
    }

    // Check UDP checksum when required
    if((checksum = htons(udp_header->checksum)) != 0) {
        // Clear checksum
        udp_header->checksum = 0;

        // Calculate and compare
        if(checksum != udp_checksum(htons(udp_header->length),
                                    packet + (MAC_DEFAULT_HEADER_LENGTH + IP_DEFAULT_HEADER_LENGTH),
                                    udp_header->ip.src_addr, udp_header->ip.dest_addr)) {
            return 0;
        }
    }

    // Create data block pointer
    uint8_t* data = packet + (MAC_DEFAULT_HEADER_LENGTH + IP_DEFAULT_HEADER_LENGTH + UDP_DEFAULT_HEADER_LENGTH);

    // Execute callback function
    return udp_bindings[id]->callback(udp_header, data);
}

/**
 * @function:   udp_bind
 * @param:      Port to bind.
 * @param:      Function to call when there's a packet received at the specified port
 * @return:     True if the port has been succesfully bound.
 * @brief:      Binds a port with the specified callback function.
 */
bool
udp_bind(uint16_t port, udp_inbound_t callback)
{
    uint8_t id = 0;

    for(; (id < UDP_MAX_BINDINGS) && (udp_bindings[id] != NULL); id++) {
        continue;
    }

    if(id == UDP_MAX_BINDINGS) {
        return false;
    }

    udp_bindings[id] = (struct udp_bind_t*) calloc(1, sizeof(struct udp_bind_t));

    if(udp_bindings[id] == NULL) {
        return false;
    }

    udp_bindings[id]->port = port;
    udp_bindings[id]->callback = callback;

    return true;
}

/**
 * @function:   udp_unbind
 * @param:      The port number to unbind
 * @result:     True if port has been succesfully unbound
 * @brief:      Removes binding with its callback function
 */
bool
udp_unbind(uint16_t port)
{
    uint8_t id = 0;

    for(; (id < UDP_MAX_BINDINGS) && (udp_bindings[id]->port != port); id++) {
        continue;
    }

    if(id == UDP_MAX_BINDINGS) {
        return false;
    }

    free(udp_bindings[id]);
    return true;
}

/**
 * @function:   udp_print_header
 * @param:      UDP packet header.
 * @brief:      Prints header content to stdout.
 */
#ifdef WITH_DEBUG
void
udp_print_header(struct udp_header_t* udp_header)
{
    printf_P(PSTR("UDP Header\n"));

    printf_P(PSTR(" Source port: %u\n"), htons(udp_header->src_port));
    printf_P(PSTR(" Destination port: %u\n"), htons(udp_header->dest_port));

    printf_P(PSTR(" Length: %u\n"), htons(udp_header->length));
    printf_P(PSTR(" Checksum: %u\n"), htons(udp_header->checksum));
}
#endif
