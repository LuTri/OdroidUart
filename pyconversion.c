#include <avr/io.h>
#include "pyconversion.h"

float per_cent_2byte(uint8_t h_value, uint8_t l_value) {
	return per_one_2byte(h_value, l_value) * 100.0;
}

float per_one_2byte(uint8_t h_value, uint8_t l_value) {
	return ((float)dualbyte(h_value, l_value)) / (float)0xFFFF;
}

float per_one_1byte(uint8_t value) {
	return (float)value / (float)0xFF;
}

float real_360_2byte(uint8_t h_value, uint8_t l_value) {
    return per_one_2byte(h_value, l_value) * 360.0;
}

uint16_t dualbyte(uint8_t h_value, uint8_t l_value) {
	return (h_value << 8) | l_value;
}