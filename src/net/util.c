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

#include "util.h"

/**
 * @function:   htons
 * @param:      Hostorder data
 * @return:     Networkorder data
 * @brief:      Convert 16 bit quantities between network
 *              byte order and host byte order.
 */
inline uint16_t
htons(uint16_t hostshort)
{
    return ((((hostshort) >> 8) & 0xff) | (((hostshort) & 0xff) << 8));
}

/**
 * @function:   htonl
 * @param:      Hostorder data
 * @return:     Networkorder data
 * @brief:      Convert 32 bit quantities between network
 *              byte order and host byte order.
 */
inline uint32_t
htonl(uint32_t hostlong)
{
    return ((((hostlong) & 0xff000000) >> 24) | (((hostlong) & 0x00ff0000) >>  8) |
            (((hostlong) & 0x0000ff00) <<  8) | (((hostlong) & 0x000000ff) << 24));
}
