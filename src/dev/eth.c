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

#include "eth.h"
#include "spi.h"

// Global status variables
static uint8_t  eth_bank_pointer;
static uint16_t eth_packet_pointer;

/**
 * @function:   eth_enable
 * @brief:      Enables the ethernet controller
 */
inline void
eth_enable(void)
{
    ETH_RESET_PORT |= (1 << ETH_RESET_PIN);
}

/**
 * @function:   eth_enable
 * @brief:      disables the ethernet controller
 */
inline void
eth_disable(void)
{
    ETH_RESET_PORT &= ~(1 << ETH_RESET_PIN);
}

/**
 * @function:   eth_select
 * @brief:      Select the ethernet controller
 *              Enabling its SPI interface
 */
inline void
eth_select(void)
{
    ETH_SELECT_PORT &= ~(1 << ETH_SELECT_PIN);
}

/**
 * @function:   eth_deselect
 * @brief:      Deselect the ethernet controller
 *              Disabling its SPI interface
 */
inline void
eth_deselect(void)
{
    ETH_SELECT_PORT |= (1 << ETH_SELECT_PIN);
}

/**
 * @function:   eth_init
 * @param:      mac_address, hardware mac address.
 * @brief:      Used to initialise ethernet controller
 */
void
eth_init(uint8_t* mac_address)
{
    // Initialise SPI driver
    spi_init();

    // Init control lines
    ETH_SELECT_DDR |= (1 << ETH_SELECT_PIN);
    ETH_RESET_DDR |= (1 << ETH_RESET_PIN);

    // Enable controller
    eth_enable();

    // Disconnect controller from SPI bus
    eth_deselect();

    // Softreset the controller
    eth_write_opcode(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);

    // Check CLKRDY bit to see if reset is complete
    // while (!(eth_read_byte(ESTAT) & ESTAT_CLKRDY));
    // See Rev. B4 Silicon Errata point 2.
    _delay_ms(100);

    /**
     * Initialise BANK 0
     */

    // Initialize receive buffer
    eth_packet_pointer = ETH_REG_RX_START;

    // Controller RX buffer start address
    eth_write_byte(ERXSTL,   ETH_REG_RX_START & 0xFF);
    eth_write_byte(ERXSTH,   ETH_REG_RX_START >> 8);
    eth_write_byte(ERXRDPTL, ETH_REG_RX_START & 0xFF);
    eth_write_byte(ERXRDPTH, ETH_REG_RX_START >> 8);

    // Controller RX buffer stop address
    eth_write_byte(ERXNDL,   ETH_REG_RX_STOP & 0xFF);
    eth_write_byte(ERXNDH,   ETH_REG_RX_STOP >> 8);

    // Controller TX buffer start address
    eth_write_byte(ETXSTL,   ETH_REG_TX_START & 0xFF);
    eth_write_byte(ETXSTH,   ETH_REG_TX_START >> 8);

    // Controller TX buffer start address
    eth_write_byte(ETXNDL,   ETH_REG_TX_STOP & 0xFF);
    eth_write_byte(ETXNDH,   ETH_REG_TX_STOP >> 8);

    /**
     * Initialise BANK 1
     */

    /**
     * Packet filter:
     *	For broadcast packets we allow only ARP packtets
     *	All other packets should be unicast only for our mac(MAADR)
     *
     *	The pattern to match on is therefore
     *	Type     ETH.DST
     *	ARP      BROADCAST
     *	06 08 -- ff ff ff ff ff ff -> ip checksum for theses bytes=f7f9
     *	in binary these poitions are:11 0000 0011 1111
     *	This is hex 303F->EPMM0=0x3f,EPMM1=0x30
     */
    eth_write_byte(ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_PMEN);
    eth_write_byte(EPMM0,   0x3F);
    eth_write_byte(EPMM1,   0x30);
    eth_write_byte(EPMCSL,  0xF9);
    eth_write_byte(EPMCSH,  0xF7);

    /**
     * Initialise BANK 2
     */

    // Enable MAC receiver
    eth_write_byte(MACON1, MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);

    // Reset MAC receiver
    eth_write_byte(MACON2, 0x00);

    // Enable automatic padding to 60 bytes and CRC operations
    eth_write_opcode(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN);

    // Set inter-frame gap(non-back-to-back)
    eth_write_byte(MAIPGL, 0x12);
    eth_write_byte(MAIPGH, 0x0C);

    // Set inter-frame gap(back-to-back)
    eth_write_byte(MABBIPG, 0x12);

    // Set the maximum packet size which the controller will accept
    eth_write_byte(MAMXFLL, ETH_MAX_FRAME_LENGTH & 0xFF);
    eth_write_byte(MAMXFLH, ETH_MAX_FRAME_LENGTH >> 8);

    /**
     * Initialise BANK 3
     */

    // Set the hardware mac address
    eth_set_mac(mac_address);

    // No loopback of transmitted frames
    eth_write_phy(PHCON2, PHCON2_HDLDIS);

    // Switch to bank 0
    eth_set_bank(ECON1);

    // Enable interrutps
    eth_write_opcode(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE | EIE_PKTIE);

    // Enable packet reception
    eth_write_opcode(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
}

/**
 * @function:   eth_set_mac
 * @param:      mac_address, hardware mac address
 * @brief:      Sets the ethernet controller hardware address
 */
void eth_set_mac(uint8_t* mac_address)
{
    eth_write_byte(MAADR5, mac_address[0]);
    eth_write_byte(MAADR4, mac_address[1]);
    eth_write_byte(MAADR3, mac_address[2]);
    eth_write_byte(MAADR2, mac_address[3]);
    eth_write_byte(MAADR1, mac_address[4]);
    eth_write_byte(MAADR0, mac_address[5]);
}

/**
 * @function:   eth_set_bank
 * @param:      address, bank address to be set
 * @brief:      Sets the bank pointer
 */
void
eth_set_bank(uint8_t address)
{
    uint8_t address_masked = address & BANK_MASK;

    // Bank already active?
    if(address_masked != eth_bank_pointer) {
        // Set the bank
        eth_write_opcode(ENC28J60_BIT_FIELD_CLR, ECON1, (ECON1_BSEL1 | ECON1_BSEL0));
        eth_write_opcode(ENC28J60_BIT_FIELD_SET, ECON1, (address_masked) >> 5);

        // Update local bank pointer
        eth_bank_pointer = address_masked;
    }
}

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
void
eth_set_clock(uint8_t value)
{
    eth_write_byte(ECOCON, value & 0x7);
}

/**
 * @function:   eth_get_rx_packet_count
 * @return:     Amount of pending packets in the controller.
 * @brief:      Returns the amound of unprocessed packets stored
 *              in the ethernet controller.
 */
uint8_t
eth_get_rx_packet_count(void)
{
    return eth_read_byte(EPKTCNT);
}

/**
 * @function:   eth_get_link_status
 * @return:     The current status value of the ethernet link.
 * @brief:      Returns the current status of ethernet link.
 */
uint8_t
eth_get_link_status(void)
{
    return (eth_read_phy_h(PHSTAT2) && 4);
}

/**
 * @function:   eth_get_link_status
 * @return:     Core revision number.
 * @brief:      Returns the core revision numer of the ethernet controller.
 */
uint8_t
eth_get_revision(void)
{
    return eth_read_byte(EREVID);
}

/**
 * @function:   eth_read_byte
 * @param:      Register address to be read.
 * @return:     Byte of data read from the controller.
 * @brief:      Reads a byte of 8 bits from the ethernet controller.
 */
uint8_t
eth_read_byte(uint8_t address)
{
    // Set the bank
    eth_set_bank(address);

    // Read the data form controller
    return eth_read_opcode(ENC28J60_READ_CTRL_REG, address);
}

/**
 * @function:   eth_write_byte
 * @param:      Register address to be written.
 * @param:      Byte of data to be send to the controller.
 * @brief:      Writes a byte of 8 bits to the ethernet controller.
 */
void
eth_write_byte(uint8_t address, uint8_t data)
{
    // Set the bank
    eth_set_bank(address);

    // Write data to controller
    eth_write_opcode(ENC28J60_WRITE_CTRL_REG, address, data);
}

/**
 * @function:   eth_read_phy_h
 * @param:      PHY register address to be read.
 * @param:      Word of data read from the PHY register.
 * @brief:      Reads the high byte of a 16 bit PHY register located
 *              at the ethernet controller.
 */
uint16_t
eth_read_phy_h(uint8_t address)
{
    // Select register address
    eth_write_byte(MIREGADR, address);

    // Start the register read operation
    eth_write_byte(MICMD, MICMD_MIIRD);

    // Wait until the PHY read completes
    _delay_us(15);

    while(eth_read_byte(MISTAT) & MISTAT_BUSY);

    // Reset reading bit
    eth_write_byte(MICMD, 0x00);

    // Read the actual byte and return
    return(uint16_t)(eth_read_byte(MIRDH) << 8);
}

/**
 * @function:   eth_write_phy
 * @param:      PHY register address to be written.
 * @param:      Word of data to be send to the PHY register.
 * @brief:      Writes a word of 16 bits to the PHY register located
 *              at the ethernet controller.
 */
void
eth_write_phy(uint8_t address, uint16_t data)
{
    // Set the PHY register address
    eth_write_byte(MIREGADR, address);

    // Write the PHY data
    eth_write_byte(MIWRL, data);
    eth_write_byte(MIWRH, data >> 8);

    // Wait until the PHY write completes
    while(eth_read_byte(MISTAT) & MISTAT_BUSY) {
        _delay_us(15);
    }
}

/**
 * @function:   eth_read_opcode
 * @param:      Operation instruction code.
 * @param:      Involving register address.
 * @return:     Byte of data read from the controller.
 * @brief:      Write an operation to the controller following an
 *              8 bit read containing the result of the operation.
 */
uint8_t
eth_read_opcode(uint8_t opcode, uint8_t address)
{
    uint8_t result = 0;

    // Check if the SPI interface is avaiable
    spi_wait();

    // Select controller
    eth_select();

    // Issue read command
    spi_write_byte(opcode | (address & ADDR_MASK));

    // Read data
    result = spi_read_byte();

    // Do dummy read if needed(for mac and mii, see datasheet page 29)
    if(address & 0x80) {
        result = spi_read_byte();
    }

    // Deselect controller
    eth_deselect();

    return (result);
}

/**
 * @function:   eth_write_opcode
 * @param:      Operation instruction code.
 * @param:      Involving register address.
 * @param:      Byte of data to be send to the controller.
 * @brief:      Write an opcode to the controller.
 */
void
eth_write_opcode(uint8_t opcode, uint8_t address, uint8_t data)
{
    // Check if the SPI interface is avaiable
    spi_wait();

    // Select controller
    eth_select();

    // Issue write command
    spi_write_byte(opcode | (address & ADDR_MASK));

    // Write data
    spi_write_byte(data);

    // Deselect controller
    eth_deselect();
}

/**
 * @function:   eth_read_buffer
 * @param:      Lenght of data to be read from the memory.
 * @param:      Local data buffer to be written to.
 * @brief:      Reads a chunk of data from the memory located
 *              in the ethernet controller.
 */
void
eth_read_buffer(uint16_t length, uint8_t* data)
{
    // Check if the SPI interface is avaiable
    spi_wait();

    // Select controller
    eth_select();

    // Issue read command
    spi_write_byte(ENC28J60_READ_BUF_MEM);

    while(length--) {
        // Read data
        *data = spi_read_byte();

        // Increment data pointer
        data++;
    }

    // Add string delimiter
    *data = '\0';

    // Deselect controller
    eth_deselect();
}

/**
 * @function:   eth_write_buffer
 * @param:      Lenght of data to be written to the memory.
 * @param:      Local data buffer to be read from.
 * @brief:      Write a chunk of data to the memory located
 *              in the ethernet controller.
 */
void
eth_write_buffer(uint16_t length, uint8_t* data)
{
    // Check if the SPI interface is avaiable
    spi_wait();

    // Select controller
    eth_select();

    // Issue read command
    spi_write_byte(ENC28J60_WRITE_BUF_MEM);

    while(length--) {
        // Write data
        spi_write_byte(*data);

        // Increment data pointer
        data++;
    }

    // Deselect controller
    eth_deselect();
}

/**
 * @function:   eth_send_packet
 * @param:      Lenght of the packet to be send.
 * @param:      Local packet buffer to be read from.
 * @brief:      Sends an ethernet packet to the ethernet controller.
 */
void
eth_send_packet(uint16_t length, uint8_t* packet)
{
    // Check if transmit is in progress
    while(eth_read_opcode(ENC28J60_READ_CTRL_REG, ECON1) & ECON1_TXRTS) {
        // Reset the transmit logic problem. See Rev. B4 Silicon Errata point 12.
        if((eth_read_byte(EIR) & EIR_TXERIF)) {
            eth_write_opcode(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRST);
            eth_write_opcode(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRST);
        }
    }

    // Set the write pointer to start of transmit buffer area
    eth_write_byte(EWRPTL, ETH_REG_TX_START & 0xFF);
    eth_write_byte(EWRPTH, ETH_REG_TX_START >> 8);

    // Set the TXND pointer to correspond to the packet size given
    eth_write_byte(ETXNDL, (ETH_REG_TX_START + length) & 0xFF);
    eth_write_byte(ETXNDH, (ETH_REG_TX_START + length) >> 8);

    // Write per-packet control byte(0x00 means use macon3 settings)
    eth_write_opcode(ENC28J60_WRITE_BUF_MEM, 0, 0x00);

    // Copy the packet into the transmit buffer
    eth_write_buffer(length, packet);

    // Send the contents of the transmit buffer onto the network
    eth_write_opcode(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
}

/**
 * @function:   eth_receive_packet
 * @param:      Maximum lenght of the packet to be read.
 * @param:      Local packet buffer to be written to.
 * @brief:      Reads a pending ethernet packet from the ethernet controller.
 */
uint16_t
eth_receive_packet(uint16_t max_length, uint8_t* packet)
{
    uint16_t rxstatus = 0;
    uint16_t length = 0;

    // Check if a packet has been received and buffered
    if(eth_get_rx_packet_count() == 0) { // See Rev. B4 Silicon Errata point 6.
        return 0;
    }

    // Set the read pointer to the start of the received packet
    eth_write_byte(ERDPTL, (eth_packet_pointer & 0xFF));
    eth_write_byte(ERDPTH, (eth_packet_pointer) >> 8);

    // Read the next packet pointer
    eth_packet_pointer  = eth_read_opcode(ENC28J60_READ_BUF_MEM, 0);
    eth_packet_pointer |= eth_read_opcode(ENC28J60_READ_BUF_MEM, 0) << 8;

    // Read the packet length(see datasheet page 43)
    length  = eth_read_opcode(ENC28J60_READ_BUF_MEM, 0);
    length |= eth_read_opcode(ENC28J60_READ_BUF_MEM, 0) << 8;

    // Remove the CRC count
    length -= 4;

    // Read the receive status(see datasheet page 43)
    rxstatus  = eth_read_opcode(ENC28J60_READ_BUF_MEM, 0);
    rxstatus |= ((uint16_t) eth_read_opcode(ENC28J60_READ_BUF_MEM, 0)) << 8;

    // Limit retrieve length
    if(length > max_length - 1) {
        length = max_length - 1;
    }

    // Check CRC and symbol errors(see datasheet page 44, table 7-3):
    // The ERXFCON.CRCEN is set by default. Normally we should not
    // need to check this.
    if((rxstatus & 0x80) == 0) {
        // Invalid packet
        length = 0;
    } else {
        // Copy the packet from the receive buffer
        eth_read_buffer(length, packet);
    }

    // Move the RX read pointer to the start of the next received packet
    eth_write_byte(ERXRDPTL, (eth_packet_pointer & 0xFF));
    eth_write_byte(ERXRDPTH, (eth_packet_pointer >> 8));

    /* XXX: Two lines above can be deleted? */

    // See Rev. B4 Silicon Errata point 13.
    if(((eth_packet_pointer - 1) < ETH_REG_RX_START) || ((eth_packet_pointer - 1) > ETH_REG_RX_STOP)) {
        eth_write_byte(ERXRDPTL, (ETH_REG_RX_STOP) & 0xFF);
        eth_write_byte(ERXRDPTH, (ETH_REG_RX_STOP) >> 8);
    } else {
        eth_write_byte(ERXRDPTL, (eth_packet_pointer - 1) & 0xFF);
        eth_write_byte(ERXRDPTH, (eth_packet_pointer - 1) >> 8);
    }

    // Decrement the packet counter indicate we are done with this packet
    eth_write_opcode(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);

    return(length);
}
