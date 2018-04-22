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

/* Let AVR-libc validate BAUD rate and error */
#define BAUD_TOL 1

#include "utils.h"
#ifndef BAUD
#define BAUD 500000UL
#endif

#define STATUS_BUFFER_SIZE 200

#include <util/setbaud.h>

#include <avr/io.h>

void uart_setup(void);

/* non-blocking interface */

uint8_t uart_write_character(char character);
uint16_t uart_write_string(char* string);

char uart_read_character(void);
uint16_t uart_read_string(char* buffer, uint16_t buffer_size);

/* blocking interface */

uint8_t uart_blocking_write_character(char character);
uint16_t uart_blocking_write_string(char* string);

char uart_blocking_read_character(void);
uint16_t uart_blocking_read_string(char* buffer, uint16_t buffer_size);

extern uint8_t UART_STATI[STATUS_BUFFER_SIZE];
extern uint8_t uart_status_idx;

/* Utilities */

uint16_t fletchers_checksum(char* string);
#endif
