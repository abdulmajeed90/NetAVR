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

#include "mac.h"

/**
 * @var:         static mac_addr_t
 * @brief:      Hardware mac address
 */
static mac_addr_t mac_host_addr;

/**
 * @function:   mac_set_host_addr
 * @param:      Local hardware mac address.
 * @brief:      Sets local hardware mac address.
 *              Be aware that this does not directly
 *              write the address to the ethernet
 *              controller. It is only a software
 *              representation.
 */
inline void
mac_set_host_addr(mac_addr_t mac_addr)
{
    memcpy(mac_host_addr, mac_addr, 6);
}

/**
 * @function:   mac_get_host_addr
 * @return:     Local hardware mac address.
 * @brief:      Returns local hardware mac address.
 */
inline uint8_t*
mac_get_host_addr(void)
{
    return mac_host_addr;
}

/**
 * @function:   mac_print_header
 * @param:      MAC packet header.
 * @brief:      Prints header content to stdout.
 */
#ifdef WITH_DEBUG
void
mac_print_header(struct mac_header_t* mac_header)
{
    printf_P(PSTR("Mac header\n"));

    printf_P(PSTR(" Source: %02X:%02X:%02X:"), mac_header->src_addr[0], mac_header->src_addr[1], mac_header->src_addr[2]);
    printf_P(PSTR("%02X:%02X:%02X\n"), mac_header->src_addr[3], mac_header->src_addr[4], mac_header->src_addr[5]);

    printf_P(PSTR(" Destination: %02X:%02X:%02X:"), mac_header->dest_addr[0], mac_header->dest_addr[1], mac_header->dest_addr[2]);
    printf_P(PSTR("%02X:%02X:%02X\n"), mac_header->dest_addr[3], mac_header->dest_addr[4], mac_header->dest_addr[5]);

    switch(htons(mac_header->type)) {
        case MAC_TYPE_ARP :
            printf_P(PSTR(" Type: ARP\n"));
            break;

        case MAC_TYPE_IP4 :
            printf_P(PSTR(" Type: IPv4\n"));
            break;

        case MAC_TYPE_IP6 :
            printf_P(PSTR(" Type: IPv6\n"));
            break;

        default:
            printf_P(PSTR(" Type: Invalid\n"));
            break;
    }

    printf_P(PSTR("\n"));
}
#endif
