#ifndef LOADER_H
#define LOADER_H

#include "object.h"
#include <cstdint>
#include <string>
#include <vector>

using std::string, std::vector;

struct File {
  unsigned short major_version;
  unsigned short minor_version;
  const vector<uint8_t> bytecode;
  const vector<Object> const_pool;
  uint32_t pc;
};

File load_from_file(string path);
void generate_file(File file);

#endif
