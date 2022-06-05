#ifndef _PYCONVERSION_H
#define _PYCONVERSION_H

float per_cent_2byte(uint8_t h_value, uint8_t l_value);
float per_one_2byte(uint8_t h_value, uint8_t l_value);
float per_one_1byte(uint8_t value);
float real_360_2byte(uint8_t h_value, uint8_t l_value);
uint16_t dualbyte(uint8_t h_value, uint8_t l_value);

#endif