//===--- Tokenizer.cpp ----------------------------------------------------===//
//
// Author: ケジ
// Description: TBD
//
//===----------------------------------------------------------------------===//

#include "Tokenizer.h"
#include "Token.h"
#include <sstream> // std::ostringstream
#include <cstdio> // EOF (constant)

Tokenizer::Tokenizer(std::string FilePath) {
  // Open up the input file stream.
  FileStream.open(FilePath);

  // Throw error if stream could not be openned.
  if (!FileStream) {
    std::ostringstream error;
    error << "Could not open file: \"";
    error << FilePath;
    error << "\"";
    throw error.str();
  }

  CurrentToken = new Token();
  CurrentToken->setToken(tok::undefined, "");
  LineNumber = 1;
}

bool Tokenizer::isEOF() {
  return FileStream.eof();
}

std::string Tokenizer::currentToken() {
  return CurrentToken->getData();
}

int Tokenizer::currentTokenNumber() {
  return CurrentToken->getType();
}

void Tokenizer::nextToken() {
  assert(!isEOF() && "End of token stream.");

  while (1) {
    std::string tokenString;
    char tokenChar = FileStream.get();
    tokenString += tokenChar;
    std::ostringstream error;

    switch (tokenChar) {
    default:
      if (isupper(tokenChar)) {
        // 100% must be an identifier if valid.
        return nextIdentifier(tokenString);
      }

      if (islower(tokenChar)) {
        // 100% must be a reserved token if valid.
        return nextReservedToken(tokenString);
      }

      if (isdigit(tokenChar)) {
        // 100% must be an integer if valid.
        return nextInteger(tokenString);
      }

      error << "Unknown token: \"";
      error << tokenString;
      error << "\"";
      throw error.str();

    case '\n':
      LineNumber += 1;
    case '\t': case ' ': case EOF:
      if (isEOF()) {
        return CurrentToken->setToken(tok::eof, "eof");
      }
      break;
    case ';': return CurrentToken->setToken(tok::semicolon, tokenString);
    case ',': return CurrentToken->setToken(tok::comma, tokenString);
    case '=':
      // Be greedy...
      if (FileStream.peek() == '=') {
        tokenString += FileStream.get();
        return CurrentToken->setToken(tok::comp_equal, tokenString);
      }
      return CurrentToken->setToken(tok::equal, tokenString);
    case '!':
      if (FileStream.peek() == '=') {
        tokenString += FileStream.get();
        return CurrentToken->setToken(tok::comp_not_equal, tokenString);
      }
      return CurrentToken->setToken(tok::exclamation_mark, tokenString);
    case '[': return CurrentToken->setToken(tok::l_square_bracket, tokenString);
    case ']': return CurrentToken->setToken(tok::r_square_bracket, tokenString);
    case '(': return CurrentToken->setToken(tok::l_round_bracket, tokenString);
    case ')': return CurrentToken->setToken(tok::r_round_bracket, tokenString);
    case '+': return CurrentToken->setToken(tok::plus, tokenString);
    case '-': return CurrentToken->setToken(tok::minus, tokenString);
    case '*': return CurrentToken->setToken(tok::star, tokenString);
    case '>':
      if (FileStream.peek() == '=') {
        tokenString += FileStream.get();
        return CurrentToken->setToken(tok::comp_greater_than_equal, tokenString);
      }
      return CurrentToken->setToken(tok::comp_greater_than, tokenString);
    case '<':
      if (FileStream.peek() == '=') {
        tokenString += FileStream.get();
        return CurrentToken->setToken(tok::comp_less_than_equal, tokenString);
      }
      return CurrentToken->setToken(tok::comp_less_than, tokenString);
    }
  }
}

void Tokenizer::nextIdentifier(std::string tokenString) {
  assert(isupper(tokenString.at(0)) && "Start of identifier expected to be upper.");

  char peekValue = FileStream.peek();
  bool containsInvalidCharacter = false;
  while (isalnum(peekValue)) {
    if (islower(peekValue)) {
      containsInvalidCharacter = true;
    }

    tokenString += FileStream.get();
    peekValue = FileStream.peek();
  }

  if (containsInvalidCharacter) {
    std::ostringstream error;
    error << "Illegal identifier: \"";
    error << tokenString;
    error << "\": may not contain non-uppercase, non-digit characters.";
    throw error.str();
  }

  if (tokenString.length() >= Tokenizer::IdentifierMaxLength) {
    std::ostringstream error;
    error << "Illegal identifier: \"" << tokenString << "\" has a length of ";
    error << tokenString.length() << ". The length of an identifier may not exceed: ";
    error << Tokenizer::IdentifierMaxLength << ".";
    throw error.str();
  }

  return CurrentToken->setToken(tok::identifier, tokenString);
}

void Tokenizer::nextReservedToken(std::string tokenString) {
  assert(islower(tokenString.at(0)) && "Start of reserved token expected to be lower.");

  char peekValue = FileStream.peek();
  bool containsInvalidCharacter = false;
  while (isalnum(peekValue)) {
    if (!islower(peekValue)) {
      containsInvalidCharacter = true;
    }

    tokenString += FileStream.get();
    peekValue = FileStream.peek();
  }

  if (!containsInvalidCharacter) {
    if (tokenString == "program") {
      return CurrentToken->setToken(tok::rw_program, tokenString);
    } else if (tokenString == "begin") {
      return CurrentToken->setToken(tok::rw_begin, tokenString);
    } else if (tokenString == "end") {
      return CurrentToken->setToken(tok::rw_end, tokenString);
    } else if (tokenString == "int") {
      return CurrentToken->setToken(tok::rw_int, tokenString);
    } else if (tokenString == "if") {
      return CurrentToken->setToken(tok::rw_if, tokenString);
    } else if (tokenString == "then") {
      return CurrentToken->setToken(tok::rw_then, tokenString);
    } else if (tokenString == "else") {
      return CurrentToken->setToken(tok::rw_else, tokenString);
    } else if (tokenString == "while") {
      return CurrentToken->setToken(tok::rw_while, tokenString);
    } else if (tokenString == "loop") {
      return CurrentToken->setToken(tok::rw_loop, tokenString);
    } else if (tokenString == "read") {
      return CurrentToken->setToken(tok::rw_read, tokenString);
    } else if (tokenString == "write") {
      return CurrentToken->setToken(tok::rw_write, tokenString);
    } else if (tokenString == "and") {
      return CurrentToken->setToken(tok::rw_and, tokenString);
    } else if (tokenString == "or") {
      return CurrentToken->setToken(tok::rw_or, tokenString);
    } else {
      containsInvalidCharacter = true;
    }
  }

  if (containsInvalidCharacter) {
    std::ostringstream error;
    error << "Illegal token: \"";
    error << tokenString;
    error << "\". Token contained invalid combinatio of characters.";
    throw error.str();
  }

  return CurrentToken->setToken(tok::identifier, tokenString);
}

void Tokenizer::nextInteger(std::string tokenString) {
  assert(isdigit(tokenString.at(0)) && "Start of integer expected to be digit.");

  char peekValue = FileStream.peek();
  bool containsInvalidCharacter = false;
  while (isalnum(peekValue)) {
    if (!isdigit(peekValue)) {
      containsInvalidCharacter = true;
    }

    tokenString += FileStream.get();
    peekValue = FileStream.peek();
  }

  if (containsInvalidCharacter) {
    std::ostringstream error;
    error << "Illegal integer: \"";
    error << tokenString;
    error << "\": may not contain non-digit characters.";
    throw error.str();
  }

  if (tokenString.length() >= Tokenizer::IdentifierMaxLength) {
    std::ostringstream error;
    error << "Illegal integer: \"" << tokenString << "\" has a length of ";
    error << tokenString.length() << ". The length of an integer may not exceed: ";
    error << Tokenizer::IntegerMaxLength << ".";
    throw error.str();
  }

  return CurrentToken->setToken(tok::integer, tokenString);
}
