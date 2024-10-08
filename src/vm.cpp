#include <csignal>
#include <cstdio>
#include <iostream>

#include "debug.h"
#include "vm.h"

VM::VM(bool debug) : sp(0), debug(debug) {
    for (int i = 0; i < 8; ++i) {
        cpu.registers[i] = 0;
    }
    cpu.flags = 0;
}

void VM::execute(const Instruction instr) {
    switch (instr.opcode) {
    case ADD:
        cpu.registers[instr.operand1] += cpu.registers[instr.operand2];
        break;
    case SUB:
        cpu.registers[instr.operand1] -= cpu.registers[instr.operand2];
        break;
    case MUL:
        cpu.registers[instr.operand1] *= cpu.registers[instr.operand2];
        break;
    case DIV:
        cpu.registers[instr.operand1] /= cpu.registers[instr.operand2];
        break;
    case MOD:
        cpu.registers[instr.operand1] %= cpu.registers[instr.operand2];
        break;
    case INC:
        cpu.registers[instr.operand1]++;
        break;
    case DEC:
        cpu.registers[instr.operand1]--;
        break;

    case AND:
        cpu.registers[instr.operand1] &= cpu.registers[instr.operand2];
        break;
    case OR:
        cpu.registers[instr.operand1] |= cpu.registers[instr.operand2];
        break;
    case XOR:
        cpu.registers[instr.operand1] ^= cpu.registers[instr.operand2];
        break;
    case NOT:
        cpu.registers[instr.operand1] = ~cpu.registers[instr.operand1];
        break;
    case SHL:
        cpu.registers[instr.operand1] <<= instr.operand2;
        break;
    case SHR:
        cpu.registers[instr.operand1] >>= instr.operand2;
        break;

    case MOV:
        cpu.registers[instr.operand1] = instr.operand2;
        break;
    case LOAD:
        cpu.registers[instr.operand1] = heap[instr.operand2];
        break;
    case STORE:
        heap[instr.operand2] = cpu.registers[instr.operand1];
        break;
    case PUSH:
        stack[sp++] = cpu.registers[instr.operand1];
        break;
    case POP:
        cpu.registers[instr.operand1] = stack[--sp];
        break;

    case JMP:
        pc = instr.operand1;
        return;
    case JZ:
        if (is_flag_set(cpu.flags, ZF)) {
            pc = instr.operand1;
            return;
        }
        break;
    case JNZ:
        if (!is_flag_set(cpu.flags, ZF)) {
            pc = instr.operand1;
            return;
        }
        break;
    case JG:
        if (is_flag_set(cpu.flags, GF)) {
            pc = instr.operand1;
            return;
        }
        break;
    case JL:
        if (is_flag_set(cpu.flags, LF)) {
            pc = instr.operand1;
            return;
        }
        break;
    case CALL:
        stack[sp++] = pc + 1;
        pc = instr.operand1;
        return;
    case RET:
        pc = stack[--sp];
        return;

    case CMP: {
        int32_t reg1 = cpu.registers[instr.operand1];
        int32_t reg2 = cpu.registers[instr.operand2];

        clear_flag(cpu.flags, ZF);
        clear_flag(cpu.flags, GF);
        clear_flag(cpu.flags, LF);
        clear_flag(cpu.flags, SF);

        if (reg1 == reg2)
            set_flag(cpu.flags, ZF);
        else if (reg1 > reg2)
            set_flag(cpu.flags, GF);
        else
            set_flag(cpu.flags, LF);

        if (reg1 < 0)
            set_flag(cpu.flags, SF);
        break;
    }
    case TEST: {
        int32_t result =
            cpu.registers[instr.operand1] & cpu.registers[instr.operand2];

        clear_flag(cpu.flags, ZF);
        clear_flag(cpu.flags, SF);

        if (result == 0)
            set_flag(cpu.flags, ZF);
        else if (result < 0)
            set_flag(cpu.flags, SF);
        break;
    }

    case NOP:
        break;
    case HLT:
        running = false;
        break;

    default:
        std::cerr << "unknow opcode: " << instr.opcode << std::endl;
        break;
    }
    pc++;
}

void VM::run(const std::vector<Instruction> &program, const uint32_t start) {
    running = true;
    pc = start;

    if (debug) {
        init_term();
        std::atexit(revert_term);
        std::signal(SIGINT, sig_revert_term);
        std::signal(SIGTERM, sig_revert_term);
    }

    while (running) {
        if (debug) {
            clear_term();
            print_debug(cpu, program[pc], pc, stack, sp);
            getchar();
        }

        execute(program[pc]);
    }
}
