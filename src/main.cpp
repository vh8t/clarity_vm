#include <fstream>
#include <iostream>

#include "vm.h"

std::vector<uint8_t> read_file(const std::string &file_name) {
    std::ifstream file(file_name, std::ios::binary);

    if (!file) {
        throw std::runtime_error("could not open file");
    }

    file.seekg(0, std::ios::end);
    std::streamsize file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(file_size);

    if (!file.read(reinterpret_cast<char *>(buffer.data()), file_size)) {
        throw std::runtime_error("error reading file");
    }

    return buffer;
}

uint32_t bytesToUint32(const std::vector<uint8_t> &bytes, int offset) {
    if (bytes.size() < 4) {
        throw std::runtime_error("not enough bytes to form uint32_t");
    }

    return static_cast<uint32_t>((bytes[offset]) | (bytes[offset + 1] << 8) |
                                 (bytes[offset + 2] << 16) |
                                 (bytes[offset + 3] << 24));
}

std::vector<Instruction> bytes_to_prog(std::vector<uint8_t> bytes) {
    std::vector<Instruction> prog;

    for (int i = 0; i < bytes.size(); i++) {
        uint8_t opcode = bytes[i];
        Instruction instr;
        switch (opcode) {
            // clang-format off
        case ADD:   instr = {ADD, bytes[i + 1], bytes[i + 2]}; i += 2; break;
        case SUB:   instr = {SUB, bytes[i + 1], bytes[i + 2]}; i += 2; break;
        case MUL:   instr = {MUL, bytes[i + 1], bytes[i + 2]}; i += 2; break;
        case DIV:   instr = {DIV, bytes[i + 1], bytes[i + 2]}; i += 2; break;
        case MOD:   instr = {MOD, bytes[i + 1], bytes[i + 2]}; i += 2; break;
        case INC:   instr = {INC, bytes[i + 1], 0}; i++; break;
        case DEC:   instr = {DEC, bytes[i + 1], 0}; i++; break;
        case AND:   instr = {AND, bytes[i + 1], bytes[i + 2]}; i += 2; break;
        case OR:    instr = {OR, bytes[i + 1], bytes[i + 2]}; i += 2; break;
        case XOR:   instr = {XOR, bytes[i + 1], bytes[i + 2]}; i += 2; break;
        case NOT:   instr = {NOT, bytes[i + 1], 0}; i++; break;
        case SHL:   instr = {SHL, bytes[i + 1], bytesToUint32(bytes, i + 2)}; i += 5; break;
        case SHR:   instr = {SHR, bytes[i + 1], bytesToUint32(bytes, i + 2)}; i += 5; break;
        case MOV:   instr = {MOV, bytes[i + 1], bytesToUint32(bytes, i + 2)}; i += 5; break;
        case LOAD:  instr = {LOAD, bytes[i + 1], bytesToUint32(bytes, i + 2)}; i += 5; break;
        case STORE: instr = {STORE, bytes[i + 1], bytesToUint32(bytes, i + 2)}; i += 5; break;
        case PUSH:  instr = {PUSH, bytes[i + 1], 0}; i++; break;
        case POP:   instr = {POP, bytes[i + 1], 0}; i++; break;
        case JMP:   instr = {JMP, bytesToUint32(bytes, i + 1), 0}; i += 4; break;
        case JZ:    instr = {JZ, bytesToUint32(bytes, i + 1), 0}; i += 4; break;
        case JNZ:   instr = {JNZ, bytesToUint32(bytes, i + 1), 0}; i += 4; break;
        case JG:    instr = {JG, bytesToUint32(bytes, i + 1), 0}; i += 4; break;
        case JL:    instr = {JL, bytesToUint32(bytes, i + 1), 0}; i += 4; break;
        case CALL:  instr = {CALL, bytesToUint32(bytes, i + 1), 0}; i += 4; break;
        case RET:   instr = {RET, 0, 0}; break;
        case CMP:   instr = {CMP, bytes[i + 1], bytes[i + 2]}; i += 2; break;
        case TEST:  instr = {TEST, bytes[i + 1], bytes[i + 2]}; i += 2; break;
        case NOP:   instr = {NOP, 0, 0}; break;
        case HLT:   instr = {HLT, 0, 0}; break;
            // clang-format on
        }
        prog.push_back(instr);
    }

    return prog;
};

int main(int argc, char **argv) {
    // clang-format off
    // std::vector<Instruction> program = {
    //     {MOV, 0, 10},
    //     {MOV, 1, 20},
    //     {ADD, 0, 1},
    //     {PUSH, 0},
    //     {POP, 2},
    //     {HLT},
    // };
    // clang-format on

    std::vector<uint8_t> file_data;
    try {
        file_data = read_file(argv[1]);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    std::vector<Instruction> prog = bytes_to_prog(file_data);
    // std::cout << prog.size() << std::endl;

    // for (int i = 0; i < prog.size(); i++) {
    //     Instruction inst = prog[i];
    //     std::cout << inst.opcode << " " << inst.operand1 << " " <<
    //     inst.operand2
    //               << std::endl;
    // }
    // return 0;

    VM vm;
    vm.run(prog);
}
