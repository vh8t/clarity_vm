#include "lexer.h"
#include <cctype>

bool is_keyword(string s) {
  const vector<string> keywords = {"fn",    "Int",    "Str",   "Bool",
                                   "Float", "return", "if",    "else",
                                   "for",   "while",  "break", "continue"};
  for (string keyword : keywords) {
    if (keyword == s)
      return true;
  }
  return false;
}

Lexer::Lexer(string source) : source(source) {
  if (index < source.size()) {
    current = source[index];
    if (index + 1 < source.size())
      next = source[index + 1];
  }
}

void Lexer::advance() {
  if (current == '\n') {
    col = 1;
    row++;
  } else {
    col++;
  }

  if (index + 1 < source.size()) {
    current = source[index + 1];
    if (index + 2 < source.size()) {
      next = source[index + 2];
    } else {
      next = 0;
    }
  } else {
    current = 0;
  }

  index++;
}

void Lexer::tokenize() {
  while (current != 0) {
    int start_row = row;
    int start_col = col;

    if (isalpha(current)) {
      string buffer;
      while (isalnum(current)) {
        buffer.push_back(current);
        advance();
      }

      if (is_keyword(buffer))
        tokens.push_back(Token(KEYWORD, buffer, start_row, start_col));
      else
        tokens.push_back(Token(IDENTIFIER, buffer, start_row, start_col));
    } else if (isdigit(current)) {
      string buffer;
      bool has_dot = false;
      while (isdigit(current) || (!has_dot && current == '.')) {
        if (current == '.')
          has_dot = true;
        buffer.push_back(current);
        advance();
      }

      if (has_dot)
        tokens.push_back(Token(FLOAT, buffer, start_row, start_col));
      else
        tokens.push_back(Token(INTEGER, buffer, start_row, start_col));
    } else if (current == '"') {
      advance();

      string buffer;
      while (true) {
        if (current == '\\') {
          advance();
          switch (current) {
          case 'b':
            buffer.push_back('\b');
            break;
          case 'n':
            buffer.push_back('\n');
            break;
          case 'r':
            buffer.push_back('\r');
            break;
          case 't':
            buffer.push_back('\t');
            break;
          case '"':
            buffer.push_back('"');
            break;
          case '\\':
            buffer.push_back('\\');
            break;
          default:
            errors.push_back(to_string(row) + ":" + to_string(col) +
                             " Unknown escape sequence");
            break;
          }
        } else if (current == '"') {
          advance();
          break;
        } else if (current == '\n') {
          errors.push_back(to_string(start_row) + ":" + to_string(start_col) +
                           " Unclosed string");
          break;
        } else {
          buffer.push_back(current);
        }
        advance();
      }
      tokens.push_back(Token(STRING, buffer, start_row, start_col));
    } else if (isspace(current)) {
      if (current == '\n' && next != '\n')
        tokens.push_back(Token(NEWLINE, "", start_row, start_col));
      advance();
    } else {
      Type type;
      switch (current) {
      case '(':
        type = LPAREN;
        break;
      case ')':
        type = RPAREN;
        break;
      case '[':
        type = LBRACKET;
        break;
      case ']':
        type = RBRACKET;
        break;
      case '{':
        type = LBRACE;
        break;
      case '}':
        type = RBRACE;
        break;
      case ',':
        type = COMMA;
        break;
      case ':':
        type = COLON;
        break;
      case '+':
        type = PLUS;
        break;
      case '-':
        type = MINUS;
        break;
      case '*':
        type = ASTERISK;
        break;
      case '/': {
        if (next == '/') {
          while (current != '\n' && current != 0)
            advance();
          type = COMMENT;
        } else {
          type = SLASH;
        }
        break;
      }
      case '^':
        type = CARRET;
        break;
      case '~':
        type = TILDA;
        break;
      case '!': {
        if (next == '=') {
          advance();
          type = NOT_EQ;
        } else {
          type = BANG;
        }
        break;
      }
      case '<': {
        if (next == '=') {
          advance();
          type = LESS_EQ;
        } else {
          type = LESS;
        }
        break;
      }
      case '>': {
        if (next == '=') {
          advance();
          type = GREATER_EQ;
        } else {
          type = GREATER;
        }
        break;
      }
      case '=': {
        if (next == '=') {
          advance();
          type = EQ;
        } else {
          type = ASSIGN;
        }
        break;
      }
      case '%':
        type = MODULO;
        break;
      case '&': {
        if (next == '&') {
          advance();
          type = LOG_AND;
        } else {
          type = BIT_AND;
        }
        break;
      }
      case '|': {
        if (next == '|') {
          advance();
          type = LOG_OR;
        } else {
          type = BIT_OR;
        }
        break;
      }
      case '.':
        type = DOT;
        break;
      default:
        errors.push_back(to_string(row) + ":" + to_string(col) +
                         " Unknown token");
        break;
      }
      if (type != COMMENT)
        tokens.push_back(Token(type, "", start_row, start_col));
      advance();
    }
  }
}
