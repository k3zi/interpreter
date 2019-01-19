//===--- Tokenizer.cpp ------------------------------------------------===//
//
// Author: Kesi Maduka
// Description: Includes and runs the Tokenizer class.
//
//===----------------------------------------------------------------------===//

#include "Tokenizer.h"
#include <iostream> // std::cerr, std::endl

int main(int argc, char** argv) {
  if (argc <= 1) {
    std::cerr << "Please specify a file name." << std::endl;
    exit(1);
  }

  unsigned lineNumber = 0;
  try {
    Tokenizer tokenizer (argv[argc - 1]);

    while (!tokenizer.isEOF()) {
      lineNumber = tokenizer.lineNumber();
      tokenizer.nextToken();
      // std::string token = tokenizer.currentToken();
      int tokenNumber = tokenizer.currentTokenNumber();
      std::cout << tokenNumber << std::endl;
      lineNumber = tokenizer.lineNumber();
    }
  } catch (std::string error) {
    if (lineNumber) {
      std::cerr << "Error [Line " << lineNumber << "] " << error << std::endl;
    } else {
      std::cerr << error << std::endl;
    }
  }
}
