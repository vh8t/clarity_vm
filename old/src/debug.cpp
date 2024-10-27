#include "debug.h"
#include <iomanip>

void init_term() { std::cout << "\x1b[?1049h"; }
void revert_term() { std::cout << "\x1b[?1049l"; }

void sig_revert_term(int signal) {
  revert_term();
  exit(signal);
}

void clear_term() { std::cout << "\x1b[2J\x1b[3J\x1b[H"; }
void set_pos(int x, int y) { std::cout << "\x1b[" << y << ";" << x << "H"; }

const std::string get_opcode_name(OpCode opcode) {
  switch (opcode) {
    // clang-format off
  case ADD: return "add";
  case SUB: return "sub";
  case MUL: return "mul";
  case DIV: return "div";
  case MOD: return "mod";
  case INC: return "inc";
  case DEC: return "dec";
  case AND: return "and";
  case OR: return "or";
  case XOR: return "xor";
  case NOT: return "not";
  case SHL: return "shl";
  case SHR: return "shr";
  case MOV_ADDR:
    return "mov";
  case MOV_REG:
    return "mov_reg";
  case MOV_FROM_ADDR:
  case MOV_FROM_REG:
  case MOV_INT:
  case MOV:
      return "mov";
  case LOAD: return "load";
  case STORE: return "store";
  case PUSH: return "push";
  case POP: return "pop";
  case JMP: return "jmp";
  case JZ: return "jz";
  case JNZ: return "jnz";
  case JG: return "jg";
  case JL: return "jl";
  case CALL: return "call";
  case RET: return "ret";
  case CMP: return "cmp";
  case TEST: return "test";
  case NOP: return "nop";
  case HLT: return "hlt";
  default: return "unknown";
    // clang-format on
  }
}

void print_registers(const CPU &cpu, int x, int y) {
  set_pos(x, y);
  std::cout << "┏━ Registers ━━┓";
  for (int i = 0; i < 8; i++) {
    set_pos(x, y + i + 1);
    std::cout << "┃ r" << i << ": " << std::setw(8) << cpu.registers[i] << " ┃";
  }
  set_pos(x, y + 9);
  std::cout << "┗━━━━━━━━━━━━━━┛";
}

void print_flags(const CPU &cpu, int x, int y) {
  set_pos(x, y);
  std::cout << "┏━ Flags ━━━━━━┓";
  set_pos(x, y + 1);
  std::cout << "┃ ZF: " << (is_flag_set(cpu.flags, ZF) ? "1" : "0")
            << "        ┃";
  set_pos(x, y + 2);
  std::cout << "┃ GF: " << (is_flag_set(cpu.flags, GF) ? "1" : "0")
            << "        ┃";
  set_pos(x, y + 3);
  std::cout << "┃ LF: " << (is_flag_set(cpu.flags, LF) ? "1" : "0")
            << "        ┃";
  set_pos(x, y + 4);
  std::cout << "┃ SF: " << (is_flag_set(cpu.flags, SF) ? "1" : "0")
            << "        ┃";
  set_pos(x, y + 5);
  std::cout << "┗━━━━━━━━━━━━━━┛";
}

void print_instruction(const Instruction &instr, uint32_t pc, int x, int y) {
  set_pos(x, y);
  std::cout << "┏━ Instruction ━━━┓";
  set_pos(x, y + 1);
  std::cout << "┃ PC: " << std::left << std::setw(5) << pc << "       ┃";
  set_pos(x, y + 2);
  std::cout << "┃ " << std::left << std::setw(15)
            << (get_opcode_name(instr.opcode) + " " +
                std::to_string(instr.operand1) + ", " +
                std::to_string(instr.operand2))
            << " ┃";
  set_pos(x, y + 3);
  std::cout << "┗━━━━━━━━━━━━━━━━━┛";
}

void print_stack(const uint32_t *stack, uint32_t sp, int x, int y) {
  int offset = 1;
  int limit = 0;

  set_pos(x, y);
  std::cout << "┏━ Stack ━━━━━━━━━┓";
  if (sp >= 9) {
    limit = sp - 15;
  }
  for (int i = sp; i > limit; i--) {
    set_pos(x, y + offset);
    std::cout << "┃ " << std::setw(15) << stack[i] << " ┃";
    offset++;
  }
  if (limit == 0) {
    for (size_t i = 0; i < 15 - sp; i++) {
      set_pos(x, y + offset);
      std::cout << "┃                 ┃";
      offset++;
    }
  }
  set_pos(x, y + offset);
  std::cout << "┗━━━━━━━━━━━━━━━━━┛";
}

void print_help(int x, int y) {
  set_pos(x, y);
  std::cout << "┏━ Help ━━━━━━━━━━━━━━━┓";
  set_pos(x, y + 1);
  std::cout << "┃ b <int> - breakpoint ┃";
  set_pos(x, y + 2);
  std::cout << "┃ s - step             ┃";
  set_pos(x, y + 3);
  std::cout << "┃ r - run              ┃";
  set_pos(x, y + 4);
  std::cout << "┃ q - quit             ┃";
  set_pos(x, y + 5);
  std::cout << "┗━━━━━━━━━━━━━━━━━━━━━━┛";
}

void print_debug(const CPU &cpu, const Instruction &instr, uint32_t pc,
                 uint32_t *stack, uint32_t sp) {
  print_instruction(instr, pc, 1, 1);
  print_stack(stack, sp, 1, 5);
  print_registers(cpu, 21, 1);
  print_flags(cpu, 21, 11);
  print_help(38, 1);
  set_pos(1, 23);
  std::cout << "clarity: ";
}
