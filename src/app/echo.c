/**
 * Copyright 2010 Roy van Dam <roy@8bit.cx>
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

#include "echo.h"

uint16_t
echo_udp(struct udp_header_t* udp_header, uint8_t* data)
{
    // Swap ports
    udp_header->dest_port = udp_header->src_port;
    udp_header->src_port = htons(7);

    // Set new destination and source mac address
    memcpy(udp_header->ip.mac.dest_addr, udp_header->ip.mac.src_addr, 6);
    memcpy(udp_header->ip.mac.src_addr, mac_get_host_addr(), 6);

    // Set new destination and source ip address
    memcpy(udp_header->ip.dest_addr, udp_header->ip.src_addr, 4);
    memcpy(udp_header->ip.src_addr, ip_get_host_addr(), 4);

    // Return packet
    return sizeof(struct udp_header_t) + (htons(udp_header->length) - UDP_DEFAULT_HEADER_LENGTH);
}
