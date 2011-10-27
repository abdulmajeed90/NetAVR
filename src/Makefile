##
# Copyright 2011 Roy van Dam <roy@8bit.cx>
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without modification, are
# permitted provided that the following conditions are met:
# 
#    1. Redistributions of source code must retain the above copyright notice, this list of
#       conditions and the following disclaimer.
# 
#    2. Redistributions in binary form must reproduce the above copyright notice, this list
#       of conditions and the following disclaimer in the documentation and/or other materials
#       provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ''AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Project configuration
# ---------------------
TARGET          = firmware
TARGET_PLATFORM = Atmel AVR
TARGET_MCU      = atmega32
TARGET_MCU_TAG  = m32
TARGET_CLOCK    = 16000000
TARGET_LFUSE    = 0x2F
TARGET_HFUSE    = 0xD9
TARGET_EFUSE    = 0xFF
TARGET_LOADER   = avrdude
TARGET_PORT     = /dev/tty.usbserial
TARGET_BOARD    = stk500v2

SOURCES =  main.c      \
           app/echo.c  \
           \
           dev/eth.c   \
           dev/spi.c   \
           dev/uart.c  \
           \
           lib/clock.c \
           lib/date.c  \
           lib/timer.c \
           lib/tty.c   \
           \
           net/arp.c   \
           net/icmp.c  \
           net/ip.c    \
           net/mac.c   \
           net/net.c   \
           net/queue.c \
           net/tcp.c   \
           net/udp.c   \
           net/util.c
          
OBJECTS = $(SOURCES:.c=.o)

INCLUDES = -I.
OPTIONS  = -DF_CPU=$(TARGET_CLOCK) \
               -DWITH_DEBUG=

PPFLAGS = -mmcu=$(TARGET_MCU)

EFLAGS  = -Wall -Werror

LDLIBS  =
LDFLAGS = $(PPFLAGS) $(EFLAGS),-nm,-Map=$(TARGET).map,--cref $(LDLIBS)

CC      = avr-gcc
CFLAGS  = $(PPFLAGS) $(EFLAGS) $(OPTIONS) $(INCLUDES) -pipe -O3 -c

OBJCOPY = avr-objcopy
OBJSIZE = avr-size

# Implicit suffix rules
# ---------------------
.SUFFIXES:
.SUFFIXES: .c .o .elf. rom

.c.o:
	@echo CC $@
	@$(CC) $(CFLAGS) -std=gnu99 $< -o $@

# Build rules
# ---------------------
.PHONY: all clean install
.PHONY: checkfuses burnfuses terminal

all: $(SOURCES) $(TARGET).rom

$(TARGET).rom: $(TARGET).elf
	@echo OBJCOPY $@
	@$(OBJCOPY) -O ihex $(TARGET).elf $(TARGET).rom
	@echo OBJSIZE $@
	@$(OBJSIZE) -t $(OBJECTS) $(TARGET).elf $(TARGET).rom

$(TARGET).elf: $(OBJECTS)
	@echo LD $@
	@$(CC) $(LDFLAGS) $(OBJECTS) -o $@

clean:
	rm -rf $(OBJECTS) $(TARGET).*

# Install: Write rom to the target device
install:
	$(TARGET_LOADER) -v -p $(TARGET_MCU_TAG) -c $(TARGET_BOARD) -P $(TARGET_PORT) -e -U flash:w:$(TARGET).rom

# Terminal: Open a connection with the STK500 terminal.
terminal:
	$(TARGET_LOADER) -v -p $(TARGET_MCU_TAG) -c $(TARGET_BOARD) -P $(TARGET_PORT) -t $(FLAGS)

# Fuses: make sure to uncomment the extended fuse byte when needed by the microcontroller.
checkfuses:
	$(TARGET_LOADER) -v -p $(TARGET_MCU_TAG) -c $(TARGET_BOARD) -P $(TARGET_PORT) \
	-U hfuse:v:$(TARGET_HFUSE):m -U lfuse:v:$(TARGET_LFUSE):m #-U efuse:v:$(TARGET_EFUSE):m

burnfuses:
	$(TARGET_LOADER) -v -p $(TARGET_MCU_TAG) -c $(TARGET_BOARD) -P $(TARGET_PORT) \
	-U hfuse:w:$(TARGET_HFUSE):m -U lfuse:w:$(TARGET_LFUSE):m #-U efuse:w:$(TARGET_EFUSE):m