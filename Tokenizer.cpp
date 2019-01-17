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
  Tokenizer tokenizer (argv[argc - 1]);
}
