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

#include "tcp.h"

/**
 * @var:        tcp_bindings
 * @brief:      TCP port binding list.
 */
static struct tcp_bind_t* tcp_bindings[TCP_MAX_BINDINGS];

/**
 * @function:   tcp_checksum
 * @param:      The length of the header including data
 * @param:      Pointer to the first byte of the data
 * @return:     The two-byte checksum of the TCP packet
 * @brief:      udp_checksum () calculates the checksum of an TCP packet
 */
uint16_t
tcp_checksum(uint16_t length, uint8_t* packet, const ip_addr_t src_addr, const ip_addr_t dest_addr)
{
    uint16_t i = 0;
    uint32_t sum = 0;

    // The protocol number and the length of the packet
    sum = length + IP_PROTOCOL_TCP;

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
 * @function:   tcp_decode
 * @param:      The length of the received packet
 * @param:      Pointer to the first byte of the received data
 * @return:     The length of an (optional) message to be transmitted
 * @brief:      This functions decodes an incoming TCP packet and checks if the port has been bound.
 *              If so, the function attached will be called to process the received data within the TCP packet.
 */
uint16_t
tcp_decode(uint16_t length, uint8_t* packet)
{
    uint8_t id = 0;

    // Check the length of the packet
    if(length < sizeof(struct tcp_header_t)) {
        return 0;
    }

    // Create TCP header structure
    struct tcp_header_t* tcp_header = (struct tcp_header_t*)(packet);

    // Find the corrosponding port binding
    for(; (id < TCP_MAX_BINDINGS) && (tcp_bindings[id]->port != htons(tcp_header->dest_port)); id++) {
        continue;
    }

    // Corresponding binding has been found?
    if(id == TCP_MAX_BINDINGS) {
        return 0;
    }

    // XXX: Run TCP state machine.
    //      Still to be implemented.

    return 0;
}

/**
 * @function:   tcp_bind
 * @param:      Port to bind.
 * @param:      Function to call when there's a packet received at the specified port
 * @return:     True if the port has been succesfully bound.
 * @brief:      Binds a port with the specified callback function.
 */
bool
tcp_bind(uint16_t port, tcp_inbound_t callback)
{
    uint8_t id = 0;

    for(; (id < TCP_MAX_BINDINGS) && (tcp_bindings[id] == NULL); id++) {
        continue;
    }

    if(id == TCP_MAX_BINDINGS) {
        return false;
    }

    tcp_bindings[id] = (struct tcp_bind_t*) calloc(1, sizeof(struct tcp_bind_t));

    if(tcp_bindings[id] == NULL) {
        return false;
    }

    tcp_bindings[id]->port = port;
    tcp_bindings[id]->callback = callback;

    return false;
}

/**
 * @function:   tcp_unbind
 * @param:      The port number to unbind
 * @result:     True if port has been succesfully unbound
 * @brief:      Removes binding with its callback function
 */
bool
tcp_unbind(uint16_t port)
{
    uint8_t id = 0;

    for(; (id < TCP_MAX_BINDINGS) && (tcp_bindings[id]->port != port); id++) {
        continue;
    }

    if(id == TCP_MAX_BINDINGS) {
        return false;
    }

    free(tcp_bindings[id]);
    return true;
}

/**
 * @function:   tcp_print_header
 * @param:      TCP packet header.
 * @brief:      Prints header content to stdout.
 */
#ifdef WITH_DEBUG
void
tcp_print_header(struct tcp_header_t* tcp_header)
{
    printf_P(PSTR("TCP Header\n"));

    printf_P(PSTR(" Source port: %u\n"), htons(tcp_header->src_port));
    printf_P(PSTR(" Destination port: %u\n"), htons(tcp_header->dest_port));

    printf_P(PSTR(" Sequence No.: %lu\n"), tcp_header->seq_no);
    printf_P(PSTR(" Acknowledge No.: %lu\n"), tcp_header->ack_no);

    printf_P(PSTR(" Offset: %u\n"), tcp_header->offset >> 4);

    printf_P(PSTR(" Flags:\n"));
    printf_P(PSTR("  -Fin: %s\n"), (TCP_GET_FLAG(tcp_header, TCP_FLAG_FIN)) ? "Set" : "Clear");
    printf_P(PSTR("  -Syn: %s\n"), (TCP_GET_FLAG(tcp_header, TCP_FLAG_SYN)) ? "Set" : "Clear");
    printf_P(PSTR("  -Rst: %s\n"), (TCP_GET_FLAG(tcp_header, TCP_FLAG_RST)) ? "Set" : "Clear");
    printf_P(PSTR("  -Psh: %s\n"), (TCP_GET_FLAG(tcp_header, TCP_FLAG_PSH)) ? "Set" : "Clear");
    printf_P(PSTR("  -Ack: %s\n"), (TCP_GET_FLAG(tcp_header, TCP_FLAG_ACK)) ? "Set" : "Clear");
    printf_P(PSTR("  -Urg: %s\n"), (TCP_GET_FLAG(tcp_header, TCP_FLAG_URG)) ? "Set" : "Clear");
    printf_P(PSTR("  -Ece: %s\n"), (TCP_GET_FLAG(tcp_header, TCP_FLAG_ECE)) ? "Set" : "Clear");
    printf_P(PSTR("  -Cwr: %s\n"), (TCP_GET_FLAG(tcp_header, TCP_FLAG_CWR)) ? "Set" : "Clear");

    printf_P(PSTR(" Window: %u\n"), htons(tcp_header->window));
    printf_P(PSTR(" Checksum: %u\n"), htons(tcp_header->checksum));
    printf_P(PSTR(" Urgent: %u\n"), htons(tcp_header->urgent));
}
#endif
