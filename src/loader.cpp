#include "loader.h"

bool check_header(std::vector<uint8_t> bytes) {
    if (bytes[0] == 0xc1 && bytes[1] == 0xa0)
        return true;
    return false;
}

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

    if (!check_header(buffer)) {
        throw std::runtime_error("invalid file format");
    }

    return buffer;
}

uint32_t bytes_to_uint32(const std::vector<uint8_t> &bytes, int offset) {
    if (bytes.size() < 4) {
        throw std::runtime_error("not enough bytes to form uint32_t");
    }

    return static_cast<uint32_t>((bytes[offset]) | (bytes[offset + 1] << 8) |
                                 (bytes[offset + 2] << 16) |
                                 (bytes[offset + 3] << 24));
}

std::vector<uint8_t> bytes_to_data(std::vector<uint8_t> bytes) {
    std::vector<uint8_t> data;
    size_t end = bytes_to_uint32(bytes, 4);

    for (size_t i = 12; i < end + 12; i++) {
        data.push_back(bytes[i]);
    }

    return data;
}

std::vector<Instruction> bytes_to_prog(std::vector<uint8_t> bytes) {
    std::vector<Instruction> prog;
    size_t start = bytes_to_uint32(bytes, 4);

    for (size_t i = start + 12; i < bytes.size(); i++) {
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
        case SHL:   instr = {SHL, bytes[i + 1], bytes_to_uint32(bytes, i + 2)}; i += 5; break;
        case SHR:   instr = {SHR, bytes[i + 1], bytes_to_uint32(bytes, i + 2)}; i += 5; break;
        case MOV_ADDR: instr = {MOV_ADDR, bytes[i + 1], bytes_to_uint32(bytes, i + 2)}; i += 5; break;
        case MOV_REG: instr = {MOV_REG, bytes[i + 1], bytes[i + 2]}; i += 2; break;
        case MOV_FROM_ADDR: instr = {MOV_FROM_ADDR, bytes[i + 1], bytes_to_uint32(bytes, i + 2)}; i += 5; break;
        case MOV_FROM_REG: instr = {MOV_FROM_REG, bytes[i + 1], bytes[i + 2]}; i += 2; break;
        case MOV:   instr = {MOV, bytes[i + 1], bytes_to_uint32(bytes, i + 2)}; i += 5; break;
        case LOAD:  instr = {LOAD, bytes[i + 1], bytes_to_uint32(bytes, i + 2)}; i += 5; break;
        case STORE: instr = {STORE, bytes[i + 1], bytes_to_uint32(bytes, i + 2)}; i += 5; break;
        case PUSH:  instr = {PUSH, bytes[i + 1], 0}; i++; break;
        case POP:   instr = {POP, bytes[i + 1], 0}; i++; break;
        case JMP:   instr = {JMP, bytes_to_uint32(bytes, i + 1), 0}; i += 4; break;
        case JZ:    instr = {JZ, bytes_to_uint32(bytes, i + 1), 0}; i += 4; break;
        case JNZ:   instr = {JNZ, bytes_to_uint32(bytes, i + 1), 0}; i += 4; break;
        case JG:    instr = {JG, bytes_to_uint32(bytes, i + 1), 0}; i += 4; break;
        case JL:    instr = {JL, bytes_to_uint32(bytes, i + 1), 0}; i += 4; break;
        case CALL:  instr = {CALL, bytes_to_uint32(bytes, i + 1), 0}; i += 4; break;
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
