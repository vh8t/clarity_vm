#ifndef OBJECT_H
#define OBJECT_H

#include <cstdint>
#include <iostream>
#include <variant>
#include <vector>

using std::string, std::vector, std::monostate, std::get, std::cout;

enum Type {
  NULL_TYPE,
  INTEGER,
  FLOAT,
  STRING,
  BOOLEAN,
  LIST,
};

struct Object {
  Type type;
  std::variant<monostate, int, double, string, bool, vector<Object>> value;

  Object() : type(Type::NULL_TYPE), value(monostate{}) {}

  template <typename T> Object(Type type, T val) : type(type), value(val) {}

  void print() const {
    switch (type) {
    case NULL_TYPE:
      cout << "null";
      break;
    case INTEGER:
      cout << as<int>();
      break;
    case FLOAT:
      cout << as<double>();
      break;
    case STRING:
      cout << as<string>();
      break;
    case BOOLEAN: {
      if (as<bool>())
        cout << "true";
      else
        cout << "false";
      break;
    }
    case LIST: {
      vector<Object> list = as<vector<Object>>();
      cout << "[";
      for (size_t i = 0; i < list.size(); i++) {
        if (i != 0)
          cout << ", ";
        list[i].print();
      }
      cout << "]";
      break;
    }
    }
  }

  template <typename T> bool is_type() const {
    return std::holds_alternative<T>(value);
  }

  template <typename T> T &as() { return get<T>(value); }

  template <typename T> const T &as() const { return get<T>(value); }
};

string type_to_string(Type type);
void encode_object(const Object &obj, vector<uint8_t> &bytecode);
Object decode_object(vector<uint8_t> &bytecode);

#endif // OBJECT_H
