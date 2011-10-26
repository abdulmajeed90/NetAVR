/**
 * Copyright 2010 Ramon Kleiss <ramonkleiss@gmail.com>
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

#include "dhcp.h"

static uint8_t dhcp_magic_cookie[4] = {99, 130, 83, 99};
static dhcp_state_t dhcp_state = init;
static int dhcp_socket = -1;

static ip_addr_t dhcp_server = {0, 0, 0, 0};

static uint32_t dhcp_xid = 0;
static uint32_t	dhcp_lease_time = 0;

void
dhcp_init(void)
{
    // Open the DHCP socket
    dhcp_socket = sock_open(SOCK_DGRAM);

    // Bind the socket to port 67
    sock_bind(dhcp_socket, 67, dhcp_daemon);
}

uint16_t
dhcp_discover(uint16_t length, uint8_t* packet)
{
    dhcp_xid = (uint32_t) random();
    struct dhcp_header_t* dhcp = dhcp_create_header(dhcp, dhcp_xid);

    // Add the dhcp options
    uint16_t	opt_count;
    uint8_t		option[4];

    opt_count = dhcp_add_opt(dhcp, 0, magic_cookie, 4);	// Add the magic cookie option

    option[0] = 53;
    option[1] = 1;
    option[2] = DHCP_DISCOVER;
    option[3] = 0;
    opt_count = dhcp_add_opt(dhcp, opt_count, option, 4);	// Add the DHCPDISCOVER option

    // Make the packet parameter point to our dhcp header
    if(packet) {
        free(packet);
    }

    packet = (uint8_t*) dhcp;

    dhcp_state = selecting;

    // Return the size of the packet to the user
    return (sizeof(struct dhcp_header_t) + opt_count);
}

uint16_t
dhcp_request(uint16_t length, uint8_t* packet)
{
    // Check the length of the received packet
    if(length < sizeof(struct dhcp_header_t)) {
        return 0;
    }

    // Convert the packet to a DHCP header
    struct dhcp_header_t* dhcp = (struct dhcp_header_t*) packet;

    // Check if the opcode is BOOT_REPLY and the session id is the same as we sent
    if(dhcp->op == DHCP_BOOT_REPLY && dhcp->xid == dhcp_xid) {
        mac_addr_t	host_addr 	= mac_get_host_addr();

        // Check MAC address
        if(!memcmp(dhcp->chaddr, host_addr, 6)) {
            // Get the options from the header
            uint8_t* 	get_option;
            uint16_t	get_opt_count;

            // Get the magic cookie first
            get_opt_count = dhcp_get_opt(dhcp, opt_count, get_option);

            // Check if the magic cookie is present, else just ignore the package
            if(memcmp(get_option, dhcp_magic_cookie, 4)) {
                return 0;
            }

            // Get the yiaddr field from the header for later confirmation
            ip_addr_t yiaddr;
            memcpy(dhcp->yiaddr, yiaddr, 4);

            while(get_option) {
                get_opt_count = dhcp_get_opt(dhcp, get_opt_count, get_option);

                switch(get_option[DHCP_OPT_TYPE]) {
                    case DHCP_SUBNET_MASK:
                        // Get the subnet mask from the header
                        ip_addr_t addr;
                        memcpy(addr, get_option + DHCP_OPT_VALUE_START, get_option[DHCP_OPT_LENGTH]);

                        // Set the subnet mask in this host
                        ip_set_netmask(addr);
                        break;

                    case DHCP_ROUTER_ADDR:
                        // Get the router address from the header
                        ip_addr_t addr;
                        memcpy(addr, option + DHCP_OPT_VALUE_START, option[DHCP_OPT_LENGTH]);

                        // Set the default router for the host
                        ip_set_router(mask);
                        break;

                    case DHCP_SERVER_ADDR:
                        // Get the server address from the header
                        ip_addr_t addr;
                        memcpy(addr, option + DHCP_OPT_VALUE_START, option[DHCP_OPT_LENGTH]);

                        // Set the DHCP server address in this host
                        memcpy(dhcp_server, addr, 4);
                        break;

                    default:
                        // We can ignore this option, 'cause we won't be using it
                        break;
                }
            }

            // Create the DHCPREQUEST message
            struct dhcp_header_t* dhcp_request = dhcp_create_header(dhcp_request, dhcp_xid);

            uint16_t	opt_count;
            uint8_t*		option;

            opt_count = 0;
            opt_count = dhcp_add_opt(dhcp_request, 0, dhcp_magic_cookie, 4);

            // Add the message type option
            option = (uint8_t*) calloc(3, sizeof(uint8_t));
            option[0] = DHCP_MESSAGE_TYPE;
            option[1] = 1;
            option[2] = DHCP_REQUEST;
            opt_count = dhcp_add_opt(dhcp_request, opt_count, option, 3);
            free(option);

            // Add the address request
            option = (uint8_t*) calloc(6, sizeof(uint8_t));
            option[0] = DHCP_REQUESTED_ADDR;
            option[1] = 4;
            memcpy(option + 2, yiaddr, 4);
            opt_count = dhcp_add_opt(dhcp_request, opt_count, option, 6);
            free(option);

            // Add the dhcp server option
            option = (uint8_t*) calloc(6, sizeof(uint8_t));
            option[0] = DHCP_SERVER_ADDR;
            option[1] = 4;
            memcpy(option + 2, dhcp_server, 4);
            opt_count = dhcp_add_opt(dhcp_request, opt_count, option, 6);
            free(option);

            // Reset the packet pointer to our dhcp header
            if(packet) {
                free(packet);
            }

            packet = (uint8_t*) dhcp_request;

            dhcp_state = requesting;

            return (sizeof(struct dhcp_header_t) + opt_count);
        }
    }

    return 0;
}

uint16_t
dhcp_bind(uint16_t length, uint8_t* packet)
{
    // Check if the packet is long enough
    if(length < sizeof(struct dhcp_header_t)) {
        return 0;
    }

    struct dhcp_header_t* dhcp_reply = (struct dhcp_header_t*) packet;

    // Check MAC address, session id and opcode
    if(dhcp_reply->op == DHCP_BOOT_REPLY && dhcp_reply->xid == dhcp_xid && !memcmp(dhcp_reply->chaddr, mac_get_host_addr(), 6)) {
        ip_addr_t yiaddr;
        memcpy(yiaddr, dhcp_reply->yiaddr, 4);

        // Set the host's IP address to the received IP address
        ip_set_host_addr(yiaddr);

        uint16_t 	get_opt_count = 0;
        uint8_t* 	get_option;

        // Check the magic cookie, if not correct, discard the packet
        get_opt_count = dhcp_get_opt(dhcp_reply, get_opt_count, get_option);

        if(memcmp(get_option, dhcp_magic_cookie, 4)) {
            return 0;
        }

        get_opt_count = dhcp_get_opt(dhcp_reply, get_opt_count, get_option);

        while(get_option) {
            switch(get_option[DHCP_OPT_TYPE]) {
                    // Check if we get an ack or a nak
                case DHCP_MESSAGE_TYPE:
                    if(get_option[DHCP_OPT_VALUE_START] == DHCP_NACK) {
                        dhcp_state = init;
                    } else {
                        dchp_state = bound;
                    }

                    break;

                case DHCP_SUBNET_MASK:
                    ip_addr_t addr;
                    memcpy(addr, get_option + DHCP_OPT_VALUE_START, 4);
                    ip_set_netmask(addr);
                    break;

                case DHCP_ROUTER_ADDR:
                    ip_addr_t addr;
                    memcpy(addr, get_option + DHCP_OPT_VALUE_START, 4);
                    ip_set_router(addr);
                    break;

                case DHCP_LEASE_TIME:
                    memcpy(dhcp_lease_time, get_option + DHCP_OPT_VALUE_START, 4);
                    break;

                case DHCP_SERVER_ADDR:
                    memcpy(dhcp_server, get_option + DHCP_OPT_VALUE_START, 4);
                    break;

                default:
                    break;
            }

            get_opt_count = dhcp_get_opt(dhcp_reply, get_opt_count, get_option);
        }

        // Set the DHCP timers
        uint32_t timer1 = (uint32_t) dhcp_lease_time * 0.5;
        uint32_t timer2 = (uint32_t) dhcp_lease_time * 0.875;


    }

    return 0;
}

bool
dhcp_renew_timer(void)
{
    dhcp_state = renewing;
    return true;
}

bool
dhcp_rebind_timer(void)
{
    dhcp_state = rebinding;
    return true;
}

uint16_t
dhcp_daemon(uint16_t length, uint8_t* packet)
{
    uint16_t result;

    switch(dhcp_state) {
        case init:
            // Create the DHCPDISCOVER packet
            result = dhcp_discover(length, packet);
            // Return the result
            return result;
            break;

        case selecting:
            // Create the DHCPREQUEST packet
            result = dhcp_request(length, packet);
            // Return the result
            return result;
            break;

        case requesting:
            return dhcp_bind(length, packet);
            break;

        case bound:
            break;

        case renewing:
            break;

        case releasing:
            break;

        default:
            return;
    }
}

struct dhcp_header_t*
dhcp_create_header(struct dhcp_header_t* dhcp, uint32_t xid) {
    if(dhcp) {
        free(dhcp);
    }

    dhcp = NULL;

    dhcp = (struct dhcp_header_t*) malloc(sizeof(struct dhcp_header_t));

    dhcp->udp.src_port	= 67;
    dhcp->udp.dest_port	= 68;

    dhcp->op	= DHCP_BOOT_REQUEST;
    dhcp->htype	= 1;
    dhcp->hlen	= 6;
    dhcp->flags	= 0;
    dhcp->xid	= xid;
    dhcp->secs	= 0;
    dhcp->hops	= 0;

    memset(dhcp->ciaddr, 0, 4);
    memset(dhcp->yiaddr, 0, 4);
    memset(dhcp->siaddr, 0, 4);
    memset(dhcp->giaddr, 0, 4);
    memset(dhcp->chaddr, mac_get_host_addr(), 6);
    memset(dhcp->sname, 0, 64);
    memset(dhcp->file, 0, 128);

    return dhcp;
}