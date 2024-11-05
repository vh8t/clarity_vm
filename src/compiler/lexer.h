#ifndef LEXER_H
#define LEXER_H

#include "tokens.h"
#include <vector>

using std::vector;

class Lexer {
public:
  vector<string> errors;
  vector<Token> tokens;
  void tokenize();

  Lexer(string source);

private:
  string source;
  int index = 0;
  int row = 1;
  int col = 1;

  char current = 0;
  char next = 0;

  void advance();
};

#endif
