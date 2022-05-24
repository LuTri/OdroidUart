#include "fletcher.h"

uint16_t fletchers_checksum(uint8_t* data, uint16_t length) {
    uint8_t sum1 = 0, sum2 = 0;

    while (length--) {
        sum1 = (sum1 + *data++) % 255;
        sum2 = (sum1 + sum2) % 255;
    }

    return (sum1 << 8) | sum2;
}