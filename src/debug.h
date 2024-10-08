#ifndef DEBUG_H
#define DEBUG_H

#include "vm.h"
#include <iostream>

void init_term();
void clear_term();
void revert_term();
void sig_revert_term(int signal);
void print_debug(const CPU &cpu, const Instruction &instr, uint32_t pc,
                 uint32_t *stack, uint32_t sp);

#endif
