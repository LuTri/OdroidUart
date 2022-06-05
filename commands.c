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

#include <stdlib.h>
#include "commands.h"
#include "basic/config.h"
#include "basic/fletcher.h"
#include "avr-uart/uart.h"

COMMAND_BUFFER _uart_buf1 = {
    .status = UART_FLAG_UNFINISHED,
    .cur_byte = 0,
    .cmd = 0,
    .size = 0,
    .checksum = 0,
    .data = {0}
};
COMMAND_BUFFER _uart_buf2 = {
    .status = UART_FLAG_UNFINISHED,
    .cur_byte = 0,
    .cmd = 0,
    .size = 0,
    .checksum = 0,
    .data = {0}
};

COMMAND_BUFFER* current_buffer = &_uart_buf1;
COMMAND_BUFFER* other_buffer = &_uart_buf2;

void verify_framebuffer(void) {
    if (fletchers_checksum(current_buffer->data, current_buffer->size)
        != current_buffer->checksum) {
        current_buffer->status = UART_FLAG_CHECKSUM_ERROR;
    } else {
        current_buffer->status = UART_FLAG_VERIFIED;
    }
}

uint8_t push_framebuffer(uint16_t available) {
    uint8_t byte;
    uint16_t current;

    for (uint16_t idx = 0; idx < available; idx++) {
        byte = uart0_getc();

        current = current_buffer->cur_byte++;

        if (current < UART_PART_1) {
            current_buffer->header[current & 0xff] = byte;
            if (current_buffer->header[current & 0xff] != UART_HEADER_BYTES[current & 0xff]) {
                current_buffer->status = UART_FLAG_RESET;
                current_buffer->cur_byte = 0;
                continue;
            }
            current_buffer->status = UART_FLAG_UNFINISHED;
        } else if (current < UART_PART_2) {
            current_buffer->cmd = byte;
        } else if (current < UART_PART_3) {
            current_buffer->size = (byte << 8);
        } else if (current < UART_PART_4) {
            current_buffer->size |= byte;
        } else if (current < UART_PART_5) {
            current_buffer->checksum = (byte << 8);
        } else if (current < UART_PART_6) {
            current_buffer->checksum |= byte;
        } else if (current < UART_PART_6 + current_buffer->size) {
            current_buffer->data[current - UART_PART_6] = byte;
        } else if (current < UART_PART_7 + current_buffer->size) {
            uint8_t t_byte = (current - (UART_PART_6 + current_buffer->size)) & 0xff;
            if (UART_DONE_BYTES[t_byte] != byte) {
                current_buffer->status = UART_FLAG_RESET;
                current_buffer->cur_byte = 0;
                continue;
            }
        }

        if (current == (UART_PART_7 + current_buffer->size) - 1) {
            current_buffer->status = UART_FLAG_DONE;
            current_buffer->cur_byte = 0;
            break;
        }
    }

    if (current_buffer->status & UART_FLAG_DONE) {
        verify_framebuffer();
    }
    return current_buffer->status;
}

COMMAND_BUFFER* _swap_framebuffer_ptr(void) {
    COMMAND_BUFFER* _tmp = current_buffer;
    current_buffer = other_buffer;
    other_buffer = _tmp;
    return _tmp;
}

COMMAND_BUFFER* get_next_command(uint16_t* error_counter) {
    static uint8_t unfinished_flags_idx = 0;
    uint8_t frame_status = 0;
    uint16_t available = uart0_available();

    if (available & UART_NO_DATA) return 0;

    if (available & UART_OVERRUN_ERROR) {
        *error_counter += 1;
        uart0_puts(MSG_ANSWER_START);
        uart0_puts(MSG_DATA_OVERRUN);
    } else if (available & UART_BUFFER_OVERFLOW) {
        *error_counter += 1;
        uart0_puts(MSG_ANSWER_START);
        uart0_puts(MSG_BUFFER_OVERFLOW);
    } else if (available & UART_FRAME_ERROR) {
        *error_counter += 1;
        uart0_puts(MSG_ANSWER_START);
        uart0_puts(MSG_FRAME_ERROR);
    } else if (available > 0) {
        frame_status = push_framebuffer(available);
        if (EXPLICIT(frame_status, UART_FLAG_UNFINISHED)) {
            if (unfinished_flags_idx++ < UART_IGNORE_EARLY_POLLS) {
                return 0;
            } else {
                unfinished_flags_idx = 0;
            }
        }
        uart0_puts(MSG_ANSWER_START);
        switch(frame_status) {
            case UART_FLAG_VERIFIED:
                uart0_puts(MSG_OK);
                break;
            case UART_FLAG_CHECKSUM_ERROR:
                *error_counter += 1;
                uart0_puts(MSG_CHECKSUM_ERROR);
                break;
            case UART_FLAG_UNFINISHED:
                uart0_puts(MSG_FRAME_UNFINISHED);
                break;
            case UART_FLAG_RESET:
                *error_counter += 1;
                uart0_puts(MSG_RESET);
                break;
            default:
                break;
        }
        if (frame_status & UART_FLAG_VERIFIED) {
            return _swap_framebuffer_ptr();
        }
    }
    return 0;
}