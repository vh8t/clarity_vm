#ifndef LOADER_H
#define LOADER_H

#include "vm.h"
#include <fstream>

std::vector<uint8_t> read_file(const std::string &file_name);
std::vector<Instruction> bytes_to_prog(std::vector<uint8_t> bytes);
uint32_t bytes_to_uint32(const std::vector<uint8_t> &bytes, int offset);

#endif
