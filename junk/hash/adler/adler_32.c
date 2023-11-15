#include <stdint.h>

#include "adler_32.h"

static uint32_t adler_a_ = 1, adler_b_ = 0;
const uint32_t MOD_ADLER = 65521;

void adler_32_add_8(uint8_t data) {
    adler_a_ = (adler_a_ + data) % MOD_ADLER;
    adler_b_ = (adler_b_ + adler_a_) % MOD_ADLER;
}

uint32_t adler32_get() {
    return (adler_b_ << 16) | adler_a_;
}

void adler32_reset() {
    adler_a_ = 1;
    adler_b_ = 0;
}
