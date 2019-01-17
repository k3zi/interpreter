//===--- Tokenizer.cpp ------------------------------------------------===//
//
// Author: ケジ
// Description: TBD
//
//===----------------------------------------------------------------------===//

#include "Tokenizer.h"
#include <iostream>

const std::string Tokenizer::Delimiter = ";,=![]()+-*>< ";
const std::array<std::string, 30> Tokenizer::LanguageTokens = {
  "program", "begin", "end", "int", "if", "then", "else", "while", "loop",
  "read", "write", "and", "or", ";", ",", "=", "!", "[", "]", "(", ")", "+",
  "-", "*", "!=", "==", ">=", "<=", ">", "<"
};

Tokenizer::Tokenizer(std::string FilePath) {
  InputFile.open(FilePath);
  if (!InputFile) {
    std::cerr << "Could not open file: " << FilePath << std::endl;
    exit(1);
  }

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

// Retrieve all the tokens from the given line input and place them in `Tokens`.
void Tokenizer::scanIn(std::string Input) {
  if (Input.length() == 0) {
    return;
  }

  size_t lastPosition = 0;
  size_t position = Input.find_first_of(Delimiter);
  while (position != std::string::npos) {
    std::string substring = Input.substr(lastPosition, position - lastPosition);
    if (substring.length() > 0) {
      // This part should be anything that is not in `Delimiter`.
      Tokens.push_back(substring);
    }

    // `position` is the location of the Delimiter.
    // We still need to add in the Delimiter as long as it's not a space.
    substring = Input.substr(position, 1);
    if (substring.length() > 0 && substring != " ") {
      // This part should be anything that is in `Delimiter`.
      Tokens.push_back(substring);
    }

    lastPosition = position + 1;
    position = Input.find_first_of(Delimiter, position + 1);
  }

  if (lastPosition < Input.length()) {
    // Found a token at the end of a line.
    std::string substring = Input.substr(lastPosition, std::string::npos);
    if (substring.length() > 0) {
      Tokens.push_back(substring);
    }
  }
}
