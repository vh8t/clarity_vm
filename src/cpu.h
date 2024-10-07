#ifndef CPU_H
#define CPU_H

#include <cstdint>

struct CPU {
    uint32_t registers[8];
    uint32_t flags;
};

const uint32_t ZF = 1 << 0;
const uint32_t GF = 1 << 1;
const uint32_t LF = 1 << 2;

void set_flag(uint32_t &flags, uint32_t flag);
void clear_flag(uint32_t &flags, uint32_t flag);
bool is_flag_set(uint32_t flags, uint32_t flag);

#endif
