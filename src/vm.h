#ifndef VM_H
#define VM_H

#include "cpu.h"
#include <cstdint>
#include <vector>

// clang-format off
enum OpCode {
    ADD, SUB, MUL, DIV, MOD, INC, DEC,
    AND, OR, XOR, NOT, SHL, SHR,
    MOV, LOAD, STORE, PUSH, POP,
    JMP, JZ, JNZ, JG, JL, CALL, RET,
    CMP, TEST,
    NOP, HLT,
};
// clang-format on

struct Instruction {
    OpCode opcode;
    uint32_t operand1;
    uint32_t operand2;
};

class VM {
  public:
    static const int STACK_SIZE = 1024;
    static const int HEAP_SIZE = 4096;

    VM();
    void run(const std::vector<Instruction> &program);

  private:
    uint32_t stack[STACK_SIZE];
    uint32_t heap[HEAP_SIZE];
    uint32_t sp;
    CPU cpu;
    uint32_t pc;
    bool running;

    void execute(const Instruction instr);
};

#endif
