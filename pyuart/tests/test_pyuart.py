# This file is part of the OdroiUart distribution
# (https://github.com/LuTri/OdroiUart).
# Copyright (c) 2016 Tristan Lucas.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, version 3.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

import subprocess

import time
import random

import pytest

from pyuart import UART, UartError, UartOverflowError, UartChecksumError


class CustomUProcess(subprocess.Popen):
    def __init__(self, *args, **kwargs):
        super(CustomUProcess, self).__init__(
            *args, stdout=subprocess.PIPE, stdin=subprocess.PIPE, **kwargs)

    def press_char(self, char=None):
        self.stdin.write(char + b'\n' if char else b'\n')
        self.stdin.flush()

    def choke(self):
        self.press_char(b'x')


@pytest.fixture(scope="session")
def uart_pty():
    uart_obj = UART(None, 500000, timeout=2, write_trailing=True)

    subprocess.check_output(['make', 'clean'], cwd='mockserial')
    subprocess.check_output(
        ['make',
         'UART_BUFFER_SIZE={}'.format(uart_obj.UART_BUFFER_SIZE)],
        cwd='mockserial',
    )

    process = CustomUProcess(['mockserial/mock'])

    process.press_char()

    uart_obj.set_port(process.stdout.readline().decode().strip())

    with uart_obj:
        yield process, uart_obj

    process.choke()
    process.terminate()


@pytest.fixture(scope="session")
def binary_messages(uart_pty):
    _, uart = uart_pty
    random.seed(time.time())
    m = [bytes(bytearray([random.randint(0, 255)
               for x in range(random.randint(1, uart.UART_BUFFER_SIZE))]))
         for x in range(1000)]

    return m


def test_protocol_write(uart_pty, binary_messages):
    process, uart = uart_pty
    try:
        for t in binary_messages:
            process.press_char(b'b')
            uart.write_data(t)
    except UartError as e:
        print('Error on:')
        print(t)
        raise e


def test_overflow_error(uart_pty):
    process, uart = uart_pty
    msg = bytearray([1] * (uart.UART_BUFFER_SIZE + 1))

    process.press_char(b'b')
    with pytest.raises(UartOverflowError):
        uart.write_data(msg)


def test_checksum_error(uart_pty):
    process, uart = uart_pty
    msg = bytearray([1] * (10))

    process.press_char(b'b')
    with pytest.raises(UartChecksumError):
        uart.write_data(msg, True)
