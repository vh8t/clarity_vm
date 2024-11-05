#include "tokens.h"

string type_to_string(Type type) {
  switch (type) {
  case DOT:
    return "dot";
  case NEWLINE:
    return "newline";
  case KEYWORD:
    return "keyword";
  case IDENTIFIER:
    return "identifier";
  case INTEGER:
    return "integer";
  case FLOAT:
    return "float";
  case STRING:
    return "string";
  case LPAREN:
    return "lparen";
  case RPAREN:
    return "rparen";
  case LBRACKET:
    return "lbracket";
  case RBRACKET:
    return "rbracket";
  case LBRACE:
    return "lbrace";
  case RBRACE:
    return "rbrace";
  case COMMA:
    return "comma";
  case COLON:
    return "colon";
  case PLUS:
    return "plus";
  case MINUS:
    return "minus";
  case ASTERISK:
    return "asterisk";
  case SLASH:
    return "slash";
  case CARRET:
    return "carret";
  case TILDA:
    return "tilda";
  case BANG:
    return "bang";
  case NOT_EQ:
    return "not_eq";
  case LESS:
    return "less";
  case LESS_EQ:
    return "less_eq";
  case GREATER:
    return "greater";
  case GREATER_EQ:
    return "greater_eq";
  case ASSIGN:
    return "assign";
  case EQ:
    return "eq";
  case MODULO:
    return "modulo";
  case BIT_AND:
    return "bit_and";
  case LOG_AND:
    return "log_and";
  case BIT_OR:
    return "bit_or";
  case LOG_OR:
    return "log_or";
  case COMMENT:
    return "comment";
  }
}
