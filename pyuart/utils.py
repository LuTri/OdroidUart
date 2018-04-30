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

import struct


def string_to_uint8_array(string):
    return struct.unpack('B' * len(string), string)


def fletcher_checksum(string):
    sum1 = sum2 = 0

    values = string_to_uint8_array(string)
    for x in values:
        sum1 = (sum1 + x) % 255
        sum2 = (sum2 + sum1) % 255

    return (sum1 << 8) | sum2
