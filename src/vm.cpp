#include <iostream>

#include "debug.h"
#include "vm.h"

VM::VM() : sp(0) {
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
        stack[sp++] = pc;
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

        if (reg1 == reg2) {
            set_flag(cpu.flags, ZF);
        } else if (reg1 > reg2) {
            set_flag(cpu.flags, GF);
        } else {
            set_flag(cpu.flags, LF);
        }
        break;
    }
    case TEST:
        if ((cpu.registers[instr.operand1] & cpu.registers[instr.operand2]) ==
            0)
            set_flag(cpu.flags, ZF);
        else
            clear_flag(cpu.flags, ZF);
        break;

    case NOP:
        break;
    case HLT:
        running = false;
        break;

    default:
        std::cerr << "unknow opcode: " << instr.opcode << std::endl;
        break;
    }
}

void VM::run(const std::vector<Instruction> &program) {
    running = true;
    pc = 0;

    while (running) {
        if (DEBUG) {
            std::cout << "==== VM State ====" << std::endl;
            print_cpu_state(cpu);
            print_stack(stack, sp);
            std::cout << "PC: " << pc << "\n==================" << std::endl;
            print_instruction(program[pc], pc);
        }

        execute(program[pc]);
        pc++;
    }
}
