#ifndef DEBUG_H
#define DEBUG_H

#include "vm.h"
#include <iostream>

extern bool DEBUG;

void print_instruction(const Instruction &instr, uint32_t pc);
void print_cpu_state(const CPU &cpu);
void print_stack(const uint32_t *stack, uint32_t sp);

#endif
