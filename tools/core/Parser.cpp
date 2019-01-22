//===--- Parser.cpp -------------------------------------------------------===//
//
// Author: ケジ
// Description: Runs the parser.
//
//===----------------------------------------------------------------------===//

#include "core/AST/AST.h"
#include "core/Parser/Parser.h"
#include <cstdlib>  // std::exit
#include <iostream> // std::cerr, std::endl
#include <sstream>  // std::ostringstream

int main(int argc, char **argv) {
  // Second argument [1] should be the name of the file.
  if (argc <= 1) {
    std::cerr << "Please specify a file name." << std::endl;
    std::exit(1);
  }

  try {
    // Construct an AST.
    AST A;
    // Thread the AST to the Parser.
    Parser P = *Parser::CreateFromFile(argv[1], A);
    // Parse into the AST.
    P.Parse();
    // Print the AST.
    A.Print();
  } catch (std::string &error) {
    std::cerr << error << std::endl;
  }
}
