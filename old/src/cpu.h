#ifndef CPU_H
#define CPU_H

#include <cstdint>

struct CPU {
  uint32_t registers[8];
  uint32_t flags;
};

const uint32_t ZF = 1 << 0; // zero flag
const uint32_t GF = 1 << 1; // greater flag
const uint32_t LF = 1 << 2; // less flag
const uint32_t SF = 1 << 3; // sign flag

void set_flag(uint32_t &flags, uint32_t flag);
void clear_flag(uint32_t &flags, uint32_t flag);
bool is_flag_set(uint32_t flags, uint32_t flag);

#endif
