#ifndef _MOCKREGISTERS_H
#define _MOCKREGISTERS_H

typedef unsigned char uint8_t;
typedef unsigned long uint16_t;

void read_next(void);
void write_next(void);

void write_to_buff(char* data);
void read_from_buff(char* dst);

void write_to_buff_binary(uint8_t* data, int size);
#endif
