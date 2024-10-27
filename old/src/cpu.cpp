#include "cpu.h"

bool is_flag_set(uint32_t flags, uint32_t flag) { return (flags & flag) != 0; }

void clear_flag(uint32_t &flags, uint32_t flag) { flags &= ~flag; }

void set_flag(uint32_t &flags, uint32_t flag) { flags |= flag; }
