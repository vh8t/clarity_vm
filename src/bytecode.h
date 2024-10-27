#ifndef BYTECODE_H
#define BYTECODE_H

#include <cstdint>
#include <string>

enum {
  ADD,
  SUB,
  MUL,
  DIV,
  IDIV,
  PUSH,
  POP,
  HALT,

  EQ,
  NEQ,
  LT,
  GT,
  LTE,
  GTE,

  LOG_AND,
  LOG_OR,
  LOG_NOT,
  BIT_AND,
  BIT_OR,
  BIT_NOT,
  XOR,
};

std::string inst_to_string(uint8_t inst);

#endif // BYTECODE_H
