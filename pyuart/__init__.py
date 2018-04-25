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

import logging
import serial
import struct
import time

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


class UART(serial.Serial):
    def __init__(self, port, baud, logger=None, *args, **kwargs):
        self.designated_port = port

        self.logger = logger

        self._connection = None
        super(serial.Serial, self).__init__(None, baud, *args, **kwargs)

    def _connect(self):
        self.port = self.designated_port
        self.open()

    def __enter__(self):
        self._connect()
        return self

    def __exit__(self, *args):
        if self.isOpen():
            self.close()

    def string_to_uint8_array(self, string):
        return struct.unpack('B' * len(string), string)

    def fletcher_checksum(self, string):
        sum1 = sum2 = 0

        values = self.string_to_uint8_array(string)
        for x in values:
            sum1 = (sum1 + x) % 255
            sum2 = (sum2 + sum1) % 255

        return (sum1 << 8) | sum2

    @benchmark
    def write_string(self, string):
        expected_checksum = self.fletcher_checksum(string)
        self.write(string)
        while self.out_waiting:  # pragma: no cover
            pass

        self.write(b'\0')  # Signal string.end
        while self.out_waiting:  # pragma: no cover
            pass

        gotten_checksum = self.string_to_uint8_array(self.read(2))
        self.read(1)  # ommit string end
        try:
            gotten_checksum = (gotten_checksum[0] << 8) | gotten_checksum[1]
        except IndexError:  # pragma: no cover
            raise UartError('Checksum receival failed.')

        if hex(expected_checksum) != hex(gotten_checksum):  # pragma: no cover
            raise UartError(
                'Transmission failed! expected {}, got {} instead!'.format(
                    hex(expected_checksum), hex(gotten_checksum)),
            )
        return string

    @benchmark
    def read_whole(self):
        res = self.read(1)
        while self.in_waiting and b'\0' not in res:
            if self.logger:  # pragma: no cover
                self.logger.info(
                    '{} bytes are waiting for us...'.format(self.in_waiting),
                )
            res += self.read(self.in_waiting)
        return res
