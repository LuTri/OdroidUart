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

from pyuart import UART


class CustomUProcess(subprocess.Popen):
    def __init__(self, *args, **kwargs):
        super(CustomUProcess, self).__init__(
            *args, stdout=subprocess.PIPE, stdin=subprocess.PIPE,
            **kwargs,
        )

    def press_return(self):
        self.stdin.write(b'\n')
        self.stdin.flush()

    def choke(self):
        self.stdin.write(b'x\n')
        self.stdin.flush()


@pytest.fixture
def uart_pty():
    subprocess.check_output(['make', 'clean'], cwd='mockserial')
    subprocess.check_output(['make'], cwd='mockserial')
    process = CustomUProcess(['mockserial/mock'])

    process.press_return()

    pty = process.stdout.readline().decode().strip()
    yield process, pty

    process.terminate()


@pytest.fixture
def messages():
    random.seed(time.time())
    messages = [bytes(bytearray(x for x in range(1, 256)))]
    messages += [bytes(bytearray([random.randint(1, 255)
                 for x in range(random.randint(1, 200))])) for x in range(10)]

    return messages


def test_mocker(uart_pty, messages):
    process, pty = uart_pty
    uart_obj = UART(pty, 500000, timeout=2)
    with uart_obj:
        process.press_return()
        result = uart_obj.read_whole()

        assert result == b'Hello Python!'

        try:
            for t in messages:  # test fletcher-checksum-echoing
                process.press_return()
                uart_obj.write_string(t)
            process.choke()
        except Exception as e:
            print('Error on:')
            print(t)
            raise e
