//===--- Tokenizer.cpp ------------------------------------------------===//
//
// Author: ケジ
// Description: Includes and runs the Tokenizer class.
//
//===----------------------------------------------------------------------===//

#include "Tokenizer.h"
#include <iostream>

int main(int argc, char** argv) {
  if (argc <= 1) {
    std::cerr << "Please specify a file name." << std::endl;
    exit(1);
  }
  try {
    Tokenizer tokenizer (argv[argc - 1]);
    while (!tokenizer.isEOF()) {
      std::string token = tokenizer.currentToken();
      int tokenNumber = tokenizer.numberForToken(token);
      std::cout << tokenNumber << std::endl;
      tokenizer.nextToken();
    }
  } catch (std::string error) {
    std::cerr << error << std::endl;
  }
}
