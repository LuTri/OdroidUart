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

#include <stdio.h>
#include <string.h>
#include "uart.h"

typedef char (*TEST_FUNC)(void);

/* forward definitions */
void print_test_beauty(const char* name);
void success_message(const char* name);
void failure_message(const char* name);

char compare_hex(uint16_t reg, uint16_t expected, const char* hint);
/*********************************
 * TEST FUNCTIONS *
 ********************************/

char test_setup_uart(void) {
    char result = 0;
    print_test_beauty(__FUNCTION__);

    uart_setup();

    result += compare_hex(UCSR0A, 0, "Setting up register UCSR0A");
    result += compare_hex(UCSR0B, 0x18, "Setting up register UCSR0B");
    result += compare_hex(UCSR0C, 0x26, "Setting up register UCSR0C");
    result += compare_hex(UBRR0H, 0xf0, "Setting up register UBRR0H");
    result += compare_hex(UBRR0L, 0xf, "Setting up register UBRR0L");

    if (result != 0) {
        failure_message(__FUNCTION__);
    } else {
        success_message(__FUNCTION__);
    }
    return result;
}

char test_binary_fletcher_checksum(void) {
    char result = 0;
    char hintbuffer[200];
    uint16_t checksum;
    int idx;

    const uint8_t data[][6] = {
        {0, 255, 36, 48, 91, 34},
        {0, 25, 64, 84, 0, 1},
        {0, 255, 36, 20, 93, 94},
    };
    uint16_t expected_checksums[] = {0xd1f9, 0xae7c, 0xf3e5};

    print_test_beauty(__FUNCTION__);
    for (idx = 0; idx < 3; idx++) {
        checksum = fletchers_binary((uint8_t*)data[idx], 6);
        sprintf(hintbuffer, "Checksum for data %d", idx);
        result += compare_hex(checksum, expected_checksums[idx], hintbuffer);
    }

    if (result != 0) {
        failure_message(__FUNCTION__);
    } else {
        success_message(__FUNCTION__);
    }
    return result;
}

/* Generate the list of testfunction, add function-names here */
TEST_FUNC* collect_tests(void) {
    static TEST_FUNC funcs[] = {
        test_setup_uart, test_binary_fletcher_checksum, NULL /* Array end */
    };
    return funcs;
}

/* Human readable test-state messages */
void print_test_beauty(const char* name) {
    printf("Running Test \"%s\"...\n", name);
}

void success_message(const char* name) {
    printf("             \"%s\"...Success!\n", name);
}

void failure_message(const char* name) {
    printf("             \"%s\"...Failed!\n", name);
}

/* comparison helpers */
char compare_hex(uint16_t value, uint16_t expected, const char* hint) {
    if (value != expected) {
        printf("Expected 0x%x, got 0x%x. Hint: \"%s\"\n", (unsigned)expected,
               (unsigned)value, hint);
        return 1;
    }
    return 0;
}

/* returns the number of failed tests */
int main(void) {
    int idx = 0;
    int success = 0;

    TEST_FUNC* funcs;
    TEST_FUNC func;

    funcs = collect_tests();

    while ((func = funcs[idx++]) != NULL) {
        success += (*func)();
    }

    if (success == 0) {
        printf("Awesome, all tests succeeded!\n");
    } else {
        printf("%d tests failed.\n", success);
    }

    return success;
}
