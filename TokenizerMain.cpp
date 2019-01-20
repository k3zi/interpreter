//===--- TokenizerMain.cpp ------------------------------------------------===//
//
// Author: ケジ
// Description: Includes and runs the Tokenizer class.
//
//===----------------------------------------------------------------------===//

#include "Tokenizer.h"
#include <iostream> // std::cerr, std::endl
#include <sstream> // std::ostringstream
#include <cstdlib> // std::exit

int main(int argc, char** argv) {
  // Second argument [1] should be the name of the file.
  if (argc <= 1) {
    std::cerr << "Please specify a file name." << std::endl;
    std::exit(1);
  }

  try {
    Tokenizer tokenizer (argv[1]);

    // Use a stream so we can control when the token numbers get outputted.
    std::ostringstream output;

    while (!tokenizer.isEOF()) {
      tokenizer.nextToken();
      output << tokenizer.currentTokenType() << std::endl;
    }

    // Now, output all lines of token numbers.
    std::cout << output.str();
  } catch (std::string error) {
    std::cerr << error << std::endl;
  }
}
