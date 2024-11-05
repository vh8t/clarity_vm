#include "tests.h"
#include "bytecode.h"
#include "loader.h"
#include "object.h"
#include "vm.h"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iterator>
#include <string>

const double EPSILON = 1e-6;

struct Result {
  bool passed;
  string error;
};

// test_utils {{{
void print_test_result(const string &test_name, const string &test,
                       Result res) {
  std::cout << "["
            << (res.passed ? "\x1b[1;32mPASSED\x1b[0m"
                           : "\x1b[1;31mFAILED\x1b[0m")
            << "] \x1b[34m" << std::left << std::setw(18) << test_name
            << "\x1b[0m " << test;
  if (!res.passed) {
    std::cout << "\n\t\x1b[33m" << res.error;
  }
  std::cout << "\x1b[0m" << std::endl;
}

Result assert_int_result(const Object &result, int expected_value) {
  if (!result.is_type<int>()) {
    return {false, "type mismatch"};
  }
  return {result.as<int>() == expected_value,
          "value mismatch - wanted: " + std::to_string(expected_value) +
              ", got: " + std::to_string(result.as<int>())};
}

Result assert_float_result(const Object &result, double expected_value) {
  if (!result.is_type<double>()) {
    return {false, "type mismatch"};
  }
  return {std::fabs(result.as<double>() - expected_value) < EPSILON,
          "value mismatch - wanted: " + std::to_string(expected_value) +
              ", got: " + std::to_string(result.as<double>())};
}

Result assert_string_result(const Object &result, string expected_value) {
  if (!result.is_type<string>()) {
    return {false, "type mismatch"};
  }
  return {result.as<string>() == expected_value,
          "value mismatch - wanted: " + expected_value +
              ", got: " + result.as<string>()};
}

Result assert_bool_result(const Object &result, bool expected_value) {
  if (!result.is_type<bool>()) {
    return {false, "type mismatch"};
  }
  return {result.as<bool>() == expected_value,
          "value mismatch - wanted: " + std::to_string(expected_value) +
              ", got: " + std::to_string(result.as<bool>())};
}

void run_vm_test(const std::vector<uint8_t> &bytecode,
                 const std::vector<Object> &const_pool, const string &test_name,
                 const string &test, const Object &expected_result) {
  VM vm(bytecode, const_pool);
  vm.run();
  Object result = vm.pop();

  Result res;
  if (expected_result.is_type<int>()) {
    res = assert_int_result(result, expected_result.as<int>());
  } else if (expected_result.is_type<double>()) {
    res = assert_float_result(result, expected_result.as<double>());
  } else if (expected_result.is_type<string>()) {
    res = assert_string_result(result, expected_result.as<string>());
  } else if (expected_result.is_type<bool>()) {
    res = assert_bool_result(result, expected_result.as<bool>());
  }

  print_test_result(test_name, test, res);
}
// test_utils }}}

// add_test {{{
void add_test() {
  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, ADD, HALT},
              {Object(INTEGER, 10), Object(INTEGER, 12)}, "add_test",
              "10 + 12 = 22", Object(INTEGER, 22));

  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, ADD, HALT},
              {Object(FLOAT, 10.293), Object(FLOAT, 12.2782)}, "add_test",
              "10.293 + 12.2782 = 22.5712", Object(FLOAT, 22.5712));

  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, ADD, HALT},
              {Object(STRING, "Hello, "), Object(STRING, "World!")}, "add_test",
              "\"Hello, \" + \"World!\" = \"Hello, World!\"",
              Object(STRING, "Hello, World!"));
}
// add_test }}}

// sub_test {{{
void sub_test() {
  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, SUB, HALT},
              {Object(INTEGER, 10), Object(INTEGER, 12)}, "sub_test",
              "10 - 12 = -2", Object(INTEGER, -2));

  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, SUB, HALT},
              {Object(FLOAT, 12.2782), Object(FLOAT, 10.293)}, "sub_test",
              "12.2782 - 10.293 = 1.9852", Object(FLOAT, 1.9852));
}
// sub_test }}}

// mul_test {{{
void mul_test() {
  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, MUL, HALT},
              {Object(INTEGER, 10), Object(INTEGER, 12)}, "mul_test",
              "10 * 12 = 120", Object(INTEGER, 120));

  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, MUL, HALT},
              {Object(FLOAT, 12.293), Object(INTEGER, 10)}, "mul_test",
              "12.293 * 10 = 122.93", Object(FLOAT, 122.93));

  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, MUL, HALT},
              {Object(STRING, "Hello, World!"), Object(STRING, 3)}, "mul_test",
              "\"Hello, World!\" * 3 = \"Hello, World!Hello, "
              "World!Hello, World!\"",
              Object(STRING, "Hello, World!Hello, World!Hello, World!"));
}
// mul_test }}}

// div_test {{{
void div_test() {
  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, DIV, HALT},
              {Object(INTEGER, 10), Object(INTEGER, 2)}, "div_test",
              "10 / 2 = 5.0", Object(FLOAT, 5.0));

  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, DIV, HALT},
              {Object(FLOAT, 5.25), Object(FLOAT, 1.25)}, "div_test",
              "5.25 / 1.25 = 4.2", Object(FLOAT, 4.2));
}
// div_test }}}

// idiv_test {{{
void idiv_test() {
  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, IDIV, HALT},
              {Object(INTEGER, 10), Object(INTEGER, 2)}, "idiv_test",
              "10 // 2 = 5", Object(INTEGER, 5));

  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, IDIV, HALT},
              {Object(FLOAT, 5.25), Object(FLOAT, 1.25)}, "idiv_test",
              "5.25 // 1.25 = 4", Object(INTEGER, 4));
}
// idiv_test }}}

// eq_test {{{
void eq_test() {
  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, EQ, HALT},
              {Object(INTEGER, 12), Object(INTEGER, 12)}, "eq_test",
              "12 == 12 = true", Object(BOOLEAN, true));

  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, EQ, HALT},
              {Object(FLOAT, 12.543), Object(FLOAT, 12.543)}, "eq_test",
              "12.543 == 12.543 = true", Object(BOOLEAN, true));

  run_vm_test(
      {PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, EQ, HALT},
      {Object(STRING, "Hello, World!"), Object(STRING, "Hello, World!")},
      "eq_test", "\"Hello, World!\" == \"Hello, World!\" = true",
      Object(BOOLEAN, true));
}
// eq_test }}}

// neq_test {{{
void neq_test() {
  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, NEQ, HALT},
              {Object(INTEGER, 12), Object(INTEGER, 12)}, "neq_test",
              "12 != 12 = false", Object(BOOLEAN, false));

  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, NEQ, HALT},
              {Object(FLOAT, 12.543), Object(FLOAT, 12.543)}, "neq_test",
              "12.543 != 12.543 = false", Object(BOOLEAN, false));

  run_vm_test(
      {PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, NEQ, HALT},
      {Object(STRING, "Hello, World!"), Object(STRING, "Hello, World!")},
      "neq_test", "\"Hello, World!\" != \"Hello, World!\" = false",
      Object(BOOLEAN, false));
}
// neq_test }}}

// lt_test {{{
void lt_test() {
  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, LT, HALT},
              {Object(INTEGER, 10), Object(INTEGER, 7)}, "lt_test",
              "10 < 7 = false", Object(BOOLEAN, false));

  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, LT, HALT},
              {Object(FLOAT, 10.231), Object(FLOAT, 10.232)}, "lt_test",
              "10.231 < 10.232 = true", Object(BOOLEAN, true));

  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, LT, HALT},
              {Object(FLOAT, 9.99), Object(INTEGER, 10)}, "lt_test",
              "9.99 < 10 = true", Object(BOOLEAN, true));
}
// lt_test }}}

// gt_test {{{
void gt_test() {
  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, GT, HALT},
              {Object(INTEGER, 10), Object(INTEGER, 7)}, "gt_test",
              "10 > 7 = true", Object(BOOLEAN, true));

  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, GT, HALT},
              {Object(FLOAT, 10.232), Object(FLOAT, 10.232)}, "gt_test",
              "10.232 > 10.232 = false", Object(BOOLEAN, false));

  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, GT, HALT},
              {Object(FLOAT, 9.99), Object(INTEGER, 10)}, "gt_test",
              "9.99 > 10 = false", Object(BOOLEAN, false));
}
// gt_test }}}

// lte_test {{{
void lte_test() {
  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, LTE, HALT},
              {Object(INTEGER, 10), Object(INTEGER, 7)}, "lte_test",
              "10 <= 7 = false", Object(BOOLEAN, false));

  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, LTE, HALT},
              {Object(FLOAT, 10.232), Object(FLOAT, 10.232)}, "lte_test",
              "10.232 <= 10.232 = true", Object(BOOLEAN, true));

  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, LTE, HALT},
              {Object(FLOAT, 9.99), Object(INTEGER, 10)}, "lte_test",
              "9.99 <= 10 = true", Object(BOOLEAN, true));
}
// lte_test }}}

// gte_test {{{
void gte_test() {
  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, GTE, HALT},
              {Object(INTEGER, 10), Object(INTEGER, 7)}, "gte_test",
              "10 >= 7 = true", Object(BOOLEAN, true));

  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, GTE, HALT},
              {Object(FLOAT, 10.232), Object(FLOAT, 10.232)}, "gte_test",
              "10.232 >= 10.232 = true", Object(BOOLEAN, true));

  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, GTE, HALT},
              {Object(FLOAT, 9.99), Object(INTEGER, 10)}, "gte_test",
              "9.99 >= 10 = false", Object(BOOLEAN, false));
}
// gte_test }}}

// log_and_test {{{
void log_and_test() {
  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, LOG_AND, HALT},
              {Object(BOOLEAN, true), Object(BOOLEAN, true)}, "log_and_test",
              "true && true = true", Object(BOOLEAN, true));

  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, LOG_AND, HALT},
              {Object(BOOLEAN, true), Object(BOOLEAN, false)}, "log_and_test",
              "true && false = false", Object(BOOLEAN, false));

  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, LOG_AND, HALT},
              {Object(BOOLEAN, false), Object(BOOLEAN, false)}, "log_and_test",
              "false && false = false", Object(BOOLEAN, false));
}
// log_and_test }}}

// log_or_test {{{
void log_or_test() {
  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, LOG_OR, HALT},
              {Object(BOOLEAN, true), Object(BOOLEAN, true)}, "log_or_test",
              "true || true = true", Object(BOOLEAN, true));

  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, LOG_OR, HALT},
              {Object(BOOLEAN, true), Object(BOOLEAN, false)}, "log_or_test",
              "true || false = true", Object(BOOLEAN, true));

  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, LOG_OR, HALT},
              {Object(BOOLEAN, false), Object(BOOLEAN, false)}, "log_or_test",
              "false || false = false", Object(BOOLEAN, false));
}
// log_or_test }}}

// log_not_test {{{
void log_not_test() {
  run_vm_test({PUSH, 0, 0, 0, 0, LOG_NOT, HALT}, {Object(BOOLEAN, true)},
              "log_not_test", "!true = false", Object(BOOLEAN, false));

  run_vm_test({PUSH, 0, 0, 0, 0, LOG_NOT, HALT}, {Object(BOOLEAN, false)},
              "log_not_test", "!false = true", Object(BOOLEAN, true));
}
// log_not_test }}}

// bit_and_test {{{
void bit_and_test() {
  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, BIT_AND, HALT},
              {Object(INTEGER, 3), Object(INTEGER, 5)}, "bit_and_test",
              "3 & 5 = 1", Object(INTEGER, 1));
}
// bit_and_test }}}

// bit_or_test {{{
void bit_or_test() {
  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, BIT_OR, HALT},
              {Object(INTEGER, 3), Object(INTEGER, 5)}, "bit_or_test",
              "3 | 5 = 7", Object(INTEGER, 7));
}
// bit_or_test }}}

// bit_not_test {{{
void bit_not_test() {
  run_vm_test({PUSH, 0, 0, 0, 0, BIT_NOT, HALT}, {Object(INTEGER, 3)},
              "bit_not_test", "~3 = -4", Object(INTEGER, -4));
}
// bit_not_test }}}

// xor_test {{{
void xor_test() {
  run_vm_test({PUSH, 0, 0, 0, 0, PUSH, 1, 0, 0, 0, XOR, HALT},
              {Object(INTEGER, 3), Object(INTEGER, 5)}, "xor_test", "3 ^ 5 = 6",
              Object(INTEGER, 6));
}
// xor_test }}}

// encode_bytecode_test {{{
void encode_bytecode_test() {
  // clang-format off
  const vector<uint8_t> bytecode = {
    PUSH, 0, 0, 0, 0,
    PUSH, 1, 0, 0, 0,
    ADD,
    PUSH, 2, 0, 0, 0,
    DIV,
    HALT,
  };
  const vector<Object> const_pool = {
    Object(INTEGER, 2839),
    Object(FLOAT, 82.2842),
    Object(INTEGER, 28),
    Object(LIST, vector<Object>{
      Object(INTEGER, 10),
      Object(INTEGER, 12),
      Object(INTEGER, 821),
    }),
  };
  // clang-format on

  File file = {0, 1, bytecode, const_pool, 0};
  generate_file(file);
}
// encode_bytecode_test }}}

// load_bytecode_test {{{
void load_bytecode_test() {
  File file = load_from_file("out.bin");
  run_vm_test(file.bytecode, file.const_pool, "load_bytecode_test",
              "(2839 + 82.2842) / 28 = 104.331579", Object(FLOAT, 104.331579));
}
// load_bytecode_test }}}

// const_load_test {{{
void write_objects() {
  vector<Object> objs = {
      Object(LIST,
             vector<Object>{Object(INTEGER, 43),
                            Object(STRING, "Hello, World!"),
                            Object(BOOLEAN, true), Object(FLOAT, 10.2841)}),
      Object(INTEGER, 10), Object()};

  vector<uint8_t> bytecode;

  for (auto obj : objs)
    encode_object(obj, bytecode);

  std::ofstream file("obj.bin", std::ios::binary);
  file.write(reinterpret_cast<const char *>(bytecode.data()), bytecode.size());
  file.close();
}

void const_load_test() {
  std::ifstream file("obj.bin", std::ios::binary);
  vector<uint8_t> bytecode((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
  file.close();

  while (bytecode.size() > 0) {
    decode_object(bytecode).print();
    cout << " ";
  }
  cout << endl;
}
// const_load_test }}}

// tests {{{
void tests() {
  add_test();
  sub_test();
  mul_test();
  div_test();
  idiv_test();
  eq_test();
  neq_test();
  lt_test();
  gt_test();
  lte_test();
  gte_test();
  log_and_test();
  log_or_test();
  log_not_test();
  bit_and_test();
  bit_or_test();
  bit_not_test();
  xor_test();

  encode_bytecode_test();
  load_bytecode_test();
}
// tests }}}
