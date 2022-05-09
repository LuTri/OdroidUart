#!/usr/bin/env python3

import argparse
import json
import os
import sys

DEFAULTS = {
    'UART_RX_BUFFER_SIZE': 64,
    'UART_TX_BUFFER_SIZE': 8,
    'USART_LARGE_BUFFER': 0,
    'UART_PARITY_MODE': 0,
    'UART_STOP_BITS': 1,
    'UART_SYNC': 0,
    'UART_MASTER_SPI': 0,
    'UART_CHAR_SIZE': 8,
}


class Definition:
    def __init__(self, name, value=None, export=False):
        self._name = name
        self._value = value
        self._export = export

    def as_define(self):
        if self._name is None:
            return ''
        return '#define {} {}'.format(self._name, self._value)

    def as_json(self):
        if self._name is None or not self._export:
            return {}
        value = self._value
        if isinstance(value, str):
            value = value.replace('"', '')
        return {self._name: value}


ADDITIONAL_CONFIG = [
    Definition('CMD_SOUNDTOLIGHT', 0x01, True),
    Definition('CMD_SLAVE', 0x02, True),
    Definition('CMD_MOOD', 0x03, True),
    Definition('CMD_WHITE', 0x04, True),
    Definition('CMD_OFF', 0x05, True),
    Definition('CMD_SNAKE', 0x06, True),
    Definition(None),
    Definition('MAX_SNAKE_LENGTH', 4, True),
    Definition(None),
    Definition('MSG_CHECKSUM_ERROR', '"CE"', True),
    Definition('MSG_OK', '"OK"', True),
    Definition('MSG_RESET', '"RS"', True),
    Definition('MSG_PARITY_ERROR', '"PE"', True),
    Definition('MSG_BUFFER_OVERFLOW', '"BO"', True),
    Definition('MSG_DATA_OVERRUN', '"DO"', True),
    Definition('MSG_FRAME_ERROR', '"FE"', True),
    Definition('MSG_FRAME_UNFINISHED', '"UF"', True),
    Definition('MSG_ANSWER_START', '"ANS"', True),
    Definition(None),
    Definition('UART_SKIP_UNFINISHED', 20, False),
    Definition('UART_FLAG_RESET', 1, True),
    Definition('UART_FLAG_CHECKSUM_ERROR', 2, True),
    Definition('UART_FLAG_UNFINISHED', 4, True),
    Definition('UART_FLAG_VERIFIED', 8, True),
    Definition('UART_FLAG_DONE', 16, True),
    Definition(None),
    Definition('UART_HEADER_BYTES', '"DAT"', True),
    Definition('UART_DONE_BYTES', '"DONE"', True),
    Definition('UART_HEADER_SIZE', 3),
    Definition('UART_DONE_SIZE', 4),
    Definition(None),
    Definition('UART_CMD_SIZE', 1),
    Definition('UART_T_SIZE', 2),
    Definition('UART_CH_SIZE', 2),
    Definition(None),
    Definition('UART_PART_1', 'UART_HEADER_SIZE'),
    Definition('UART_PART_2', '(UART_PART_1 + UART_CMD_SIZE)'),
    Definition('UART_PART_3', '(UART_PART_2 + (UART_T_SIZE - 1))'),
    Definition('UART_PART_4', '(UART_PART_3 + 1)'),
    Definition('UART_PART_5', '(UART_PART_4 + (UART_CH_SIZE -1))'),
    Definition('UART_PART_6', '(UART_PART_5 + 1)'),
    Definition('UART_PART_7', '(UART_PART_6 + UART_DONE_SIZE)'),
]


def write(num, outfile, expansions):
    sys.stderr.write("Regenerating {}\n".format(outfile))
    with open(outfile, 'w') as fp:
        for expand, fmt in expansions:
            if expand:
                for x in range(0, num+1):
                    fp.write(fmt.format(num=x, **DEFAULTS))
            else:
                fp.write(fmt)


def control_meta(outfile=os.path.join(os.curdir, "control.meta")):
    data = {}
    for item in ADDITIONAL_CONFIG:
        data.update(item.as_json())
    with open(outfile, 'w') as fp:
        json.dump(data, fp)


def config_writer_meta(num, outfile=os.path.join(os.curdir, "writer.meta")):
    fmt = """\
USART{num}_ENABLED
UART_RX{num}_BUFFER_SIZE={UART_RX_BUFFER_SIZE}
UART_TX{num}_BUFFER_SIZE={UART_TX_BUFFER_SIZE}
USART{num}_LARGE_BUFFER={USART_LARGE_BUFFER}
UART{num}_SYNC={UART_SYNC}
UART{num}_MASTER_SPI={UART_MASTER_SPI}
UART{num}_PARITY_MODE={UART_PARITY_MODE}
UART{num}_STOP_BITS={UART_STOP_BITS}
UART{num}_CHAR_SIZE={UART_CHAR_SIZE}
"""
    expansions = [(True, fmt)]
    write(num, outfile, expansions)


def makefile_passings(num, outfile=os.path.join(os.curdir, "passings.mk")):
    passings = """\
ifdef USART{num}_ENABLED
\tCFLAGS += -DUSART{num}_ENABLED
\tPFLAGS += --USART{num}_ENABLED
endif

ifdef UART_RX{num}_BUFFER_SIZE
\tCFLAGS += -DUART_RX{num}_BUFFER_SIZE=$(UART_RX{num}_BUFFER_SIZE)
\tPFLAGS += --UART_RX{num}_BUFFER_SIZE $(UART_RX{num}_BUFFER_SIZE)
endif

ifdef UART_TX{num}_BUFFER_SIZE
\tCFLAGS += -DUART_TX{num}_BUFFER_SIZE=$(UART_TX{num}_BUFFER_SIZE)
\tPFLAGS += --UART_TX{num}_BUFFER_SIZE $(UART_TX{num}_BUFFER_SIZE)
endif

ifdef USART{num}_LARGE_BUFFER
\tCFLAGS += -DUSART{num}_LARGE_BUFFER=$(USART{num}_LARGE_BUFFER)
\tPFLAGS += --USART{num}_LARGE_BUFFER $(USART{num}_LARGE_BUFFER)
endif

ifdef UART{num}_SYNC
\tCFLAGS += -DUART{num}_SYNC=$(UART{num}_SYNC)
\tPFLAGS += --UART{num}_SYNC $(UART{num}_SYNC)
endif

ifdef UART{num}_MASTER_SPI
\tCFLAGS += -DUART{num}_MASTER_SPI=$(UART{num}_MASTER_SPI)
\tPFLAGS += --UART{num}_MASTER_SPI $(UART{num}_MASTER_SPI)
endif

ifdef UART{num}_PARITY_MODE
\tCFLAGS += -DUART{num}_PARITY_MODE=$(UART{num}_PARITY_MODE)
\tPFLAGS += --UART{num}_PARITY_MODE $(UART{num}_PARITY_MODE)
endif

ifdef UART{num}_STOP_BITS
\tCFLAGS += -DUART{num}_STOP_BITS=$(UART{num}_STOP_BITS)
\tPFLAGS += --UART{num}_STOP_BITS $(UART{num}_STOP_BITS)
endif

ifdef UART{num}_CHAR_SIZE
\tCFLAGS += -DUART{num}_CHAR_SIZE=$(UART{num}_CHAR_SIZE)
\tPFLAGS += --UART{num}_CHAR_SIZE $(UART{num}_CHAR_SIZE)
endif
"""
    write(num, outfile, [(True, passings)])


def setup_header(num, outfile=os.path.join(os.curdir, "basic", "setup.h")):
    head = """\
#ifndef _UART_SETUP_H
#define _UART_SETUP_H

#include "config.h"

#ifndef UNITTEST
#include <avr/io.h>
#include <util/setbaud.h>
#else
#include "../test/mockdefines.h"
#endif

#define uart_setup() uart0_setup()
"""
    tail = """
#endif /* ifndef _UART_SETUP_H */\
"""

    fmt = """
#ifdef USART{num}_ENABLED
/*! @brief Initialze the USART{num} interface. */
void uart{num}_setup(void);
#endif
"""
    expansions = [
        (False, head),
        (True, fmt),
        (False, tail),
    ]

    write(num, outfile, expansions)


def setup_source(num, outfile=os.path.join(os.curdir, "basic", "setup.c")):
    head = """\
#include "setup.h"
#include "print_config.h"
#ifndef UNITTEST
#endif

"""

    fmt = """\
#ifdef USART{num}_ENABLED
void uart{num}_setup(void) {{
    // Set BAUD prescaler
    UBRR{num}H = UBRRH_VALUE;
    UBRR{num}L = UBRRL_VALUE;

    // Enable TX and RD lines, enable UART RX interrupt
    UCSR{num}B |= UART{num}_EN;

    // Frame Format: Asynchron 8bit 1 stopbits even parity
    UCSR{num}C = (_{num}_UM_SEL1 << UMSEL{num}1) |
             (_{num}_UM_SEL0 << UMSEL{num}0) |
             (_{num}_PAR_M1 << UPM{num}1) |
             (_{num}_PAR_M0 << UPM{num}0) |
             (_{num}_STOP_B << USBS{num}) |
             (_{num}_CH_S2 << UCSZ{num}2) |
             (_{num}_CH_S1 << UCSZ{num}1) |
             (_{num}_CH_S0 << UCSZ{num}0);


#if USE_2X
    UCSR{num}A |= (1 << U2X0);
#else
    UCSR{num}A &= ~(1 << U2X0);
#endif
}}

#endif /* ifdef USART{num}_ENABLED */
"""
    expansions = [(False, head), (True, fmt)]
    write(num, outfile, expansions)


def print_config(num, outfile=os.path.join(os.curdir, "basic",
                                           "print_config.h")):
    cmt_head = """\
#ifndef _PRINT_CONFIG_H
#define _PRINT_CONFIG_H

#include "config.h"

"""
    cmt_tail = """\

#endif /* ifndef _PRINT_CONFIG_H */\
"""
    fmt_string = """\
#ifdef USART{num}_ENABLED

#if _{num}_UM_SEL1
#define _UMSEL{num}1 "UMSEL{num}1, "
#else /* if _{num}_UM_SEL1 */
#define _UMSEL{num}1 ""
#endif /* if _{num}_UM_SEL1 */

#if _{num}_UM_SEL0
#define _UMSEL{num}0 "UMSEL{num}0, "
#else /* if _{num}_UM_SEL0 */
#define _UMSEL{num}0 ""
#endif /* if _{num}_UM_SEL0 */

#if _{num}_PAR_M1
#define _UPM{num}1 "UPM{num}1, "
#else /* if _{num}_PAR_M1 */
#define _UPM{num}1 ""
#endif /* if _{num}_PAR_M1 */

#if _{num}_PAR_M0
#define _UPM{num}0 "UPM{num}0, "
#else /* if _{num}_PAR_M0 */
#define _UPM{num}0 ""
#endif /* if _{num}_PAR_M0 */

#if _{num}_STOP_B
#define _USBS{num} "USBS{num}, "
#else /* if _{num}_STOP_B */
#define _USBS{num} ""
#endif /* if _{num}_STOP_B */

#if _{num}_CH_S2
#define _UCSZ{num}2 "UCSZ{num}2, "
#else /* if _{num}_CH_S2 */
#define _UCSZ{num}2 ""
#endif /* if _{num}_CH_S2 */

#if _{num}_CH_S1
#define _UCSZ{num}1 "UCSZ{num}1, "
#else /* if _{num}_CH_S1 */
#define _UCSZ{num}1 ""
#endif /* if _{num}_CH_S1 */

#if _{num}_CH_S0
#define _UCSZ{num}0 "UCSZ{num}0, "
#else /* if _{num}_CH_S0 */
#define _UCSZ{num}0 ""
#endif /* if _{num}_CH_S0 */

#pragma message("UART{num} enabled with flags: " _UMSEL{num}1 _UMSEL{num}0 _UPM{num}1 _UPM{num}0 _USBS{num} _UCSZ{num}2 _UCSZ{num}1 _UCSZ{num}0)
#endif /* ifdef USART{num}_ENABLED */

"""
    expansions = [
        (False, cmt_head),
        (True, fmt_string),
        (False, cmt_tail),
    ]
    write(num, outfile, expansions)


def config(num, outfile=os.path.join(os.curdir, "basic", "config.h")):
    cmt_head = """\
#ifndef _UART_CONFIG_H
#define _UART_CONFIG_H

/* avr-uart configuration
 * see https://andygock.github.io/avr-uart-documentation/html/index.html
 */

/* Fallback to unnumbered UART_* config when no numbered UART is defined */
#ifndef UART_ENABLED
#ifndef USART0_ENABLED
#ifndef USART1_ENABLED
#ifndef USART2_ENABLED
#ifndef USART3_ENABLED

#define UART_ENABLED
#define USART0_ENABLED

#endif /* ifndef USART3_ENABLED */
#endif /* ifndef USART2_ENABLED */
#endif /* ifndef USART1_ENABLED */
#endif /* ifndef USART0_ENABLED */
#endif /* ifndef UART_ENABLED */

"""

    cmt_tail = """
#ifndef BAUD_TOL
#define BAUD_TOL 1
#endif

#ifndef BAUD
#ifndef UNITTEST
#error "BAUD NEEDS TO BE DEFINED!"
#else
#define BAUD 9600UL
#endif
#endif

#ifndef F_OSC
#ifndef UNITTEST
#error "F_OSC NEEDS TO BE DEFINED!"
#else
#define F_OSC 1000000UL
#endif
#endif

#define UART_CYCLES_PER_BIT (F_OSC / BAUD)
#define UART_CYCLES_PER_BYTE (UART_CYCLES_PER_BIT * (UART_CHAR_SIZE + UART_STOP_BITS))

#ifndef UART_RCX_IDLE_THRESHOLD
#define UART_RCX_IDLE_THRESHOLD 4
#endif

#if ((UART_CYCLES_PER_BYTE * UART_RCX_IDLE_THRESHOLD) > 0xffff)
#error "Configured UART_RCX_IDLE_THRESHOLD with current BAUD and F_OSC would overflow failed cpu cycle counter!"
#endif
#define UART_CPU_CYCLES_IDLE ((UART_CYCLES_PER_BYTE * UART_RCX_IDLE_THRESHOLD))


#ifndef BUFFER_STATUS_BYTES
#define BUFFER_STATUS_BYTES 0
#endif

/* OdroidUart protocol definitions */

{}

#endif /* ifndef _UART_CONFIG_H */\
"""
    cmt_tail = cmt_tail.format(
        os.linesep.join(entry.as_define() for entry in ADDITIONAL_CONFIG)
    )

    fmt_string = """\
#ifdef USART{num}_ENABLED
//#pragma message("Setting defaults for USART{num}")

#ifndef UART_RX{num}_BUFFER_SIZE
//#pragma message("Setting default for UART_RX{num}_BUFFER_SIZE = {UART_RX_BUFFER_SIZE}")
#define UART_RX{num}_BUFFER_SIZE {UART_RX_BUFFER_SIZE}UL
#endif /* ifndef UART_RX{num}_BUFFER_SIZE */

#ifndef UART_TX{num}_BUFFER_SIZE
//#pragma message("Setting default for UART_TX{num}_BUFFER_SIZE = {UART_TX_BUFFER_SIZE}")
#define UART_TX{num}_BUFFER_SIZE {UART_TX_BUFFER_SIZE}UL
#endif /* ifndef UART_TX{num}_BUFFER_SIZE */

#ifndef USART{num}_LARGE_BUFFER
//#pragma message("Setting default for USART{num}_LARGE_BUFFER = {USART_LARGE_BUFFER}")
#define USART{num}_LARGE_BUFFER {USART_LARGE_BUFFER}
#endif /* ifndef UART{num}_LARGE_BUFFER */

#ifndef UART{num}_PARITY_MODE
//#pragma message("Setting default for UART{num}_PARITY_MODE = {UART_PARITY_MODE}")
#define UART{num}_PARITY_MODE {UART_PARITY_MODE}
#endif /* UART{num}_PARITY_MODE */

#ifndef UART{num}_STOP_BITS
//#pragma message("Setting default for UART{num}_STOP_BITS = {UART_STOP_BITS}")
#define UART{num}_STOP_BITS {UART_STOP_BITS}
#endif /* UART{num}_STOP_BITS */

#ifndef UART{num}_SYNC
//#pragma message("Setting default for UART{num}_SYNC = {UART_SYNC}")
#define UART{num}_SYNC {UART_SYNC}
#endif /* UART{num}_SYNC */

#ifndef UART{num}_MASTER_SPI
//#pragma message("Setting default for UART{num}_MASTER_SPI = {UART_MASTER_SPI}")
#define UART{num}_MASTER_SPI {UART_MASTER_SPI}
#endif /* UART{num}_MASTER_SPI */

#ifndef UART{num}_CHAR_SIZE
//#pragma message("Setting default for UART{num}_CHAR_SIZE = {UART_CHAR_SIZE}")
#define UART{num}_CHAR_SIZE {UART_CHAR_SIZE}
#endif /* ifndef UART{num}_CHAR_SIZE */

#if UART{num}_CHAR_SIZE < 5 || UART{num}_CHAR_SIZE > 9
#error "UART{num}_CHAR_SIZE must be >= 5 and <= 9"
#endif /* if UART{num}_CHAR_SIZE < 5 || UART{num}_CHAR_SIZE > 9 */

#endif /* USART{num}_ENABLED */

"""
    cmt_parity = """\
/*
 * Calculate mode configured UARTn parity check operation.
 * Valid parity modes:
 * UARTn_PARITY_MODE:
 *      2 (even parity)
 *      1 (odd parity)
 *      0 (disabled)
 *
 * Defaults to disabled.
 */
 
"""
    fmt_parity = """\
#ifdef USART{num}_ENABLED
#define _{num}_PAR_M1 ((UART{num}_PARITY_MODE != 0) & 1)
#define _{num}_PAR_M0 ((UART{num}_PARITY_MODE == 1) & 1)
#endif /* USART{num}_ENABLED */

"""
    cmt_stops = """\
/*
 * Calculate mode configured UARTn stop bits. Valid options:
 * UART_STOP_BITS = {1,2}
 *
 * Defaults to 1.
 */
 
"""
    fmt_stops = """\
#ifdef USART{num}_ENABLED
#define _{num}_STOP_B ((UART{num}_STOP_BITS >> 1) & 1)
#endif /* ifdef USART{num}_ENABLED */

"""
    cmt_sync = """\
/*
 * Calculate necessary mode bits for UARTn synchronous ar asynchronous 
 * operation, as well as the target uC being an SPI master.
 *
 * Default to asynchronous operation.
 */
 
"""
    fmt_sync = """\
#ifdef USART{num}_ENABLED
#define _{num}_UM_SEL0 (((UART{num}_MASTER_SPI == 1) | (UART{num}_SYNC == 1)) & 1)
#define _{num}_UM_SEL1 ((UART{num}_MASTER_SPI == 1) & 1)
#endif /* ifdef USART{num}_ENABLED */

"""
    cmt_csize = """\
/*
 * Calculate necessary mode bits for configure character size.
 * Valid options:
 * UARTn_CHAR_SIZE = {5..9}
 * Defaults to 8.
 */
 
"""
    fmt_csize = """\
#ifdef USART{num}_ENABLED
#define _{num}_CH (UART{num}_CHAR_SIZE - 5)

#define _{num}_CH_S2 ((UART{num}_CHAR_SIZE == 9) & 1)
#define _{num}_CH_S0 ((_{num}_CH & 1) | _{num}_CH_S2)
#define _{num}_CH_S1 (((_{num}_CH & 0b10) >> 1) | _{num}_CH_S2)
#endif /* ifdef USART{num}_ENABLED */

"""
    cmt_ens = """\
/* Construct the explicit initialisation flags for UCSRnB */
"""
    fmc_ens = """\
#ifdef USART{num}_ENABLED
#define UART{num}_EN   ((1 << TXEN{num})|(1 << RXEN{num})|(1 << RXCIE{num}))
#endif /* ifdef USART{num}_ENABLED */
"""

    expansions = [
        (False, cmt_head),
        (True, fmt_string),
        (False, cmt_parity),
        (True, fmt_parity),
        (False, cmt_stops),
        (True, fmt_stops),
        (False, cmt_sync),
        (True, fmt_sync),
        (False, cmt_csize),
        (True, fmt_csize),
        (False, cmt_ens),
        (True, fmc_ens),
        (False, cmt_tail),
    ]
    write(num, outfile, expansions)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('UARTNUM', type=int)

    args = parser.parse_args()

    config(args.UARTNUM)
    setup_source(args.UARTNUM)
    print_config(args.UARTNUM)
    setup_header(args.UARTNUM)
    makefile_passings(args.UARTNUM)
    config_writer_meta(args.UARTNUM)
    control_meta()
