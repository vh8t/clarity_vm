#include <csignal>
#include <regex>

#include "debug.h"
#include "vm.h"

VM::VM(bool debug, const std::vector<uint8_t> &data)
    : data(data), sp(0), debug(debug) {
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

    case MOV_ADDR:
        cpu.registers[instr.operand1] = instr.operand2;
        break;
    case MOV_REG:
        cpu.registers[instr.operand1] = cpu.registers[instr.operand2];
        break;
    case MOV_FROM_REG:
        cpu.registers[instr.operand1] = data[cpu.registers[instr.operand2]];
        break;
    case MOV_FROM_ADDR:
        cpu.registers[instr.operand1] = data[instr.operand2];
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

int VM::run(const std::vector<Instruction> &program, const uint32_t start) {
    std::vector<int> breakpoints;
    running = true;
    pc = start;
    std::regex re("^b\\s+(\\d+)$");

    bool d_running = false;

    if (debug) {
        init_term();
        std::atexit(revert_term);
        std::signal(SIGINT, sig_revert_term);
        std::signal(SIGTERM, sig_revert_term);
        clear_term();
        print_debug(cpu, program[pc], pc, stack, sp);
    }

    while (running) {
        if (debug && !d_running) {
            std::string line;
            std::getline(std::cin, line);

            if (line == "s") {
                execute(program[pc]);
            } else if (line == "r") {
                d_running = true;
            } else if (line == "q") {
                break;
            } else if (line.rfind("b ", 0) == 0) {
                std::smatch match;
                if (std::regex_match(line, match, re)) {
                    breakpoints.push_back(std::stoi(match[1]));
                }
            }
            clear_term();
            print_debug(cpu, program[pc], pc, stack, sp);
        } else if (debug && d_running) {
            execute(program[pc]);
            clear_term();
            print_debug(cpu, program[pc], pc, stack, sp);
            if (std::find(breakpoints.begin(), breakpoints.end(), pc) !=
                breakpoints.end())
                d_running = false;
        } else {
            execute(program[pc]);
        }
    }

    return cpu.registers[0];
}
