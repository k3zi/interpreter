//===--- Tokenizer.cpp ------------------------------------------------===//
//
// Author: Kesi Maduka
// Description: TBD
//
//===----------------------------------------------------------------------===//

#include "Tokenizer.h"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <sstream>

const std::string Tokenizer::Delimiter = ";,=![]()+-*>< ";
const std::array<std::string, 13> Tokenizer::LanguageKeywordTokens = {
  "program", "begin", "end", "int", "if", "then", "else", "while", "loop",
  "read", "write", "and", "or"
};
const std::array<std::string, 17> Tokenizer::LanguageDelimiterTokens = {
  ";", ",", "=", "!", "[", "]", "(", ")", "+", "-", "*", "!=", "==", ">=",
  "<=", ">", "<"
};


Tokenizer::Tokenizer(std::string FilePath) {
  InputFile.open(FilePath);
  if (!InputFile) {
    std::cerr << "Could not open file: " << FilePath << std::endl;
    exit(1);
  }

  std::string line;
  unsigned i = 0;
  try {
    while (getline(InputFile, line)) {
      i += 1;
      scanIn(line);
    }
  } catch (std::string err) {
    std::cerr << "Error [Line " << i << "] " << err << std::endl;
    exit(1);
  }
}

std::string Tokenizer::currentToken() {
  if (CurrentTokenPosition == Tokens.size()) {
    return "eof";
  }
  return Tokens[CurrentTokenPosition];
}

bool Tokenizer::isEOF() {
  return CurrentTokenPosition > Tokens.size();
}

void Tokenizer::nextToken() {
  CurrentTokenPosition += 1;
}

bool Tokenizer::isValid(std::string Token) {
  return numberForToken(Token) >= 0;
}

bool Tokenizer::isIdentifier(std::string Token) {
  if (Token.length() == 0) {
    // Really a pre-condition check.
    return false;
  }

  for (unsigned i = 0; i < Token.length(); ++i) {
    auto c = Token.at(i);
    if (i == 0 && !isupper(c)) {
      return false;
    }

    if (!isupper(c) && !isdigit(c)) {
      std::string err = "Illegal identifier: \"";
      err += Token;
      err += "\" contains a non-uppercase, non-digit character.";
      throw err;
    }
  }

  if (Token.length() >= Tokenizer::IdentifierMaxLength) {
    std::ostringstream err;
    err << "Illegal identifier: \"" << Token << "\" has a length of ";
    err << Token.length() << ". The length of an identifier can not exceed: ";
    err << Tokenizer::IdentifierMaxLength << ".";
    throw err.str();
  }

  return true;
}

bool Tokenizer::isInteger(std::string Token) {
  if (Token.length() == 0) {
    // Really a pre-condition check.
    return false;
  }

  for (unsigned i = 0; i < Token.length(); ++i) {
    if (!isdigit(Token.at(i))) {
      return false;
    }
  }

  // Do this check after we are sure it's a digit.
  if (Token.length() > Tokenizer::IntegerMaxLength) {
    std::ostringstream err;
    err << "Illegal unsigned integer: \"" << Token << "\" has a length of ";
    err << Token.length() << ". The length of an integer can not exceed: ";
    err << Tokenizer::IntegerMaxLength << ".";
    throw err.str();
  }

  return true;
}

int Tokenizer::numberForToken(std::string Token) {
  auto it = std::find(Tokenizer::LanguageKeywordTokens.begin(), Tokenizer::LanguageKeywordTokens.end(), Token);
  if (it != Tokenizer::LanguageKeywordTokens.end()) {
    return std::distance(Tokenizer::LanguageKeywordTokens.begin(), it) + 1;
  }

  it = std::find(Tokenizer::LanguageDelimiterTokens.begin(), Tokenizer::LanguageDelimiterTokens.end(), Token);
  if (it != Tokenizer::LanguageDelimiterTokens.end()) {
    return Tokenizer::LanguageKeywordTokens.size() + std::distance(Tokenizer::LanguageDelimiterTokens.begin(), it) + 1;
  }

  if (isIdentifier(Token)) {
    return Tokenizer::IdentifierTokenRepresentation;
  }

  if (isInteger(Token)) {
    return Tokenizer::IntegerTokenRepresentation;
  }

  if (Token == "eof") {
    return Tokenizer::EOFTokenRepresentation;
  }

  throw "Unknown token: " + Token;
}

void Tokenizer::add(std::string Token) {
  if (isValid(Token)) {
    Tokens.push_back(Token);
  }
}

std::string Tokenizer::findLongestDelimiterTokenAt(std::string Input, size_t Position) {
  std::string result;
  for (size_t i = 0; i < Tokenizer::LanguageDelimiterTokens.size(); i++) {
    auto delimiterToken = Tokenizer::LanguageDelimiterTokens[i];

    // The current result is at least the length of `delimiterToken` so it by
    // default wins.
    if (!result.empty() && delimiterToken.length() <= result.length()) {
      continue;
    }

    if (delimiterToken == Input.substr(Position, delimiterToken.length())) {
      result = delimiterToken;
    }
  }

  return result;
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
      add(substring);
    }

    // `position` is the location of the Delimiter.
    // We still need to add in the Delimiter as long as it's not a space.
    substring = findLongestDelimiterTokenAt(Input, position);
    if (substring.empty()) {
      position += 1;
    } else {
      add(substring);
      position += substring.length();
    }

    lastPosition = position;
    position = Input.find_first_of(Delimiter, position);
  }

  if (lastPosition < Input.length()) {
    // Found a token at the end of a line.
    std::string substring = Input.substr(lastPosition, std::string::npos);
    if (substring.length() > 0) {
      add(substring);
    }
  }
}
