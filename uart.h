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

#define MSG_CHECKSUM_ERROR "CE"
#define MSG_OK "OK"
#define MSG_GARBAGE "GB"
#define MSG_PARITY_ERROR "PE"
#define MSG_DATA_OVERRUN "DO"
#define MSG_FRAME_ERROR "FE"

#define UART_BUFFER_SIZE 500

/*! @brief Initialze the UART interface. */
void uart_setup(void);

extern uint8_t UART_STATI[UART_BUFFER_SIZE];
extern uint8_t uart_status_idx;

/*! @brief Calculate the checksum for a string.
 *
 * Refer to https://en.wikipedia.org/wiki/Fletcher%27s_checksum for
 * details on how the checksum is build..
 * @return @c the 16bit checksum of the payload. */
uint16_t fletchers_binary(uint8_t* data, uint16_t length);

/*! @brief Check if there incoming bytes over UART.
 * @return @c **1** if bytes are available, **0** otherwise. */
uint8_t has_incoming(void);

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
uint16_t uart_prot_read(uint8_t* buffer /*! buffer to hold the payload */,
                        uint16_t max_size /*! size of the buffer */,
                        uint8_t* status /*! register to store the status in */);
#endif
