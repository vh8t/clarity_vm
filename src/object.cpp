#include "object.h"

std::string type_to_string(Type type) {
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
    return "NULL";
  case INT_LIST:
    return "INT_LIST";
  case FLOAT_LIST:
    return "FLOAT_LIST";
  case STRING_LIST:
    return "STRING_LIST";
  case BOOL_LIST:
    return "BOOL_LIST";
  }
  return "UNKNOWN";
}
