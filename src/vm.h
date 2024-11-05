#ifndef VM_H
#define VM_H

#include "object.h"
#include <cstdint>

#define MAJOR 0
#define MINOR 1

using std::vector, std::cerr, std::string, std::endl, std::cout;

class VM {
public:
  VM(const vector<uint8_t> bc, const vector<Object> pool);
  void print_state();
  Object pop();
  void run();

private:
  uint32_t pc = 0;
  bool halt = false;
  vector<Object> stack;
  const vector<uint8_t> bytecode;
  const vector<Object> const_pool;

  void push(Object obj);
  uint32_t btoi(uint32_t offset);
  void execute();
};

#endif // VM_H
