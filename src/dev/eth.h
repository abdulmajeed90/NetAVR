/**
 * Copyright 2011 Roy van Dam <roy@8bit.cx>
 * Based on the enc28j60 driver from the AVRlib library by Pascal Stang
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

#include <avr/io.h>
#include <util/delay.h>

#include "enc28j60.h"

#ifndef _ETH_H_
#define _ETH_H_

// Controller select settings
#define ETH_SELECT_DDR  DDRB
#define ETH_SELECT_PORT PORTB
#define ETH_SELECT_PIN  PORTB2

// Controller select settings
#define ETH_RESET_DDR  DDRB
#define ETH_RESET_PORT PORTB
#define ETH_RESET_PIN  PORTB3

// Maximum frame lenght the driver will accept
#define ETH_MAX_FRAME_LENGTH 1500

// Ethernet RX/TX buffer memory map
#define ETH_REG_RX_START (0x0000)
#define ETH_REG_RX_STOP  (0x1FFF - 0x0600 - 1)
#define ETH_REG_TX_START (0x1FFF - 0x0600)
#define ETH_REG_TX_STOP  (0x1FFF)

/**
 * @function:   eth_enable
 * @brief:      Enables the ethernet controller
 */
extern void eth_enable(void);

/**
 * @function:   eth_enable
 * @brief:      disables the ethernet controller
 */
extern void eth_disable(void);

/**
 * @function:   eth_select
 * @brief:      Select the ethernet controller
 *              Enabling its SPI interface
 */
extern void eth_select(void);

/**
 * @function:   eth_deselect
 * @brief:      Deselect the ethernet controller
 *              Disabling its SPI interface
 */
extern void eth_deselect(void);

/**
 * @function:   eth_init
 * @param:      mac_address, hardware mac address.
 * @brief:      Used to initialise ethernet controller
 */
extern void eth_init(uint8_t* mac_address);

/**
 * @function:   eth_set_mac
 * @param:      mac_address, hardware mac address
 * @brief:      Sets the ethernet controller hardware address
 */
extern void eth_set_mac(uint8_t* mac_address);

/**
 * @function:   eth_set_bank
 * @param:      address, bank address to be set
 * @brief:      Sets the bank pointer
 */
extern void eth_set_bank(uint8_t address);

/**
 * @function:   eth_set_clock
 * @param:      value, prescaler factor
 * @brief:      Sets the prescaler factor for
 *              the clock output generated in
 *              the ethernet controller.
 *
 *              0 = Output disabled
 *              1 = 25    Mhz
 *              2 = 12.5  Mhz
 *              4 = 6.25  Mhz
 *              8 = 3.125 Mhz
 */
extern void eth_set_clock(uint8_t value);

/**
 * @function:   eth_get_rx_packet_count
 * @return:     Amount of pending packets in the controller.
 * @brief:      Returns the amound of unprocessed packets stored
 *              in the ethernet controller.
 */
extern uint8_t eth_get_rx_packet_count(void);

/**
 * @function:   eth_get_link_status
 * @return:     The current status value of the ethernet link.
 * @brief:      Returns the current status of ethernet link.
 */
extern uint8_t eth_get_link_status(void);

/**
 * @function:   eth_get_link_status
 * @return:     Core revision number.
 * @brief:      Returns the core revision numer of the ethernet controller.
 */
extern uint8_t eth_get_revision(void);

/**
 * @function:   eth_read_byte
 * @param:      Register address to be read.
 * @return:     Byte of data read from the controller.
 * @brief:      Reads a byte of 8 bits from the ethernet controller.
 */
extern uint8_t eth_read_byte(uint8_t address);

/**
 * @function:   eth_write_byte
 * @param:      Register address to be written.
 * @param:      Byte of data to be send to the controller.
 * @brief:      Writes a byte of 8 bits to the ethernet controller.
 */
extern void eth_write_byte(uint8_t address, uint8_t data);

/**
 * @function:   eth_read_phy_h
 * @param:      PHY register address to be read.
 * @param:      Word of data read from the PHY register.
 * @brief:      Reads the high byte of a 16 bit PHY register located
 *              at the ethernet controller.
 */
extern uint16_t eth_read_phy_h(uint8_t address);

/**
 * @function:   eth_write_phy
 * @param:      PHY register address to be written.
 * @param:      Word of data to be send to the PHY register.
 * @brief:      Writes a word of 16 bits to the PHY register located
 *              at the ethernet controller.
 */
extern void eth_write_phy(uint8_t address, uint16_t data);

/**
 * @function:   eth_read_opcode
 * @param:      Operation instruction code.
 * @param:      Involving register address.
 * @return:     Byte of data read from the controller.
 * @brief:      Write an operation to the controller following an
 *              8 bit read containing the result of the operation.
 */
extern uint8_t eth_read_opcode(uint8_t opcode, uint8_t address);

/**
 * @function:   eth_write_opcode
 * @param:      Operation instruction code.
 * @param:      Involving register address.
 * @param:      Byte of data to be send to the controller.
 * @brief:      Write an opcode to the controller.
 */
extern void eth_write_opcode(uint8_t opcode, uint8_t address, uint8_t data);

/**
 * @function:   eth_read_buffer
 * @param:      Lenght of data to be read from the memory.
 * @param:      Local data buffer to be written to.
 * @brief:      Reads a chunk of data from the memory located
 *              in the ethernet controller.
 */
extern void eth_read_buffer(uint16_t length, uint8_t* data);

/**
 * @function:   eth_write_buffer
 * @param:      Lenght of data to be written to the memory.
 * @param:      Local data buffer to be read from.
 * @brief:      Write a chunk of data to the memory located
 *              in the ethernet controller.
 */
extern void eth_write_buffer(uint16_t length, uint8_t* data);

/**
 * @function:   eth_send_packet
 * @param:      Lenght of the packet to be send.
 * @param:      Local packet buffer to be read from.
 * @brief:      Sends an ethernet packet to the ethernet controller.
 */
extern void eth_send_packet(uint16_t length, uint8_t* packet);

/**
 * @function:   eth_receive_packet
 * @param:      Maximum lenght of the packet to be read.
 * @param:      Local packet buffer to be written to.
 * @brief:      Reads a pending ethernet packet from the ethernet controller.
 */
extern uint16_t eth_receive_packet(uint16_t max_length, uint8_t* packet);

/* !_ETH_H_ */
#endif
