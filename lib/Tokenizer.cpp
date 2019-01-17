//===--- Tokenizer.cpp ------------------------------------------------===//
//
// Author: ケジ
// Description: TBD
//
//===----------------------------------------------------------------------===//

#include "Tokenizer.h"
#include <iostream>

Tokenizer::Tokenizer(std::string FilePath) {
  InputFile.open(FilePath);
  if (!InputFile) {
    std::cerr << "Could not open file: " << FilePath << std::endl;
    exit(1);
  }

  Delimiter = ";,=![]()+-*>< ";
  std::string line;
  while (getline(InputFile, line)) {
      scanIn(line);
  }
}

std::string Tokenizer::currentToken() {
  return Tokens[CurrentTokenPosition];
}

bool Tokenizer::isEOF() {
  return Tokens[CurrentTokenPosition] == "end";
}

void Tokenizer::nextToken() {
  CurrentTokenPosition += 1;
}

void Tokenizer::scanIn(std::string Input) {
  if (Input.length() == 0) {
    return;
  }

  size_t lastPosition = 0;
  size_t position = Input.find_first_of(Delimiter);
  while (position != std::string::npos) {
    std::string substring = Input.substr(lastPosition, position - lastPosition);
    if (substring.length() > 0) {
      std::cout << "Adding token: " << substring << std::endl;
      Tokens.push_back(substring);
    }

    lastPosition = position + 1;
    position = Input.find_first_of(Delimiter, position + 1);
  }

  if (lastPosition < Input.length()) {
    std::string substring = Input.substr(lastPosition, std::string::npos);
    if (substring.length() > 0) {
      std::cout << "Adding token: " << substring << std::endl;
      Tokens.push_back(substring);
    }
  }

  std::cout << "Tokens: " << int(Tokens.size()) << std::endl;
}
