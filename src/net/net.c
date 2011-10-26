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

#include "net.h"

// Interface status container
static struct net_status_t net_status;

// Local packet buffer
static uint16_t net_packet_length = 0;
static uint8_t  net_packet_buffer[500] = {0};

/**
 * @function:   net_init
 * @param:      Ethernet controller hardware mac address
 * @param:      Network IP address
 * @param:      Network address subnetmask
 * @param:      Network's default router IP address
 * @brief:      Initialises the network stack
 */
void
net_init(mac_addr_t mac_address, ip_addr_t ip, ip_mask_t netmask, ip_addr_t default_router)
{
    // Initialize MAC Protocol
    mac_set_host_addr(mac_address);

    // Initialize ARP protocol
    arp_init();
    timer_set(arp_flush, 10);

    // Initialize IP protocol
    ip_set_host_addr(ip);
    ip_set_netmask(netmask);
    ip_set_default_router(default_router);

    // Get actual link status
    net_status.link = eth_get_link_status();

    // Drop some status information
    printf_P(PSTR("Chip Revision: %u\n"), eth_get_revision());
    printf_P(PSTR("Link status: %s\n\n"), (net_status.link) ? "UP" : "DOWN");
}

/**
 * @function:   net_get_status
 * @return:     Interface status and statistics
 * @brief:      Returns current interface status
 *              and statistics. Containing link
 *              status and transmission statistics.
 */
const struct net_status_t*
net_get_status(void) {
    return &net_status;
}

/**
 * @function:   net_periodic
 * @brief:      Run this function periodicly to handle
 *              incomming network traffic. Be aware that
 *              this might take some time to complete.
 *              Therefor it is not wise to run this from
 *              an interrupt routine, forexample that of
 *              a hardware timer.
 */
void
net_periodic(void)
{
    // Update link status
    net_status.link = eth_get_link_status();

    // Handle incomming packets
    while(eth_get_rx_packet_count()) {
        // Read packet from ethernet controller
        net_packet_length = eth_receive_packet(500, net_packet_buffer);

        // Update statistics
        net_status.packets_received++;
        net_status.bytes_received += net_packet_length;

#ifdef WITH_DEBUG
        net_debug(net_status.packets_received, net_packet_length, net_packet_buffer);
#endif

        // Decode packet, and reply if necessary.
        if((net_packet_length = net_decode(net_packet_length, net_packet_buffer))) {
            // Update statistics
            net_status.packets_sent++;
            net_status.bytes_sent += net_packet_length;

#ifdef WITH_DEBUG
            net_debug(net_status.packets_sent, net_packet_length, net_packet_buffer);
#endif

            // Sent packet to ethernet controller
            eth_send_packet(net_packet_length, net_packet_buffer);
        }
    }
}

/**
 * @function:   net_decode
 * @param:      The packet length
 * @param:      Pointer to the first byte of the packat
 * @return:     The length of a possible new packet
 */
uint16_t
net_decode(uint16_t length, uint8_t* packet)
{
    // Ensure data length matches header
    if(length < sizeof(struct mac_header_t)) {
        return 0;
    }

    // Create MAC header structure
    struct mac_header_t* mac_header = (struct mac_header_t*)(packet);

    // Check for encapsulated protocols
    switch(htons(mac_header->type)) {
        case MAC_TYPE_ARP :
            // Decode ARP header
            return arp_decode(length, packet);
            break;

        case MAC_TYPE_IP4 :

            // Ensure data length matches header
            if(length < sizeof(struct ip_header_t)) {
                return 0;
            }

            // Create IP header structure
            struct ip_header_t* ip_header = (struct ip_header_t*)(packet);

            // Check for encapsulated protocols
            switch(ip_header->protocol) {
                case IP_PROTOCOL_ICMP :
                    // Decode ICMP header
                    return icmp_decode(length, packet);
                    break;

                case IP_PROTOCOL_UDP :
                    // Decode UDP header
                    return udp_decode(length, packet);
                    break;

                case IP_PROTOCOL_TCP :
                    // Decode TCP header
                    return tcp_decode(length, packet);
                    break;
            }

            break;
    }

    return 0;
}

/**
 * @function:   net_debug
 * @param:      The packet length
 * @param:      Pointer to the first byte of the packat
 * @brief:      Printing the packet content in human readable
 *              format over stdout.
 */
#ifdef WITH_DEBUG
void
net_debug(uint16_t count, uint16_t length, uint8_t* packet)
{
    // Print header
    printf_P(PSTR("\n--------------------\n"));
    printf_P(PSTR("Packet #%u - %u bytes\n"), count, length);
    printf_P(PSTR("--------------------\n"));

    // Ensure data length matches header
    if(length < sizeof(struct mac_header_t)) {
        printf_P(PSTR("Invalid packet!\n"));
        return;
    }

    // Create MAC header structure
    struct mac_header_t* mac_header = (struct mac_header_t*)(packet);

    // Display MAC header
    mac_print_header(mac_header);

    // Check for encapsulated protocols
    switch(htons(mac_header->type)) {
        case MAC_TYPE_ARP :

            // Ensure data length matches header
            if(length < sizeof(struct arp_header_t)) {
                break;
            }

            // Create ARP header structure
            struct arp_header_t* arp_header = (struct arp_header_t*)(packet);

            // Display ARP header
            arp_print_header(arp_header);
            break;

        case MAC_TYPE_IP4 :

            // Ensure data length matches header
            if(length < sizeof(struct ip_header_t)) {
                break;
            }

            // Create IP header structure
            struct ip_header_t* ip_header = (struct ip_header_t*)(packet);

            // Display IP header
            ip_print_header(ip_header);

            // Check for encapsulated protocols
            switch(ip_header->protocol) {
                case IP_PROTOCOL_ICMP:

                    // Ensure data length matches header
                    if(length < sizeof(struct icmp_header_t)) {
                        break;
                    }

                    // Create ICMP header structure
                    struct icmp_header_t* icmp_header = (struct icmp_header_t*)(packet);

                    // Display ICMP header
                    icmp_print_header(icmp_header);
                    break;

                case IP_PROTOCOL_UDP:

                    // Ensure data length matches header
                    if(length < sizeof(struct udp_header_t)) {
                        break;
                    }

                    // Create UDP header structure
                    struct udp_header_t* udp_header = (struct udp_header_t*)(packet);

                    // Display UDP header
                    udp_print_header(udp_header);
                    break;

                case IP_PROTOCOL_TCP:

                    // Ensure data length matches header
                    if(length < sizeof(struct tcp_header_t)) {
                        break;
                    }

                    // Create TCP header structure
                    struct tcp_header_t* tcp_header = (struct tcp_header_t*)(packet);

                    // Display TCP header
                    tcp_print_header(tcp_header);
                    break;
            }

            break;
    }

    // Print footer
    printf_P(PSTR("--------------------\n"));
}
#endif
