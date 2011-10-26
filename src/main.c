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

#include <stdio.h>

#include <avr/pgmspace.h>

#include "dev/eth.h"

#include "lib/clock.h"
#include "lib/timer.h"
#include "lib/date.h"
#include "lib/tty.h"
#include "lib/ctrl.h"

#include "net/net.h"
#include "net/mac.h"
#include "net/udp.h"
#include "net/ip.h"

#include "app/echo.h"

// Ethernet MAC address
mac_addr_t mac_address = {0x54, 0x55, 0x58, 0x10, 0x00, 0x24};

// Ethernet IP address
ip_addr_t ip_address = {10, 0, 1, 30};
ip_mask_t netmask = {255, 255, 225, 0};
ip_addr_t default_router = {10, 0, 1, 1};

#ifndef DEBUG
bool
display_status(void)
{
    static uint32_t bytes_sent;
    static uint32_t bytes_received;
    static uint32_t	rate;

    // Clear screen
    putchar(CTRL(FF));

    struct datetime_t datetime;
    time_decode(&datetime, clock_time());

    printf_P(PSTR("Time: %02d:%02d:%02d\n\n"), datetime.hour, datetime.min, datetime.sec);

    const struct net_status_t* net_status;
    net_status = net_get_status();

    printf_P(PSTR("Network connection:\n"));
    printf_P(PSTR(" Link state: [ %s ]\n\n"), (net_status->link) ? "UP" : "DOWN");

    printf_P(PSTR(" Packets sent: %lu\n"), net_status->packets_sent);

    printf_P(PSTR(" Data sent: "));

    if(net_status->bytes_sent < 1024) {
        printf_P(PSTR("%lu Bytes\n"), net_status->bytes_sent);
    } else if(net_status->bytes_sent < 1048576) {
        printf_P(PSTR("%lu kB\n"), net_status->bytes_sent / 1024);
    } else {
        printf_P(PSTR("%lu MB\n"), net_status->bytes_sent / 1048576);
    }

    rate = net_status->bytes_sent - bytes_sent;

    printf_P(PSTR(" Data rate: "));

    if(rate < 1024) {
        printf_P(PSTR("%lu B/s\n\n"), rate);
    } else if(rate < 1048576) {
        printf_P(PSTR("%lu kB/s\n\n"), rate / 1024);
    } else {
        printf_P(PSTR("%lu MB/s\n\n"), rate / 1048576);
    }

    bytes_sent = net_status->bytes_sent;


    printf_P(PSTR(" Packets received: %lu\n"), net_status->packets_received);

    printf_P(PSTR(" Data received: "));

    if(net_status->bytes_received < 1024) {
        printf_P(PSTR("%lu Bytes\n"), net_status->bytes_received);
    } else if(net_status->bytes_received < 1048576) {
        printf_P(PSTR("%lu kB\n"), net_status->bytes_received / 1024);
    } else {
        printf_P(PSTR("%lu MB\n"), net_status->bytes_received / 1048576);
    }

    rate = net_status->bytes_received - bytes_received;

    printf_P(PSTR(" Data rate: "));

    if(rate < 1024) {
        printf_P(PSTR("%lu B/s\n"), rate);
    } else if(rate < 1048576) {
        printf_P(PSTR("%lu kB/s\n"), rate / 1024);
    } else {
        printf_P(PSTR("%lu MB/s\n"), rate / 1048576);
    }

    bytes_received = net_status->bytes_received;

    return true;
}
#endif

int
main(void)
{
    // Initialise system clock
    clock_init();

    // Initialise serial communication
    tty_init(115200UL);

    // Initialise timers
#ifndef DEBUG
    timer_set(display_status, 1);
#endif

    printf("----------------------------------------------\n");
    printf("Copyright (c) 2011 - Roy van Dam <roy@8bit.cx>\n");
    printf("NetAVR 0.1-CURRENT (%s %s)\n", __TIME__, __DATE__);
    printf("----------------------------------------------\n\n");

    // Initialise ethernet controller
    eth_init(mac_address);

    // Initialise network stack
    net_init(mac_address, ip_address, netmask, default_router);

    // Bind UDP daemons
    udp_bind(7, echo_udp);  // Echo server

    while(true) {
        // Handle network traffic
        net_periodic();

        // Handle expired timers
        timer_periodic();
    }

    return 0;
}
