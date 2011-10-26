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

#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "net/udp.h"
#include "net/mac.h"
#include "net/ip.h"
#include "lib/socket.h"
#include "lib/timer.h"

#ifndef _DHCP_H_
#define _DHCP_H_

// DHCP Opcodes
#define	DHCP_BOOT_REQUEST	1
#define DHCP_BOOT_REPLY		2

// The supported DHCP message types
#define DHCP_SUBNET_MASK	1
#define DHCP_ROUTER_ADDR	3
#define DHCP_DNS_SERVER		6
#define DHCP_REQUESTED_ADDR	50
#define DHCP_LEASE_TIME		51
#define DHCP_MESSAGE_TYPE	53
#define DHCP_SERVER_ADDR	54

// DHCP Message 53 values and type number
#define DHCP_DISCOVER			1
#define DHCP_REQUEST			2
#define DHCP_OFFER				3
#define DHCP_DECLINE			4
#define	DHCP_ACK				5
#define DHCP_NAK				6
#define DHCP_RELEASE			7
#define DHCP_INFORM				8
#define DHCP_FORCE_RENEW		9
#define DHCP_LEASE_QUERY		10
#define DHCP_LEASE_UNASSIGNED	11
#define DHCP_LEASE_UNKNOWN		12
#define	DHCP_LEASE_ACTIVE		13

// Byte locations
#define DHCP_OPT_TYPE			0
#define DHCP_OPT_LENGTH			1
#define DHCP_OPT_VALUE_START	2

struct dhcp_header_t {
    struct udp_header_t udp_header;

    uint8_t		op;
    uint8_t		htype;
    uint8_t		hlen;
    uint8_t		hops;

    uint32_t	xid;

    uint16_t	secs;
    uint16_t	flags;

    ip_addr_t	ciaddr;
    ip_addr_t	yiaddr;
    ip_addr_t	siaddr;
    ip_addr_t	giaddr;

    uint8_t		chaddr[16];

    uint8_t		sname[64];
    uint8_t		file[128];
};

typedef enum {
    init,
    selecting,
    requesting,
    init_reboot,
    rebooting,
    bound,
    renewing,
    rebinding
} dhcp_state_t;

extern uint16_t dhcp_discover(uint16_t length, uint8_t* packet);
extern uint16_t dhcp_request(uint16_t length, uint8_t* packet);
extern uint16_t dhcp_bind(uint16_t length, uint8_t* packet);
extern uint16_t dhcp_release(uint16_t length, uint8_t* packet);

extern uint16_t dhcp_daemon(uint16_t length, uint8_t* packet);
extern uint16_t dhcp_add_opt(struct dhcp_header_t* header, uint16_t opt_count, uint8_t* opt_value, uint8_t opt_length);
extern uint16_t dhcp_get_opt(struct dhcp_header_t* header, uint16_t opt_count, uint8_t* opt_value);

extern struct dhcp_header_t* dhcp_create_header(struct dhcp_header_t* header, uint32_t xid);

extern bool dhcp_renew_timer(void);
extern bool dhcp_rebind_timer(void);

#endif // !_DHCP_H_
