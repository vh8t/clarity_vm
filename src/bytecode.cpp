#include "bytecode.h"

string inst_to_string(uint8_t inst) {
  switch (inst) {
  case ADD:
    return "ADD";
  case SUB:
    return "SUB";
  case MUL:
    return "MUL";
  case IDIV:
    return "IDIV";
  case DIV:
    return "DIV";
  case PUSH:
    return "PUSH";
  case POP:
    return "POP";
  case HALT:
    return "HALT";
  case EQ:
    return "EQ";
  case NEQ:
    return "NEQ";
  case LT:
    return "LT";
  case GT:
    return "GT";
  case LTE:
    return "LTE";
  case GTE:
    return "GTE";
  case LOG_AND:
    return "LOG_AND";
  case LOG_OR:
    return "LOG_OR";
  case LOG_NOT:
    return "LOG_NOT";
  case BIT_AND:
    return "BIT_AND";
  case BIT_OR:
    return "BIT_OR";
  case BIT_NOT:
    return "BIT_NOT";
  case XOR:
    return "XOR";
  default:
    return "UNKNOWN";
  }
}
