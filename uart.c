/*
 * This file is part of the OdroiUart distribution
 * (https://github.com/LuTri/OdroiUart).
 * Copyright (c) 2016 Tristan Lucas.
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

#include "uart.h"

#ifdef UNITTEST
// Mock registers and defines while in unittest
uint8_t UCSR0B = 0;
uint8_t UCSR0C = 0;
uint8_t UBRR0H = 0;
uint8_t UBRR0L = 0;

#ifndef MOCKING
uint8_t UCSR0A = 0;
uint8_t UDR0 = 0;
#endif
#include <stdio.h>
#endif

#define FRAME_ERROR (1 << FE0)
#define DATAOVERRUN_ERROR (1 << DOR0)
#define PARITY_ERROR (1 << UPE0)

uint8_t UART_STATI[UART_BUFFER_SIZE] = {0};
uint8_t uart_status_idx = 0;

typedef uint8_t (*CONDITION_FNC)(void);

/* Evaluate UART status */
void uart_save_status(void) {
    UART_STATI[uart_status_idx] = UCSR0A;
    uart_status_idx = (uart_status_idx + 1) % UART_BUFFER_SIZE;
}

/* non-blocking checks */
uint8_t _can_send(void) { return UCSR0A & (1 << UDRE0); }

uint8_t has_incoming(void) { return UCSR0A & (1 << RXC0); }

/* blocking checks */
uint8_t _blocking_can_send(void) {
    while (!(_can_send())) {
    };
    return 1;
}

uint8_t _blocking_has_incoming(void) {
    while (!(has_incoming())) {
#ifdef MOCKING
//        fprintf(stderr, "\n## NO INCOMING DATA! ##");
#endif
    };
    return 1;
}

/* internal api */

uint8_t _uart_write_char(CONDITION_FNC condition, uint8_t character) {
    if ((*condition)()) {
        UDR0 = character;
#ifdef MOCKING
        read_next();
#endif
        return 1;
    }
    return 0;
}

void uart_prot_answer(const char* msg) {
    _uart_write_char(_blocking_can_send, msg[0]);
    _uart_write_char(_blocking_can_send, msg[1]);
}

void _answer_error(uint8_t status) {
    if (!(status & (FRAME_ERROR | DATAOVERRUN_ERROR | PARITY_ERROR))) {
        return;
    }

    if (status & DATAOVERRUN_ERROR) {
        uart_prot_answer(MSG_DATA_OVERRUN);
    } else if (status & PARITY_ERROR) {
        uart_prot_answer(MSG_PARITY_ERROR);
    } else {
        uart_prot_answer(MSG_FRAME_ERROR);
    }
}

char _uart_read_char(CONDITION_FNC condition) {
    if ((*condition)()) {
        uart_save_status();
        _answer_error(UART_STATI[uart_status_idx - 1]);
#ifdef MOCKING
        write_next();
#endif
        return UDR0;
    }
    return '\0';
}

void _fletcher(uint8_t* sum1, uint8_t* sum2, uint8_t data) {
    *sum1 = (*sum1 + data) % 255;
    *sum2 = (*sum1 + *sum2) % 255;
}

/* public api */
uint16_t fletchers_binary(uint8_t* data, uint16_t length) {
    uint8_t sum1 = 0, sum2 = 0;

    while (length--) {
        sum1 = (sum1 + *data++) % 255;
        sum2 = (sum1 + sum2) % 255;
    }

    return (sum1 << 8) | sum2;
}

void uart_setup(void) {
    // Set BAUD prescaler
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

    // Enable TX and RD lines
    UCSR0B |= (1 << TXEN0) | (1 << RXEN0);

    // Frame Format: Asynchron 8bit 1 stopbits even parity
    UCSR0C = (1 << UPM01) | (1 << UCSZ01) | (1 << UCSZ00);
#if USE_2X
    UCSR0A |= (1 << U2X0);
#else
    UCSR0A &= ~(1 << U2X0);
#endif
}

uint16_t _read_16_bit(CONDITION_FNC cond) {
    uint8_t first, second;
    first = _uart_read_char(cond);
    second = _uart_read_char(cond);
    return (first << 8) | second;
}

uint16_t uart_prot_read(uint8_t* buffer, uint16_t max_size, uint8_t* status) {
    /* frame format:
     * DATXXCSdddd...
     * DAT=header
     * XX -> 16 bit data-length
     * CS -> 16 bit checksum
     * ddd... -> XX-bytes of data */
    uint16_t in_size, idx;
    uint16_t in_checksum, cmp_checksum;
    const char* head = "DAT";
    uint8_t garbage;

    for (idx = 0; idx < 3; idx++) {
        if ((garbage = _uart_read_char(_blocking_has_incoming)) != head[idx]) {
            /* Garbage received, we might as well return */
            uart_prot_answer(MSG_GARBAGE);
            *status = 1;
            return 0;
        }
    }

    in_size = _read_16_bit(_blocking_has_incoming);
    in_checksum = _read_16_bit(_blocking_has_incoming);

    for (idx = 0; idx < in_size && idx < max_size; idx++) {
        buffer[idx % max_size] = _uart_read_char(_blocking_has_incoming);
    }

    cmp_checksum = fletchers_binary(buffer, in_size);
    if (cmp_checksum != in_checksum) {
        uart_prot_answer(MSG_CHECKSUM_ERROR);
        *status = 2;
        return 0;
    }

    uart_prot_answer(MSG_OK);
    *status = 0;
    return in_size;
}
