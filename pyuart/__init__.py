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

import io
import logging
import os
import serial
import time

from pyuart.utils import fletcher_checksum

logging.basicConfig(level=logging.INFO)


def benchmark(func):
    def function_timer(self, *args, **kwargs):
        start = time.time()
        value = func(self, *args, **kwargs)
        end = time.time()

        n_bytes = len(value)

        runtime = end - start
        if self.logger:  # pragma: no cover
            msg = 'The runtime for {:20s} completed with {:7.3f} Kb/s'
            self.logger.info(
                msg.format(func.__name__, (n_bytes / 1000.0) / runtime),
            )
        return value
    return function_timer


class UartError(IOError):
    pass


class UartOverflowError(UartError):
    def __init__(self, *args, **kwargs):
        super(UartOverflowError, self).__init__(
            "Transmitted data would exceed your MC's buffer!",
        )


class UartChecksumError(UartError):
    def __init__(self, *args, **kwargs):
        super(UartChecksumError, self).__init__(
            'Checksum check failed on the MC!',
        )


class UartUnknownError(UartError):
    def __init__(self, answer, *args, **kwargs):  # pragma: no cover
        super(UartUnknownError, self).__init__(
            'Received unknown Answer from MC: ' + answer.decode(),
        )


class UART(serial.Serial):
    _uart_h = os.path.join(os.path.dirname(__file__), '..', 'uart.h')

    def __new__(cls, *args, **kwargs):
        instance = super(UART, cls).__new__(cls, *args, **kwargs)
        defines = []

        # Read defines from uart.h
        with io.open(instance._uart_h, "rt") as c_header:
            raw = c_header.read().splitlines()
            defines = [x for x in raw if 'MSG_' in x or 'BUFFER_SIZE' in x]
        for define in defines:
            _, name, value = define.split(' ', 3)
            try:
                value = int(value)
            except ValueError:
                value = value.strip('"').encode()
            setattr(instance, name, value)
        return instance

    def __init__(self, port, baud, logger=None, *args, **kwargs):
        self.designated_port = port

        self.logger = logger

        self._connection = None
        super(serial.Serial, self).__init__(None, baud, *args, **kwargs)

    def set_port(self, port):
        self.designated_port = port

    def _connect(self):
        self.port = self.designated_port
        self.open()

    def __enter__(self):
        self._connect()
        return self

    def __exit__(self, *args):
        if self.isOpen():
            self.close()

    def prepare_header(self, data):
        length = len(data)
        cs = fletcher_checksum(data)
        header = bytearray()
        for x in ['DAT'.encode(),
                  (length >> 8, length & 0xff) + (cs >> 8, cs & 0xff)]:
            header += bytearray(x)
        return header

    @benchmark
    def write_data(self, data, force_ce=False):
        if len(data) > self.UART_BUFFER_SIZE:
            raise UartOverflowError
        header = self.prepare_header(data)

        if self.logger:
            self.logger.info('Header:')
            self.logger.info([hex(x) for x in bytearray(header)])

        if force_ce:
            data[0] = (data[0] + 1) % 255

        if self.logger:
            self.logger.info('Data:')
            self.logger.info([hex(x) for x in bytearray(data)])

        self.write(header + data)
        while self.out_waiting:  # pragma: no cover
            pass
        self.write(b'\0')

        answer = self.read(2)
        while self.in_waiting:  # pragma: no cover
            self.read(self.in_waiting)

        if answer == self.MSG_CHECKSUM_ERROR:
            raise UartChecksumError()
        elif answer == self.MSG_OK:
            pass
        else:  # pragma: no cover
            raise UartUnknownError(answer)

        return data
