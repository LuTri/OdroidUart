#include "mockregisters.h"
#include <stdio.h>
#include <string.h>
#include "../uart.h"

uint8_t UCSR0A = (1 << UDRE0);
uint8_t UDR0 = 0;

uint8_t buff_in[BUFF_SIZE * 10] = {0};
uint8_t buff_out[BUFF_SIZE * 10] = {0};

int buff_in_idx = 0;
int buff_out_idx = 0;

int buff_in_size = 0;
int buff_out_size = 0;

void read_next(void) { buff_in[buff_in_idx++] = UDR0; }

void write_next(void) {
    UDR0 = buff_out[buff_out_idx++];
    if (buff_out_idx == buff_out_size) {
        buff_out_idx = 0;
        UCSR0A &= ~(1 << RXC0);
    }
}

void write_to_buff(char* data) {
    int idx = 0;

    UCSR0A |= (1 << RXC0);
    while (*data != '\0') {
        buff_out[idx++] = *data++;
    }
    buff_out[idx] = '\0';
    buff_out_size = idx + 1;
    buff_out_idx = 0;
}

void read_from_buff(char* dst) {
    int idx;
    for (idx = 0; idx < buff_in_idx; idx++) {
        dst[idx] = buff_in[idx];
    }
    buff_in_idx = 0;
}

void write_to_buff_binary(uint8_t* data, int size) {
    int idx = 0;
    UCSR0A |= (1 << RXC0);
    buff_out_size = size;
    while (size--) {
        buff_out[idx++] = *data++;
    }
    buff_out_idx = 0;
}
