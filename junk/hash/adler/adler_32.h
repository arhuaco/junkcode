#ifndef AD7606C_ADLER_32_H_
#define AD7606C_ADLER_32_H_

#include <stdint.h>

void addler_32_add_sample(uint16_t data_for_hash[8]);

uint32_t adler32_get();

void adler32_reset();

#endif  // AD7606C_ADLER_32_H_
