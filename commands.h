/*
 * This file is part of the OdroiUart distribution
 * (https://github.com/LuTri/OdroiUart).
 * Copyright (c) 2018 Tristan Lucas.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _ODROID_UART
#define _ODROID_UART

#include "basic/setup.h"

// Bytes send / received for initial echo test
#ifndef N_ECHO_TESTS
#define N_ECHO_TESTS 20
#endif

#include "../globals.h"

#ifndef UNITTEST
#include <avr/io.h>
#include <avr/interrupt.h>
#else
#include "test/mockdefines.h"
#endif

typedef struct {
    uint8_t status;
    uint8_t header[UART_HEADER_SIZE];
    uint8_t tail[UART_DONE_SIZE];
    uint16_t cur_byte;
    uint8_t cmd;
    uint16_t size;
    uint16_t checksum;
    uint8_t data[N_LEDS];
} COMMAND_BUFFER;

/*! @brief Calculate the checksum for a string.
 *
 * Refer to https://en.wikipedia.org/wiki/Fletcher%27s_checksum for
 * details on how the checksum is build..
 * @return @c the 16bit checksum of the payload. */
//uint16_t fletchers_binary(uint8_t* data, uint16_t length);

/*! @brief Check if there incoming bytes over UART.
 * @return @c **1** if bytes are available, **0** otherwise. */
//uint8_t has_incoming(void);

/*! @brief perfrom a full frame read
 *
 * Attempt to read an UART-Frame of the following format:
 * DATXXCSddd..
 * Where 'DAT' is the frame's beginning,
 * XX is the 16bit size of the frame's payload,
 * CS is the 16bit expected checksum of the frame's payload,
 * ddd... are the frame's payload.
 *
 * status will hold the following values, after the function finished:
 * **0** on success, **1** when garbage was received, **2** when checksum-check
 * failed.
 * @return @c **0** if the frame was errorneous, otherwise the number of
 * transfered bytes. */
//uint16_t uart_prot_read(uint8_t* buffer /*! buffer to hold the payload */,
//                        uint8_t* cmd /*! register to store a arbitrary command code in.*/,
//                        uint16_t max_size /*! size of the buffer */,
//                        uint8_t* status /*! register to store the status in */,
//                        INDICATOR fnc);
//
//uint16_t uart_prot_full_read(uint8_t* buffer /*! buffer to hold the payload */,
//                        uint8_t* cmd /*! register to store a arbitrary command code in.*/,
//                        uint16_t max_size /*! size of the buffer */,
//                        uint8_t* status /*! register to store the status in */,
//                        INDICATOR fnc);

COMMAND_BUFFER* get_next_command(void);

#endif
