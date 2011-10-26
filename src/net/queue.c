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

#include "queue.h"

/**
 * @function:   queue_packet
 * @param:      uint16_t, Length of the packet.
 * @param:      uint8_t *, Pointer to the first byte of the packet.
 * @return:     bool, Packet successfully queued.
 * @brief:      Queues a packet for delayed transmission until the
 *              necessary information for tranmssion has been collected.
 */
bool queue_packet(uint16_t length, uint8_t* packet)
{
    // XXX: Still to be implemented
    return false;
}

/**
 * @function:   unqueue_packet
 * @param:      queue_entry_t *, Packet queue table entry
 * @return:     bool, Packet successfully unqueued.
 * @brief:      unqueues a previously queued packet for transmission.
 */
bool
unqueue_packet(struct queue_entry_t* entry)
{
    // XXX: Still to be implemented
    return false;
}

/**
 * @function:   queue_get_table
 * @return:     queue_entry_t *, First entry of the queue table
 * @brief:      Returns the current queue table.
 */
struct queue_entry_t*
queue_get_table(void) {
    // XXX: Still to be implemented
    return NULL;
}

/**
 * @function:   queue_periodic
 * @brief:      Run this function periodicly to handle
 *              the transmission of unqueued packets. Be
 *              aware that this might take some time to
 *              complete. Therefor it is not wise to run
 *              this from an interrupt routine, forexample
 *              that of a hardware timer.
 */
void
queue_periodic(void)
{
    // XXX: Still to be implemented
}
