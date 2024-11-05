#ifndef TOKENS_H
#define TOKENS_H

#include <string>

using std::string, std::to_string;

enum Type {
  DOT,
  NEWLINE,
  KEYWORD,
  IDENTIFIER,
  INTEGER,
  FLOAT,
  STRING,
  LPAREN,
  RPAREN,
  LBRACKET,
  RBRACKET,
  LBRACE,
  RBRACE,
  COMMA,
  COLON,
  PLUS,
  MINUS,
  ASTERISK,
  SLASH,
  CARRET,
  TILDA,
  BANG,
  NOT_EQ,
  LESS,
  LESS_EQ,
  GREATER,
  GREATER_EQ,
  ASSIGN,
  EQ,
  MODULO,
  BIT_AND,
  LOG_AND,
  BIT_OR,
  LOG_OR,
  COMMENT,
};

struct Token {
  Type type;
  string literal;
  int row;
  int col;

  Token(Type type, string literal, int row, int col)
      : type(type), literal(literal), row(row), col(col) {}
};

string type_to_string(Type type);

#endif
