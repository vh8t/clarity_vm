#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>
#include <variant>
#include <vector>

enum Type {
  NULL_TYPE,
  INTEGER,
  FLOAT,
  STRING,
  BOOLEAN,
  INT_LIST,
  FLOAT_LIST,
  STRING_LIST,
  BOOL_LIST,
};

struct Object {
  Type type;
  std::variant<std::monostate, int, double, std::string, bool, std::vector<int>,
               std::vector<double>, std::vector<std::string>, std::vector<bool>>
      value;

  Object() : type(Type::NULL_TYPE), value(std::monostate{}) {}

  template <typename T> Object(Type type, T val) : type(type), value(val) {}

  void print() const {
    std::visit(
        [](auto &&arg) {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, std::monostate>) {
            std::cout << "null";
          } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                               std::is_same_v<T, std::vector<double>> ||
                               std::is_same_v<T, std::vector<std::string>> ||
                               std::is_same_v<T, std::vector<bool>>) {
            std::cout << "[";
            for (size_t i = 0; i < arg.size(); i++) {
              if (i > 0)
                std::cout << ", ";
              std::cout << arg[i];
            }
            std::cout << "]";
          } else {
            std::cout << arg;
          }
        },
        value);
  }

  template <typename T> bool is_type() const {
    return std::holds_alternative<T>(value);
  }

  template <typename T> T &as() { return std::get<T>(value); }

  template <typename T> const T &as() const { return std::get<T>(value); }
};

std::string type_to_string(Type type);

#endif // OBJECT_H
