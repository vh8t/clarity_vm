#ifndef VM_H
#define VM_H

#include "cpu.h"
#include <cstdint>
#include <vector>

#define MAJOR 1
#define MINOR 1

// clang-format off
enum OpCode {
    ADD, SUB, MUL, DIV, MOD, INC, DEC,
    AND, OR, XOR, NOT, SHL, SHR,
    MOV_ADDR, MOV_REG, MOV_FROM_ADDR, MOV_FROM_REG, MOV,
    LOAD, STORE, PUSH, POP,
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

    VM(bool debug, const std::vector<uint8_t> &data);
    int run(const std::vector<Instruction> &program, const uint32_t start);

  private:
    std::vector<uint8_t> data;
    uint32_t stack[STACK_SIZE];
    uint32_t heap[HEAP_SIZE];
    uint32_t sp;
    CPU cpu;
    uint32_t pc;
    bool running;
    bool debug;

    void execute(const Instruction instr);
};

#endif
