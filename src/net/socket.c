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

#include <stdlib.h>

#include "socket.h"
#include "dev/eth.h"

static struct socket_t* sockets[MAX_SOCKETS];

/**
 * @function:   sock_create
 * @param:      Socket address family
 * @param:      Scoket type
 * @return:     Socket descriptor
 * @brief:      Open a socket
 */
int8_t
sock_create(sock_family_t sock_family, sock_type_t sock_type);
{
    int8_t id = 0;

    for(; (id < MAX_SOCKETS) && (sockets[id] != NULL); id++) {
        continue;
    }

    if(id == MAX_SOCKETS) {
        return -1;
    }

    sockets[id] = (struct socket_t*) malloc(sizeof(struct socket_t));

    sockets[id]->family = sock_family;
    sockets[id]->type = sock_type;

    return id;
}

/**
 * @function:   sock_connect
 * @param:      Socket descriptor
 * @param:      Socket address structure
 * @param:      Inbound callback for handling incomming packets.
 * @brief:      Tries to create a peer with the given destination
 *              ip in the address structure.
 */
int8_t
sock_connect(int8_t socket, const struct sock_addr_t* addr, sock_inbound_t callback);
{
    if(sockets[socket] == NULL) {
        return -1;
    }

    // XXX: Still to be implemented
    return -1;
}

/**
 * @function:   sock_bind
 * @param:      Socket descriptor
 * @param:      Socket address structure
 * @param:      Accept callback for handling incomming connections.
 * @brief:      Make a socket listen on a specific port for incomming connections.
 */
int8_t
sock_bind(int8_t socket, const struct sock_addr_t* addr, sock_accept_t callback);
{
    if(sockets[socket] == NULL) {
        return -1;
    }

    memcpy(sockets[socket]->addr, addr, sizeof(struct sock_addr_t));
    sockets[socket]->accept = callback;

    switch(socket->family) {
        case AF_LOCAL:
            // XXX: Yet to be implemented
            break;

        case AF_INET:
            switch(socket->type) {
                case SOCK_STREAM:
                    // XXX: Yet to be implemented
                    return -1;
                    break;

                case SOCK_DGRAM:
                    if(udp_bind(addr->src_port, sock_udp_inbound)) {
                        return 0;
                    }

                    return -1;
                    break;

                case SOCK_RAW:
                    // XXX: Yet to be implemented
                    return -1;
                    break

                default:
                    // Unknow socket type, return error
                    return -1
            }
    }

           return 0;
}

/**
 * @function:   sock_accept
 * @param:      Socket descriptor
 * @param:      Socket address structure to be filled
 * @param:      Inbound callback for handling incomming packets.
 * @brief:      Make a socket listen for connections on a specific port.
 */
int8_t
sock_accept(int8_t socket, struct sock_addr_t* addr, sock_inbound_t callback)
{
    // XXX: Still to be implemented
    return -1;
}

/**
 * @function:   sock_read
 * @param:      Socket descriptor
 * @param:      Buffer to place read data in
 * @param:      Number of bytes to read at maximum
 * @return:     Actual number of bytes read
 * @brief:      Reads pending data from the socket.
 */
uint16_t
sock_read(int socket, void* data, uint16_t length)
{
    // XXX: Still to be implemented
    return 0;
}

/**
 * @function:   sock_write
 * @param:      Socket descriptor
 * @param:      Data buffer to be written
 * @param:      The number of bytes to write
 * @return:     The actual number of bytes written
 * @brief:      Writes data to the socket
 */
uint16_t
sock_write(int socket, uint8_t* data, uint16_t length)
{
    if(sockets[socket] == NULL) {
        return 0;
    }

    const struct socket_t* sock = sockets[socket];

    switch(sock->family) {
        case AF_INET :
            switch(sock->type) {
                case SOCK_DGRAM :
                    struct udp_header_t* udp_header = (struct udp_header_t*) malloc(sizeof(struct udp_header_t) + length);

                    if(udp_header == NULL) {
                        return 0;
                    }

                    // Copy the data
                    memcpy((udp_header + MAC_DEFAULT_HEADER_LENGTH + IP_DEFAULT_HEADER_LENGTH + UDP_DEFAULT_HEADER_LENGTH), data, length);

                    // Fill the UDP header
                    udp_header->src_port 	= htons(sock->addr->src_port);
                    udp_header->dest_port	= htons(sock->addr->dest_port);
                    udp_header->length		= htons(length + UDP_DEFAULT_HEADER_LENGTH);
                    udp_header->checksum	= 0;

                    // Fill the IP header
                    memset((&udp_header->ip + MAC_DEFAULT_HEADER_LENGTH), 0, IP_DEFAULT_HEADER_LENGTH);
                    udp_header->ip.version	= 0x45;
                    udp_header->ip.length	= htons(IP_DEFAULT_HEADER_LENGTH + UDP_DEFAULT_HEADER_LENGTH + length);
                    udp_header->ip.id		= htons((uint16_t) random());
                    udp_header->ip.ttl		= IP_DEFAULT_TTL;
                    udp_header->ip.protocol	= IP_PROTOCOL_UDP;
                    memcpy(udp_header->ip.src_addr, ip_get_host_addr(), 4);
                    memcpy(udp_header->ip.dest_addr, sock->addr->dest_ip, 4);

                    // Calculate the IP checksum
                    udp_header->ip.checksum	= htons(ip_checksum(IP_DEFAULT_HEADER_LENGTH, (&udp_header->ip + MAC_DEFAULT_HEADER_LENGTH)));

                    // Fill the MAC header
                    udp_header->ip.mac.type	= htons(MAC_TYPE_IP4);

                    uint16_t temp = arp_encode((sizeof(struct udp_header_t) + length), (uint8_t*) udp_header);
                    eth_write_packet(temp, (uint8_t*) udp_header);
                    free(udp_header);

                    return temp;
                    break;

                case SOCK_STREAM:
                    // XXX: Still to be implemented
                    break;
            }

            break;
    }

    return 0;
}

/**
 * @function:   sock_udp_inbound
 * @param:      UDP network header
 * @param:      Incoming data buffer
 * @return:     Size of the reply packet
 * @brief:      Handles incomming UDP traffic
 */
uint16_t
sock_udp_inbound(struct udp_header_t* udp_header, uint8_t* data)
{
    // Find the corrosponding socket
    uint16_t id = 0;

    for(; (sockets[id] != NULL) && (sockets[id]->addr->src_port == udp_header->dest_port); id++) {
        continue;
    }

    if(sockets[id] == NULL) {
        return 0;
    }

    // Copy the remote IP address and remote port
    memcpy(sockets[id]->addr->dest_ip, udp_header->ip.dest_addr, 4);
    sockets[id]->addr->dest_port = udp_header->src_port;

    // Call the inbound function
    uint16_t length = htons(udp_header->length) - UDP_DEFAULT_HEADER_LENGTH;
    sockets[id]->inbound(id, length, data);

    return 0;
}

/**
 * @function:   sock_close
 * @param:      Socket descriptor
 * @brief:      Closes a socket
 */
int8_t
sock_close(int socket)
{
    // XXX: Still to be implemented
    return -1;
}
