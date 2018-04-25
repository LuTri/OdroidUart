#ifndef _MOCKREGISTERS_H
#define _MOCKREGISTERS_H

typedef unsigned char uint8_t;

void read_next(void);
void write_next(void);

void write_to_buff(char* data);
void read_from_buff(char* dst);
#endif
