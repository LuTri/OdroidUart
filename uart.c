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

uint8_t UART_STATI[STATUS_BUFFER_SIZE] = {0};
uint8_t uart_status_idx = 0;

typedef uint8_t (*CONDITION_FNC)(void);

/* Evaluate UART status */
void uart_save_status(void) {
    UART_STATI[uart_status_idx] = UCSR0A;
    uart_status_idx = (uart_status_idx + 1) % STATUS_BUFFER_SIZE;
}

/* non-blocking checks */
uint8_t _can_send(void) { return UCSR0A & (1 << UDRE0); }

uint8_t _has_incoming(void) { return UCSR0A & (1 << RXC0); }

/* blocking checks */
uint8_t _blocking_can_send(void) {
    while (!(_can_send())) {
    };
    return 1;
}

uint8_t _blocking_has_incoming(void) {
    while (!(_has_incoming())) {
    };
    return 1;
}

/* internal api */

uint8_t _uart_write_char(CONDITION_FNC condition, char character) {
    if ((*condition)()) {
        UDR0 = character;
#ifdef MOCKING
        read_next();
#endif
        return 1;
    }
    return 0;
}

uint16_t _uart_write_string(CONDITION_FNC condition, char* string) {
    uint16_t counter = 0;
    char* tmp = string;
    while (*tmp) {
        if (!_uart_write_char(condition, *tmp)) {
            return counter;
        }
        tmp++;
        counter++;
    }
    /* Send string end */
    return counter + _uart_write_char(condition, '\0');
}

char _uart_read_char(CONDITION_FNC condition) {
    if ((*condition)()) {
        uart_save_status();
#ifdef MOCKING
        write_next();
#endif
        return UDR0;
    }
    return '\0';
}

uint16_t _uart_read_string(CONDITION_FNC condition, char* buffer,
                           uint16_t buffer_size) {
    uint16_t checksum_binary;
    CONDITION_FNC write_cond;

    char* start = buffer;

    char cur;
    uint16_t idx = 0;

    if (condition == _blocking_has_incoming) {
        write_cond = _blocking_can_send;
    } else {
        write_cond = _can_send;
    }

    do {
        cur = _uart_read_char(condition);
        *buffer = cur;
        buffer++;
        idx++;
    } while (cur != '\0' && idx < buffer_size);

    /* Calculate checksum and write it to UART */
    checksum_binary = fletchers_checksum(start);

    _uart_write_char(write_cond, checksum_binary >> 8);
    _uart_write_char(write_cond, checksum_binary & 0xff);
    _uart_write_char(write_cond, '\0');

    /* return actually read num of bytes */
    return idx;
}

/* public api */
uint16_t fletchers_checksum(char* string) {
    uint8_t sum1 = 0, sum2 = 0;

    while (*string) {
        sum1 = (sum1 + (uint8_t)(*string++)) % 255;
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

uint8_t uart_write_character(char character) {
    return _uart_write_char(_can_send, character);
}

uint8_t uart_blocking_write_character(char character) {
    return _uart_write_char(_blocking_can_send, character);
}

uint16_t uart_write_string(char* string) {
    return _uart_write_string(_can_send, string);
}

uint16_t uart_blocking_write_string(char* string) {
    return _uart_write_string(_blocking_can_send, string);
}

char uart_read_character(void) { return _uart_read_char(_has_incoming); }

char uart_blocking_read_character(void) {
    return _uart_read_char(_blocking_has_incoming);
}

uint16_t uart_read_string(char* buffer, uint16_t buffer_size) {
    return _uart_read_string(_has_incoming, buffer, buffer_size);
}

uint16_t uart_blocking_read_string(char* buffer, uint16_t buffer_size) {
    return _uart_read_string(_blocking_has_incoming, buffer, buffer_size);
}
