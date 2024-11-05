#include "object.h"
#include <cstring>

using std::memcpy;

string type_to_string(Type type) {
  switch (type) {
  case STRING:
    return "STRING";
  case INTEGER:
    return "INTEGER";
  case FLOAT:
    return "FLOAT";
  case BOOLEAN:
    return "BOOLEAN";
  case NULL_TYPE:
    return "NULL_TYPE";
  case LIST:
    return "LIST";
  }
  return "UNKNOWN";
}

void encode_object(const Object &obj, vector<uint8_t> &bytecode) {
  bytecode.push_back(static_cast<uint8_t>(obj.type));

  switch (obj.type) {
  case NULL_TYPE:
    break;
  case INTEGER:
    bytecode.insert(
        bytecode.end(), reinterpret_cast<const char *>(&obj.as<int>()),
        reinterpret_cast<const char *>(&obj.as<int>()) + sizeof(int));
    break;
  case FLOAT:
    bytecode.insert(
        bytecode.end(), reinterpret_cast<const char *>(&obj.as<double>()),
        reinterpret_cast<const char *>(&obj.as<double>()) + sizeof(double));
    break;
  case STRING: {
    const string &str = obj.as<string>();
    uint32_t length = str.length();
    bytecode.insert(bytecode.end(), reinterpret_cast<const char *>(&length),
                    reinterpret_cast<const char *>(&length) + sizeof(uint32_t));
    bytecode.insert(bytecode.end(), str.begin(), str.end());
    break;
  }
  case BOOLEAN:
    bytecode.push_back(obj.as<bool>() ? 1 : 0);
    break;
  case LIST: {
    const vector<Object> &list = obj.as<vector<Object>>();
    uint32_t length = list.size();
    bytecode.insert(bytecode.end(), reinterpret_cast<const char *>(&length),
                    reinterpret_cast<const char *>(&length) + sizeof(uint32_t));
    for (const auto &elem : list)
      encode_object(elem, bytecode);
    break;
  }
  }
}

Object decode_object(std::vector<uint8_t> &bytecode) {
  Type type = static_cast<Type>(bytecode[0]);
  bytecode.erase(bytecode.begin());

  switch (type) {
  case INTEGER: {
    int value;
    std::memcpy(&value, bytecode.data(), sizeof(int));
    bytecode.erase(bytecode.begin(), bytecode.begin() + sizeof(int));
    return Object(INTEGER, value);
  }
  case FLOAT: {
    double value;
    std::memcpy(&value, bytecode.data(), sizeof(double));
    bytecode.erase(bytecode.begin(), bytecode.begin() + sizeof(double));
    return Object(FLOAT, value);
  }
  case STRING: {
    uint32_t length;
    std::memcpy(&length, bytecode.data(), sizeof(uint32_t));
    bytecode.erase(bytecode.begin(), bytecode.begin() + sizeof(uint32_t));

    std::string str(bytecode.begin(), bytecode.begin() + length);
    bytecode.erase(bytecode.begin(), bytecode.begin() + length);
    return Object(STRING, str);
  }
  case BOOLEAN: {
    bool value = bytecode[0] != 0;
    bytecode.erase(bytecode.begin());
    return Object(BOOLEAN, value);
  }
  case LIST: {
    uint32_t length;
    std::memcpy(&length, bytecode.data(), sizeof(uint32_t));
    bytecode.erase(bytecode.begin(), bytecode.begin() + sizeof(uint32_t));

    std::vector<Object> list;
    for (size_t i = 0; i < length; ++i) {
      list.push_back(decode_object(bytecode));
    }
    return Object(LIST, list);
  }
  case NULL_TYPE:
    return Object();
  }

  throw std::runtime_error("Unknown object type");
}
