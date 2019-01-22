//===--- Tokenizer.cpp ----------------------------------------------------===//
//
// Author: ケジ
// Description: Runs the tokenizer.
//
//===----------------------------------------------------------------------===//

#include "core/Tokenizer/Tokenizer.h"
#include <cstdlib>  // std::exit
#include <iostream> // std::cerr, std::endl
#include <sstream>  // std::ostringstream

int main(int argc, char **argv) {
  if (argc <= 1) {
    std::cerr << "Please specify a file name." << std::endl;
    std::exit(1);
  }

  try {
    // Second argument [1] should be the name of the file.
    Tokenizer tokenizer = *Tokenizer::CreateFromFile(argv[1]);

    // Use a stream so we can control when the token numbers get outputted.
    std::ostringstream output;

    while (!tokenizer.isEOF()) {
      tokenizer.NextToken();
      output << tokenizer.currentToken().getType() << std::endl;
    }

    // Now, output the string of token numbers.
    std::cout << output.str();
  } catch (std::string &error) {
    std::cerr << error << std::endl;
  }
}
