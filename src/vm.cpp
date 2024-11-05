#include "vm.h"
#include "bytecode.h"

VM::VM(const vector<uint8_t> bc, const vector<Object> pool)
    : bytecode(bc), const_pool(pool) {}

Object VM::pop() {
  if (stack.empty()) {
    throw std::runtime_error(
        "Stack underflow: Attempt to pop from an empty stack.");
  }

  Object obj = stack.back();
  stack.pop_back();
  return obj;
}

void VM::push(Object obj) { stack.push_back(obj); }

uint32_t VM::btoi(uint32_t offset) {
  if (offset + 3 >= bytecode.size()) {
    throw std::runtime_error(
        "Offset out of bounds: Unable to read 4 bytes from bytecode.");
  }

  return static_cast<uint32_t>(bytecode[offset]) |
         (static_cast<uint32_t>(bytecode[offset + 1]) << 8) |
         (static_cast<uint32_t>(bytecode[offset + 2]) << 16) |
         (static_cast<uint32_t>(bytecode[offset + 3]) << 24);
}

void VM::run() {
  while (!halt) {

    // NOTE: Debug code
    // print_state();
    // ENDNOTE

    try {
      execute();
    } catch (const std::exception &ex) {
      cerr << ex.what() << endl;
      exit(1);
    }
  }
}

void VM::execute() {
  uint8_t byte = bytecode[pc];
  switch (byte) {
  case ADD: {
    Object b = pop();
    Object a = pop();

    if (a.is_type<int>() && b.is_type<int>()) {
      int a_val = a.as<int>();
      int b_val = b.as<int>();

      push(Object(Type::INTEGER, a_val + b_val));
    } else if ((a.is_type<int>() || a.is_type<double>()) &&
               (b.is_type<int>() || b.is_type<double>())) {
      double a_val;
      double b_val;

      if (a.is_type<int>()) {
        a_val = static_cast<double>(a.as<int>());
      } else {
        a_val = a.as<double>();
      }

      if (b.is_type<int>()) {
        b_val = static_cast<double>(b.as<int>());
      } else {
        b_val = b.as<double>();
      }

      push(Object(Type::FLOAT, a_val + b_val));
    } else if (a.is_type<string>() && b.is_type<string>()) {
      string a_val = a.as<string>();
      string b_val = b.as<string>();

      push(Object(Type::STRING, a_val + b_val));
    } else {
      throw std::runtime_error(
          "Type error in ADD operation: unsupported operand types '" +
          type_to_string(a.type) + "' + '" + type_to_string(b.type) + "'.");
    }

    pc++;
    break;
  }
  case SUB: {
    Object b = pop();
    Object a = pop();

    if (a.is_type<int>() && b.is_type<int>()) {
      int a_val = a.as<int>();
      int b_val = b.as<int>();

      push(Object(Type::INTEGER, a_val - b_val));
    } else if ((a.is_type<int>() || a.is_type<double>()) &&
               (b.is_type<int>() || b.is_type<double>())) {
      double a_val;
      double b_val;

      if (a.is_type<int>()) {
        a_val = static_cast<double>(a.as<int>());
      } else {
        a_val = a.as<double>();
      }

      if (b.is_type<int>()) {
        b_val = static_cast<double>(b.as<int>());
      } else {
        b_val = b.as<double>();
      }

      push(Object(Type::FLOAT, a_val - b_val));
    } else {
      throw std::runtime_error(
          "Type error in SUB operation: unsupported operand types '" +
          type_to_string(a.type) + "' - '" + type_to_string(b.type) + "'.");
    }

    pc++;
    break;
  }
  case MUL: {
    Object b = pop();
    Object a = pop();

    if (a.is_type<int>() && b.is_type<int>()) {
      int a_val = a.as<int>();
      int b_val = b.as<int>();

      push(Object(Type::INTEGER, a_val * b_val));
    } else if ((a.is_type<int>() || a.is_type<double>()) &&
               (b.is_type<int>() || b.is_type<double>())) {
      double a_val;
      double b_val;

      if (a.is_type<int>()) {
        a_val = static_cast<double>(a.as<int>());
      } else {
        a_val = a.as<double>();
      }

      if (b.is_type<int>()) {
        b_val = static_cast<double>(b.as<int>());
      } else {
        b_val = b.as<double>();
      }

      push(Object(Type::FLOAT, a_val * b_val));
    } else if (a.is_type<string>() && b.is_type<int>()) {
      string a_val = a.as<string>();
      int b_val = b.as<int>();

      string repeated;
      for (int i = 0; i < b_val; i++) {
        repeated += a_val;
      }

      push(Object(Type::STRING, repeated));
    } else {
      throw std::runtime_error(
          "Type error in MUL operation: unsupported operand types '" +
          type_to_string(a.type) + "' * '" + type_to_string(b.type) + "'.");
    }

    pc++;
    break;
  }
  case DIV: {
    Object b = pop();
    Object a = pop();

    if ((a.is_type<int>() || a.is_type<double>()) &&
        (b.is_type<int>() || b.is_type<double>())) {
      double a_val;
      double b_val;

      if (a.is_type<int>()) {
        a_val = static_cast<double>(a.as<int>());
      } else {
        a_val = a.as<double>();
      }

      if (b.is_type<int>()) {
        b_val = static_cast<double>(b.as<int>());
      } else {
        b_val = b.as<double>();
      }

      push(Object(Type::FLOAT, a_val / b_val));
    } else {
      throw std::runtime_error(
          "Type error in DIV operation: unsupported operand types '" +
          type_to_string(a.type) + "' / '" + type_to_string(b.type) + "'.");
    }

    pc++;
    break;
  }
  case IDIV: {
    Object b = pop();
    Object a = pop();

    if ((a.is_type<int>() || a.is_type<double>()) &&
        (b.is_type<int>() || b.is_type<double>())) {
      double a_val;
      double b_val;

      if (a.is_type<int>()) {
        a_val = static_cast<double>(a.as<int>());
      } else {
        a_val = a.as<double>();
      }

      if (b.is_type<int>()) {
        b_val = static_cast<double>(b.as<int>());
      } else {
        b_val = b.as<double>();
      }

      push(Object(Type::INTEGER, static_cast<int>(a_val / b_val)));
    } else {
      throw std::runtime_error(
          "Type error in IDIV operation: unsupported operand types '" +
          type_to_string(a.type) + "' // '" + type_to_string(b.type) + "'.");
    }

    pc++;
    break;
  }
  case PUSH: {
    uint32_t obj = btoi(pc + 1);
    if (obj >= const_pool.size()) {
      throw std::runtime_error(
          "PUSH operation error: constant pool index " + std::to_string(obj) +
          " is out of bounds (size: " + std::to_string(const_pool.size()) +
          ").");
    }
    push(const_pool[obj]);

    pc += 5;
    break;
  }
  case POP: {
    pop();
    pc++;
    break;
  }
  case HALT: {
    halt = true;
    pc++;
    break;
  }
  case EQ: {
    Object b = pop();
    Object a = pop();

    if (a.type != b.type) {
      push(Object(Type::BOOLEAN, false));
    } else if (a.is_type<string>()) {
      string a_val = a.as<string>();
      string b_val = b.as<string>();

      push(Object(Type::BOOLEAN, a_val == b_val));
    } else if (a.is_type<int>()) {
      int a_val = a.as<int>();
      int b_val = b.as<int>();

      push(Object(Type::BOOLEAN, a_val == b_val));
    } else if (a.is_type<double>()) {
      double a_val = a.as<double>();
      double b_val = b.as<double>();

      push(Object(Type::BOOLEAN, a_val == b_val));
    } else if (a.is_type<bool>()) {
      bool a_val = a.as<double>();
      bool b_val = b.as<double>();

      push(Object(Type::BOOLEAN, a_val == b_val));
    } else {
      throw std::runtime_error(
          "Type error in EQ operation: unsupported operand types '" +
          type_to_string(a.type) + "' == '" + type_to_string(b.type) + "'.");
    }

    pc++;
    break;
  }
  case NEQ: {
    Object b = pop();
    Object a = pop();

    if (a.type != b.type) {
      push(Object(Type::BOOLEAN, true));
    } else if (a.is_type<string>()) {
      string a_val = a.as<string>();
      string b_val = b.as<string>();

      push(Object(Type::BOOLEAN, a_val != b_val));
    } else if (a.is_type<int>()) {
      int a_val = a.as<int>();
      int b_val = b.as<int>();

      push(Object(Type::BOOLEAN, a_val != b_val));
    } else if (a.is_type<double>()) {
      double a_val = a.as<double>();
      double b_val = b.as<double>();

      push(Object(Type::BOOLEAN, a_val != b_val));
    } else if (a.is_type<bool>()) {
      bool a_val = a.as<bool>();
      bool b_val = b.as<bool>();

      push(Object(Type::BOOLEAN, a_val != b_val));
    } else {
      throw std::runtime_error(
          "Type error in NEQ operation: unsupported operand types '" +
          type_to_string(a.type) + "' != '" + type_to_string(b.type) + "'.");
    }

    pc++;
    break;
  }
  case LT: {
    Object b = pop();
    Object a = pop();

    if ((a.is_type<int>() || a.is_type<double>()) &&
        (b.is_type<int>() || b.is_type<double>())) {
      double a_val;
      double b_val;

      if (a.is_type<int>()) {
        a_val = static_cast<double>(a.as<int>());
      } else {
        a_val = a.as<double>();
      }

      if (b.is_type<int>()) {
        b_val = static_cast<double>(b.as<int>());
      } else {
        b_val = b.as<double>();
      }

      push(Object(Type::BOOLEAN, a_val < b_val));
    } else {
      throw std::runtime_error(
          "Type error in LT operation: unsupported operand types '" +
          type_to_string(a.type) + "' < '" + type_to_string(b.type) + "'.");
    }

    pc++;
    break;
  }
  case GT: {
    Object b = pop();
    Object a = pop();

    if ((a.is_type<int>() || a.is_type<double>()) &&
        (b.is_type<int>() || b.is_type<double>())) {
      double a_val;
      double b_val;

      if (a.is_type<int>()) {
        a_val = static_cast<double>(a.as<int>());
      } else {
        a_val = a.as<double>();
      }

      if (b.is_type<int>()) {
        b_val = static_cast<double>(b.as<int>());
      } else {
        b_val = b.as<double>();
      }

      push(Object(Type::BOOLEAN, a_val > b_val));
    } else {
      throw std::runtime_error(
          "Type error in GT operation: unsupported operand types '" +
          type_to_string(a.type) + "' > '" + type_to_string(b.type) + "'.");
    }

    pc++;
    break;
  }
  case LTE: {
    Object b = pop();
    Object a = pop();

    if ((a.is_type<int>() || a.is_type<double>()) &&
        (b.is_type<int>() || b.is_type<double>())) {
      double a_val;
      double b_val;

      if (a.is_type<int>()) {
        a_val = static_cast<double>(a.as<int>());
      } else {
        a_val = a.as<double>();
      }

      if (b.is_type<int>()) {
        b_val = static_cast<double>(b.as<int>());
      } else {
        b_val = b.as<double>();
      }

      push(Object(Type::BOOLEAN, a_val <= b_val));
    } else {
      throw std::runtime_error(
          "Type error in LTE operation: unsupported operand types '" +
          type_to_string(a.type) + "' <= '" + type_to_string(b.type) + "'.");
    }

    pc++;
    break;
  }
  case GTE: {
    Object b = pop();
    Object a = pop();

    if ((a.is_type<int>() || a.is_type<double>()) &&
        (b.is_type<int>() || b.is_type<double>())) {
      double a_val;
      double b_val;

      if (a.is_type<int>()) {
        a_val = static_cast<double>(a.as<int>());
      } else {
        a_val = a.as<double>();
      }

      if (b.is_type<int>()) {
        b_val = static_cast<double>(b.as<int>());
      } else {
        b_val = b.as<double>();
      }

      push(Object(Type::BOOLEAN, a_val >= b_val));
    } else {
      throw std::runtime_error(
          "Type error in GTE operation: unsupported operand types '" +
          type_to_string(a.type) + "' >= '" + type_to_string(b.type) + "'.");
    }

    pc++;
    break;
  }
  case LOG_AND: {
    Object b = pop();
    Object a = pop();

    if (a.is_type<bool>() && b.is_type<bool>()) {
      bool a_val = a.as<bool>();
      bool b_val = b.as<bool>();

      push(Object(Type::BOOLEAN, a_val && b_val));
    } else {
      throw std::runtime_error(
          "Type error in LOG_AND operation: unsupported operand types '" +
          type_to_string(a.type) + "' && '" + type_to_string(b.type) + "'.");
    }

    pc++;
    break;
  }
  case LOG_OR: {
    Object b = pop();
    Object a = pop();

    if (a.is_type<bool>() && b.is_type<bool>()) {
      bool a_val = a.as<bool>();
      bool b_val = b.as<bool>();

      push(Object(Type::BOOLEAN, a_val || b_val));
    } else {
      throw std::runtime_error(
          "Type error in LOG_OR operation: unsupported operand types '" +
          type_to_string(a.type) + "' || '" + type_to_string(b.type) + "'.");
    }

    pc++;
    break;
  }
  case LOG_NOT: {
    Object a = pop();

    if (a.is_type<bool>()) {
      bool a_val = a.as<bool>();

      push(Object(Type::BOOLEAN, !a_val));
    } else {
      throw std::runtime_error(
          "Type error in LOG_NOT operation: unsupported operand types !'" +
          type_to_string(a.type) + "'.");
    }

    pc++;
    break;
  }
  case BIT_AND: {
    Object b = pop();
    Object a = pop();

    if (a.is_type<int>() && b.is_type<int>()) {
      int a_val = a.as<int>();
      int b_val = b.as<int>();

      push(Object(Type::INTEGER, a_val & b_val));
    } else {
      throw std::runtime_error(
          "Type error in BIT_AND operation: unsupported operand types '" +
          type_to_string(a.type) + "' & '" + type_to_string(b.type) + "'.");
    }

    pc++;
    break;
  }
  case BIT_OR: {
    Object b = pop();
    Object a = pop();

    if (a.is_type<int>() && b.is_type<int>()) {
      int a_val = a.as<int>();
      int b_val = b.as<int>();

      push(Object(Type::INTEGER, a_val | b_val));
    } else {
      throw std::runtime_error(
          "Type error in BIT_OR operation: unsupported operand types '" +
          type_to_string(a.type) + "' | '" + type_to_string(b.type) + "'.");
    }

    pc++;
    break;
  }
  case BIT_NOT: {
    Object a = pop();

    if (a.is_type<int>()) {
      int a_val = a.as<int>();

      push(Object(Type::INTEGER, ~a_val));
    } else {
      throw std::runtime_error(
          "Type error in BIT_NOT operation: unsupported operand types ~'" +
          type_to_string(a.type) + "'.");
    }

    pc++;
    break;
  }
  case XOR: {
    Object b = pop();
    Object a = pop();

    if (a.is_type<int>() && b.is_type<int>()) {
      int a_val = a.as<int>();
      int b_val = b.as<int>();

      push(Object(Type::INTEGER, a_val ^ b_val));
    } else {
      throw std::runtime_error(
          "Type error in BIT_XOR operation: unsupported operand types '" +
          type_to_string(a.type) + "' ^ '" + type_to_string(b.type) + "'.");
    }

    pc++;
    break;
  }
  }
}

void VM::print_state() {
  cout << "---------------" << endl;
  cout << "PC: " << inst_to_string(bytecode[pc]) << endl;
  cout << "Stack: \n[ ";
  for (auto obj : stack) {
    obj.print();
  }
  cout << "]" << endl;
}
