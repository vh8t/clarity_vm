#include "lexer.h"
#include <fstream>
#include <iostream>
#include <sstream>

using std::cout, std::endl, std::cerr, std::ifstream, std::stringstream;

int main(int argc, const char **argv) {
  if (argc != 2) {
    cout << "Usage: " << argv[0] << " <file>" << endl;
    return 1;
  }

  ifstream file(argv[1]);
  if (!file) {
    cerr << "Could not open file" << endl;
    return 1;
  }

  stringstream buffer;
  buffer << file.rdbuf();

  string source(buffer.str());

  Lexer lexer(source);
  lexer.tokenize();

  if (lexer.errors.size() != 0) {
    for (auto error : lexer.errors) {
      cerr << error << "\n";
    }
    cerr << endl;
    return 1;
  }

  for (auto token : lexer.tokens) {
    cout << token.row << ":" << token.col << " " << type_to_string(token.type)
         << " " << token.literal << "\n";
  }
  cout << endl;
}
