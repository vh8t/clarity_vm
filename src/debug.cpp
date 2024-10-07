#include "debug.h"

bool DEBUG = true;

const char *get_opcode_name(OpCode opcode) {
    switch (opcode) {
        // clang-format off
    case ADD: return "ADD";
    case SUB: return "SUB";
    case MUL: return "MUL";
    case DIV: return "DIV";
    case MOD: return "MOD";
    case INC: return "INC";
    case DEC: return "DEC";
    case AND: return "AND";
    case OR: return "OR";
    case XOR: return "XOR";
    case NOT: return "NOT";
    case SHL: return "SHL";
    case SHR: return "SHR";
    case MOV: return "MOV";
    case LOAD: return "LOAD";
    case STORE: return "STORE";
    case PUSH: return "PUSH";
    case POP: return "POP";
    case JMP: return "JMP";
    case JZ: return "JZ";
    case JNZ: return "JNZ";
    case JG: return "JG";
    case JL: return "JL";
    case CALL: return "CALL";
    case RET: return "RET";
    case CMP: return "CMP";
    case TEST: return "TEST";
    case NOP: return "NOP";
    case HLT: return "HLT";
    default: return "UNKNOWN";
        // clang-format on
    }
}

void print_instruction(const Instruction &instr, uint32_t pc) {
    if (!DEBUG)
        return;
    std::cout << "PC: " << pc << " | "
              << "Executing: " << get_opcode_name(instr.opcode) << " "
              << instr.operand1 << ", " << instr.operand2 << std::endl;
}

void print_cpu_state(const CPU &cpu) {
    if (!DEBUG)
        return;
    std::cout << "Registers: ";
    for (int i = 0; i < 8; ++i) {
        std::cout << "r" << i << "=" << cpu.registers[i] << " ";
    }
    std::cout << "\nFlags: " << cpu.flags << std::endl;
}

void print_stack(const uint32_t *stack, uint32_t sp) {
    if (!DEBUG)
        return;
    std::cout << "Stack (SP=" << sp << "): ";
    for (int i = sp - 1; i >= 0; --i) {
        std::cout << stack[i] << " ";
    }
    std::cout << std::endl;
}
