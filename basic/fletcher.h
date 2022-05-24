#ifndef _FLETCHER_H
#define _FLETCHER_H

#include <avr/io.h>

uint16_t fletchers_checksum(uint8_t* data, uint16_t length);

#endif