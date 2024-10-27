#ifndef VM_H
#define VM_H

#include "object.h"
#include <cstdint>

class VM {
public:
  VM(const std::vector<uint8_t> bc, const std::vector<Object> pool);
  void print_state();
  Object pop();
  void run();

private:
  uint32_t pc = 0;
  bool halt = false;
  std::vector<Object> stack;
  const std::vector<uint8_t> bytecode;
  const std::vector<Object> const_pool;

  void push(Object obj);
  uint32_t btoi(uint32_t offset);
  void execute();
};

#endif // VM_H
