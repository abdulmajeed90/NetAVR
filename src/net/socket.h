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
#include "udp.h"
#include "tcp.h"

#ifndef _SOCKET_H_
#define _SOCKET_H_

/**
 * @define:     MAX_SOCKETS
 * @brief:      The maximum number of sockets.
 */
#define MAX_SOCKETS	10

/**
 * @type:       sock_type_t
 * @brief:      Socket type.
 */
typedef uint8_t sock_type_t;

/**
 * @defines:    Available socket types.
 */
#define SOCK_STREAM 1 // Stream socket
#define SOCK_DGRAM  2 // Datagram socket
#define SOCK_RAW    3 // Raw protocol interface

/**
 * @type:       sock_family_t
 * @brief:      Socket address family type.
 */
typedef uint8_t sock_family_t;

/**
 * @defines:    Address families.
 */
#define	AF_LOCAL    1 // Local loopback
#define AF_INET     2 // UDP, TCP, etc

/**
 * @type: 		sock_inbound_t
 * @brief:		These functions are attached to a
 *              socket and called when the socket
 *              receives data.
 */
typedef uint16_t (* sock_inbound_t)(int socket, uint16_t length, uint8_t* data);

/**
 * @type: 		sock_accept_t
 * @brief:		These functions are attached to a
 *              server socket and called when the
 *              socket receives an incomming connection.
 */
typedef bool (* sock_accept_t)(const struct ip_header_t* ip);

/**
 * @struct:     sock_addr_t
 * @brief:      Structure used to store address information.
 */
struct sock_addr_t {
    uint16_t src_port;

    uint16_t dest_port;
    ip_addr_t dest_ip;
};

/**
 * @struct:     socket_t
 * @brief:      Structure used to store sockets.
 */
struct socket_t {
    sock_type_t type
    sock_family_t family;

    void* data;
    uint16_t length;

    sock_inbound_t inbound;
    sock_accept_t accept

    struct sock_addr_t* addr;
};

/**
 * @function:   sock_create
 * @param:      Socket address family
 * @param:      Scoket type
 * @return:     Socket descriptor
 * @brief:      Open a socket
 */
extern int8_t sock_create(sock_family_t sock_family, sock_type_t sock_type);

/**
 * @function:   sock_connect
 * @param:      Socket descriptor
 * @param:      Socket address structure
 * @param:      Inbound callback for handling incomming packets.
 * @brief:      Tries to create a peer with the given destination
 *              ip in the address structure.
 */
extern int8_t sock_connect(int8_t socket, const struct sock_addr_t* addr, sock_inbound_t callback);

/**
 * @function:   sock_bind
 * @param:      Socket descriptor
 * @param:      Socket address structure
 * @param:      Accept callback for handling incomming connections.
 * @brief:      Make a socket listen on a specific port for incomming connections.
 */
extern int8_t sock_bind(int8_t socket, const struct sock_addr_t* addr, sock_accept_t callback);

/**
 * @function:   sock_accept
 * @param:      Socket descriptor
 * @param:      Socket address structure to be filled
 * @param:      Inbound callback for handling incomming packets.
 * @brief:      Make a socket listen for connections on a specific port.
 */
extern int8_t sock_accept(int8_t socket, struct sock_addr_t* addr, sock_inbound_t callback);

/**
 * @function:   sock_read
 * @param:      Socket descriptor
 * @param:      Buffer to place read data in
 * @param:      Number of bytes to read at maximum
 * @return:     Actual number of bytes read
 * @brief:      Reads pending data from the socket.
 */
extern uint16_t sock_read(int8_t socket, void* data, uint16_t length);

/**
 * @function:   sock_write
 * @param:      Socket descriptor
 * @param:      Data buffer to be written
 * @param:      The number of bytes to write
 * @return:     The actual number of bytes written
 * @brief:      Writes data to the socket
 */
extern uint16_t sock_write(int8_t socket, uint8_t* data, uint16_t length);

/**
 * @function:   sock_udp_inbound
 * @param:      UDP network header
 * @param:      Incomming data buffer
 * @return:     Size of the reply packet
 * @brief:      Hanles incomming UDP traffic
 */
extern uint16_t sock_udp_inbound(struct udp_header_t* udp_header, uint8_t* data);

/**
 * @function:   sock_close
 * @param:      Socket descriptor
 * @brief:      Closes a socket
 */
extern int8_t sock_close(int8_t socket);

/* !_SOCKET_H_ */
#endif
