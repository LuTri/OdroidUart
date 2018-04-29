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

#ifndef BAUD
#define BAUD 500000UL
#endif

#define STATUS_BUFFER_SIZE 200

#ifndef UNITTEST
#include <util/setbaud.h>

#include <avr/io.h>
#else
#ifdef MOCKING
#include "mockserial/mockregisters.h"
#endif

// Mock registers and defines while in unittest
#ifndef MOCKING
typedef unsigned char uint8_t;
#endif
typedef unsigned long uint16_t;

extern uint8_t UCSR0A;
extern uint8_t UCSR0B;
extern uint8_t UCSR0C;
extern uint8_t UBRR0H;
extern uint8_t UBRR0L;
extern uint8_t UDR0;
// Control register A bits

#define RXC0 7
#define TXC0 6
#define UDRE0 5
#define FE0 4
#define DOR0 3
#define UPE0 2
#define U2X0 1
#define MPCM0 0

// Control register B bits

#define RXCIE0 7
#define TXCIE0 6
#define UDRIE0 5
#define RXEN0 4
#define TXEN0 3
#define UCSZ02 2
#define RXB80 1
#define TXB80 0

// Control registers C bits

#define UMSEL01 7
#define UMSEL00 6
#define UPM01 5
#define UPM00 4
#define USBS0 3
#define UCSZ01 2
#define UCSZ00 1
#define UCPOL0 0

// Mock BAUD values

#define UBRRH_VALUE 0xF0
#define UBRRL_VALUE 0x0F
#endif

/*! @brief Initialze the UART interface. */
void uart_setup(void);

/*! @brief Write a single character over UART.
 *
 * This is the non-blocking variant; the byte is only written if the interface
 * is currently available for transmission
 * @return @c **1** on success, **0** on failure. */
uint8_t uart_write_character(char character /*! payload */);

/*! @brief Write a complete string over UART.
 *
 * This is the non-blocking variant; the string is only written while is
 * currently available for transmission. When the interface becomes unavailable
 * inbetween, the transmission is aborted.
 * @return @c the amount of bytes written. */
uint16_t uart_write_string(char* string /*! payload */);

/*! @brief Read a character over UART.
 *
 * This is the non-blocking variant; a byte is only read if currently a byte is
 * available on the interface.
 * @return @c the byte read, or \0 if none was available. */
char uart_read_character(void);

/*! @brief Read a string over UART.
 *
 * Read a string over UART while bytes are available, the number of read bytes
 * doesn't exceed the buffer's size or a \0 is encountered.
 * @return @c the number of bytes read. */
uint16_t uart_read_string(
    char* buffer /*! buffer to write the characters to. */,
    uint16_t buffer_size /*! size of the buffer. */);

/*! @brief Write a single character over UART.
 *
 * This is the blocking variant; the function does not return until the payload
 * is written over UART. */
void uart_blocking_write_character(char character /*! payload */);

/*! @brief Write a complete string over UART.
 *
 * This is the blocking variant; the function does not return until the payload
 * is completely written over UART. */
void uart_blocking_write_string(char* string /*! payload */);

/*! @brief Read a character over UART
 *
 * This is the blocking variant; this will not return before a character was
 * transmitted over the UART interface.
 * @return @c the character read. */
char uart_blocking_read_character(void);

/*! @brief Read a string over UART.
 *
 * Read a string over UART while the number of read bytes doesn't exceed the
 * buffer's size or a \0 is encountered.
 * @return @c the number of bytes read. */
uint16_t uart_blocking_read_string(
    char* buffer /*! buffer to write the characters to. */,
    uint16_t buffer_size /*! size of the buffer. */);

extern uint8_t UART_STATI[STATUS_BUFFER_SIZE];
extern uint8_t uart_status_idx;

/* Utilities */

uint16_t fletchers_checksum(char* string);
#endif
