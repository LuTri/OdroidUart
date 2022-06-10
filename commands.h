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

#define _N_OVERHEAD 2  // e.g. extra parity bits

#define T_BIT (1.0 / BAUD)
#define T_BAUD (T_BIT * (8.0 + _N_OVERHEAD))

#define N_CYCLES_BETWEEN (F_CPU / BAUD)

#define T_CMD_MIN (UART_PART_7 * T_BAUD)
#define T_CMD_MIN_US (T_CMD_MIN * 1000.0)

/* approximately 0.24ms @ BAUD 500000
 *               0.48ms @ BAUD 250000 */
#define T_CMD_MAX ((UART_PART_7 + N_LEDS) * T_BAUD)
#define T_CMD_MAX_US (T_CMD_MAX * 1000.0)
/* approximately 6.95ms @ BAUD 500000
 *              13.92ms @ BAUD 250000 */

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

COMMAND_BUFFER* get_next_command(uint16_t* error_counter);

typedef void (FUNCTIONALITY_LAUNCHER)(COMMAND_BUFFER* command);

#endif
