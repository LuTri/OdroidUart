#!/usr/bin/env python3

import json
import argparse


class ConfigWriter:
    def __init__(self, *args, **kwargs):
        self._uart_names = []
        self._filename = None

    def write(self, data):
        with open(self._filename, 'w') as fp:
            json.dump(data, fp, indent=2)

    def make_args(self, parser):
        with open('writer.meta', 'r') as fp:
            data = fp.readlines()
        for line in data:
            flag = line.strip().split('=', 1)
            _args = (
                '--{name}'.format(name=flag[0]),
            )
            _kwargs = {
            }

            if len(flag) > 1:
                _kwargs['type'] = int
                _kwargs['required'] = False
                _kwargs['default'] = flag[1]
            else:
                self._uart_names.append(flag[0].rstrip('_ENABLED'))
                _kwargs['action'] = 'store_true'
                _kwargs['default'] = False
            parser.add_argument(*_args, **_kwargs)

    def __call__(self, out_file=None, **kwargs):
        self._filename = out_file
        data = {}
        for name in self._uart_names:
            flag = '{}_ENABLED'.format(name)
            num = name.strip('USART')
            if kwargs.get(flag, False):
                data[name] = {}

                for key, value in kwargs.items():
                    if (num in key and key != flag) or key in ['BAUD',
                                                               'MC_CLOCK',
                                                               'PORT']:
                        _key = key.replace('UART_RX{}'.format(num), 'RX')
                        _key = _key.replace('UART_TX{}'.format(num), 'TX')

                        _key = _key.replace('USART{}_'.format(num), '')
                        _key = _key.replace('UART{}_'.format(num), '')

                        data[name][_key] = value
        self.write(data)


def main():
    c_parser = ConfigWriter()
    parser = argparse.ArgumentParser()
    parser.add_argument('--out-file', type=str, default='.usart.ini')
    parser.add_argument('--BAUD', type=int)
    parser.add_argument('--PORT', type=str)
    parser.add_argument('--MC-CLOCK', type=int)
    c_parser.make_args(parser)

    args = parser.parse_args()
    c_parser(**vars(args))


if __name__ == '__main__':
    main()