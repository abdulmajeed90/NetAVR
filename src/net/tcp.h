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
#include "ip.h"

#ifndef _TCP_H_
#define _TCP_H_

// TCP Flags
#define TCP_FLAG_FIN 0x01
#define TCP_FLAG_SYN 0x02
#define TCP_FLAG_RST 0x04
#define TCP_FLAG_PSH 0x08
#define TCP_FLAG_ACK 0x10
#define TCP_FLAG_URG 0x20
#define TCP_FLAG_ECE 0x40
#define TCP_FLAG_CWR 0x80

#define TCP_GET_FLAG(header,mask) (header->flags & mask)

/**
 * @define: TCP_MAX_BINDINGS
 * @brief:  The maximum number of TCP port bindings
 */
#define TCP_MAX_BINDINGS	10

/**
 * @define: TCP_HEADER_LENGTH
 * @brief:  The length in bytes of the default TCP header.
 */
#define TCP_HEADER_LENGTH	20

/**
 * @struct:		tcp_header_t
 * @brief:      TCP network packet header
 */
struct tcp_header_t {
    struct ip_header_t ip;

    uint16_t src_port;
    uint16_t dest_port;

    uint32_t seq_no;
    uint32_t ack_no;

    uint8_t offset;
    uint8_t flags;

    uint16_t window;
    uint16_t checksum;
    uint16_t urgent;
} __attribute__((__packed__));

/**
 * @type:       tcp_inbound_t
 * @brief:      Callback type for TCP bindings.
 *              These functions are called when
 *              there is inbound data on the bound port.
 */
typedef uint16_t (*tcp_inbound_t)(struct tcp_header_t* tcp_header, uint8_t* data);

/**
 * @struct:     tcp_bind_t
 * @brief:      TCP port binding structure. Holds the
 *              inbound callback for a specific port.
 */
struct tcp_bind_t {
    uint16_t port;
    tcp_inbound_t callback;

    struct tcp_session_t* sessions;
};

/**
 * @enum:       tcp_status_t
 * @brief:      TCP port binding structure. Holds the
 *              inbound callback for a specific port.
 */
enum tcp_status_t {
    CLOSED,
    LISTEN,
    SYN_SENT,
    SYN_RECEIVED,
    ESTABLISHED,
    FIN_WAIT_1,
    FIN_WAIT_2,
    CLOSE_WAIT,
    CLOSING,
    LAST_ACK,
    TIME_WAIT
};

struct tcp_session_t {
    ip_addr_t ip;
    enum tcp_status_t state;
    uint32_t last_seq_no;
    uint32_t last_ack_no;
    struct tcp_session_t* next;
};

/**
 * @function:   tcp_checksum
 * @param:      The length of the header including data
 * @param:      Pointer to the first byte of the data
 * @param:      IP source address
 * @param:      IP destination address
 * @return:     The two-byte checksum of the TCP packet
 * @brief:      Calculates the checksum of an TCP packet
 */
extern uint16_t tcp_checksum(uint16_t length, uint8_t* packet, const ip_addr_t src_addr, const ip_addr_t dest_addr);

/**
 * @function:   tcp_decode
 * @param:      The length of the received packet
 * @param:      Pointer to the first byte of the received data
 * @return:     The length of an (optional) message to be transmitted
 * @brief:      This functions decodes an incoming TCP packet and checks if the port has been bound.
 *              If so, the function attached will be called to process the received data within the TCP packet.
 */
extern uint16_t tcp_decode(uint16_t length, uint8_t* data);

/**
 * @function:   tcp_bind
 * @param:      Port to bind.
 * @param:      Function to call when there's a packet received at the specified port
 * @return:     True if the port has been succesfully bound.
 * @brief:      Binds a port with the specified callback function.
 */
extern bool tcp_bind(uint16_t port, tcp_inbound_t callback);

/**
 * @function:   tcp_unbind
 * @param:      The port number to unbind
 * @result:     True if port has been succesfully unbound
 * @brief:      Removes binding with its callback function
 */
extern bool tcp_unbind(uint16_t port);

/**
 * @function:   tcp_print_header
 * @param:      TCP packet header.
 * @brief:      Prints header content to stdout.
 */
#ifdef WITH_DEBUG
extern void tcp_print_header(struct tcp_header_t* header);
#endif

/* !_TCP_H */
#endif
