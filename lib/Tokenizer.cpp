//===--- Tokenizer.cpp ---------------------------------------------------===//
//
// Author: Kesi Maduka
// Description: Implementation file for the Tokenizer class.
//
//===---------------------------------------------------------------------===//

#include "Tokenizer.h"
#include "Token.h"
#include <sstream> // std::ostringstream
#include <cstdio> // EOF (constant)

// Tokenizer constructor. Pass in  a file path to a CORE code file.
Tokenizer::Tokenizer(std::string FilePath) {
  // Open up the input file stream.
  FileStream.open(FilePath);

  // Throw error if stream could not be opened.
  if (!FileStream) {
    std::ostringstream error;
    error << "Could not open file: \"";
    error << FilePath;
    error << "\"";
    throw error.str();
  }

  // Setup token to an undefined state.
  CurrentToken = new Token();
  CurrentToken->setToken(TokenType::undefined, "");
}

// Underlying data from token.
std::string Tokenizer::currentToken() {
  return CurrentToken->getData();
}

// Conventional enums may be implicitly returned as integers.
int Tokenizer::currentTokenType() {
  return CurrentToken->getType();
}

// Just a wrapper for `consumeToken` that adds some error information into
// the error it will re-throw.
void Tokenizer::nextToken() {
  assert(currentTokenType() != TokenType::eof && "End of token stream.");

  try {
    consumeToken();
  } catch (std::string internalError) {
    std::ostringstream error;
    error << "Error [Line " << LineNumber << ":" << ColumnNumber << "] ";
    error << internalError;
    throw error.str();
  }
}

// Processes the next token character by character.
void Tokenizer::consumeToken() {
  while (1) {
    // Get the next character.
    std::string tokenString;
    char tokenChar = FileStream.get();
    tokenString += tokenChar;

    // Initialize here since switch can't bypasses variable initialization.
    std::ostringstream error;
    unsigned charactersHandled = 0;

    // The resultant token type to be set.
    TokenType::TokenType type = TokenType::undefined;

    switch (tokenChar) {
    default: // Placing default at the top ensures no fall-through to default.
      if (isupper(tokenChar)) {
        // 100% must be an identifier if valid.
        charactersHandled = nextIdentifier(tokenString);
      }

      if (islower(tokenChar)) {
        // 100% must be a reserved token if valid.
        charactersHandled = nextReservedToken(tokenString);
      }

      if (isdigit(tokenChar)) {
        // 100% must be an integer if valid.
        charactersHandled = nextInteger(tokenString);
      }

      CurrentToken->setLocation(LineNumber, ColumnNumber);

      // If any of the above functions finished successfully
      // `charactersHandled` should have a value > 0. Otherwise there would be
      // an exception (that would skip over this flow).
      if (charactersHandled > 0) {
        ColumnNumber += charactersHandled;
        return;
      }

      error << "Unknown token: \"";
      error << tokenString;
      error << "\"";
      throw error.str();

    case '\n':
      LineNumber += 1;
      ColumnNumber = 0; // Because it will be incremented below; set it to 0.
    case '\t': case ' ': case EOF:
      if (isEOF()) {
        // In the case of reaching EOF we just need to send the last token.
        CurrentToken->setToken(TokenType::eof, "eof");
        CurrentToken->setLocation(LineNumber, ColumnNumber);
        return;
      } else {
        ColumnNumber += 1;
      }
      break;

    // Handle all the symbols.
    case ';': type = TokenType::semicolon; break;
    case ',': type = TokenType::comma; break;
    case '[': type = TokenType::l_square_bracket; break;
    case ']': type = TokenType::r_square_bracket; break;
    case '(': type = TokenType::l_round_bracket; break;
    case ')': type = TokenType::r_round_bracket; break;
    case '+': type = TokenType::plus; break;
    case '-': type = TokenType::minus; break;
    case '*': type = TokenType::star; break;
    case '>':
      if (FileStream.peek() == '=') {
        tokenString += FileStream.get();
        type = TokenType::comp_greater_than_equal;
      } else {
        type = TokenType::comp_greater_than;
      }
      break;
    case '<':
      if (FileStream.peek() == '=') {
        tokenString += FileStream.get();
        type = TokenType::comp_less_than_equal;
      } else {
        type = TokenType::comp_less_than;
      }
      break;
    case '=':
      // Be greedy...
      if (FileStream.peek() == '=') {
        tokenString += FileStream.get();
        type = TokenType::comp_equal;
      } else {
        type = TokenType::equal;
      }
      break;
    case '!':
      if (FileStream.peek() == '=') {
        tokenString += FileStream.get();
        type = TokenType::comp_not_equal;
      } else {
        type = TokenType::exclamation_mark;
      }
      break;
    }

    if (type != TokenType::undefined) {
      CurrentToken->setToken(type, tokenString);
      CurrentToken->setLocation(LineNumber, ColumnNumber);
      ColumnNumber += tokenString.size();
      return;
    }
  }
}

// Identifier = /[A-Z]+[0-9]*/ where the entire length is 8.
unsigned Tokenizer::nextIdentifier(std::string tokenString) {
  assert(tokenString.size() == 1 && isupper(tokenString.at(0))
    && "Start of identifier expected to be a single uppercase character.");

  char peekValue = FileStream.peek();
  bool containsLowercaseCharacter = false;
  // [A-Z]* (The initial character has already been taken care of).
  while (isalpha(peekValue)) {
    if (islower(peekValue)) {
      containsLowercaseCharacter = true;
    }

    tokenString += FileStream.get();
    peekValue = FileStream.peek();
  }

  // [0-9]* (But we should scan alphanumerically to capture the entire error)
  bool containsNonNumericCharacter = false;
  while (isalnum(peekValue)) {
    if (!isdigit(peekValue)) {
      containsNonNumericCharacter = true;
    }

    tokenString += FileStream.get();
    peekValue = FileStream.peek();
  }

  // Just add these two errors together.
  if (containsLowercaseCharacter || containsNonNumericCharacter) {
    std::ostringstream error;
    error << "Illegal identifier: \"";
    error << tokenString;
    error << "\".";
    if (containsLowercaseCharacter) {
      error << " May not contain lowercase characters.";
    }
    if (containsNonNumericCharacter) {
      error << " May not contain non-digit characters once a digit sequence";
      error << " has started.";
    }
    throw error.str();
  }

  // Must not exceed `IdentifierMaxLength`.
  if (tokenString.length() >= Tokenizer::IdentifierMaxLength) {
    std::ostringstream error;
    error << "Illegal identifier: \"" << tokenString << "\". Has a length of ";
    error << tokenString.length() << ". The length of an identifier may not ";
    error << "exceed " << Tokenizer::IdentifierMaxLength << ".";
    throw error.str();
  }

  CurrentToken->setToken(TokenType::identifier, tokenString);
  return tokenString.size();
}

// Reserved Token = /[a-z]+/ and must be one of the predefined reserved tokens.
unsigned Tokenizer::nextReservedToken(std::string tokenString) {
  assert(islower(tokenString.at(0))
    && "Start of reserved token expected to be lower.");

  char peekValue = FileStream.peek();
  bool containsInvalidCharacter = false;
  // Scan over all alphanumeric as groupings of alphanumeric characters
  // determine token boundaries.
  while (isalnum(peekValue)) {
    if (!islower(peekValue)) {
      containsInvalidCharacter = true;
    }

    tokenString += FileStream.get();
    peekValue = FileStream.peek();
  }


  TokenType::TokenType type = TokenType::undefined;
  if (!containsInvalidCharacter) {
    // Every character is lowercase. Check for matches.
    if (tokenString == "program") { type = TokenType::rw_program; }
    else if (tokenString == "begin") { type = TokenType::rw_begin; }
    else if (tokenString == "end") { type = TokenType::rw_end; }
    else if (tokenString == "int") { type = TokenType::rw_int; }
    else if (tokenString == "if") { type = TokenType::rw_if; }
    else if (tokenString == "then") { type = TokenType::rw_then; }
    else if (tokenString == "else") { type = TokenType::rw_else; }
    else if (tokenString == "while") { type = TokenType::rw_while; }
    else if (tokenString == "loop") { type = TokenType::rw_loop; }
    else if (tokenString == "read") { type = TokenType::rw_read; }
    else if (tokenString == "write") { type = TokenType::rw_write; }
    else if (tokenString == "and") { type = TokenType::rw_and; }
    else if (tokenString == "or") { type = TokenType::rw_or; }
    else { containsInvalidCharacter = true; }
  }

  if (containsInvalidCharacter) {
    std::ostringstream error;
    error << "Illegal token: \"";
    error << tokenString;
    error << "\". Contained invalid combination of characters.";
    throw error.str();
  }

  CurrentToken->setToken(type, tokenString);
  return tokenString.size();
}

// Identifier = /[0-9]+/ where the entire length does not exceed
// `IdentifierMaxLength`.
unsigned Tokenizer::nextInteger(std::string tokenString) {
  assert(isdigit(tokenString.at(0))
    && "Start of integer expected to be digit.");

  char peekValue = FileStream.peek();
  bool containsInvalidCharacter = false;
  // Scan over all alphanumeric as groupings of alphanumeric characters
  // determine token boundaries.
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
    error << "\". May not contain non-digit characters.";
    throw error.str();
  }

  // Integer can not exceed `IntegerMaxLength` characters.
  if (tokenString.length() >= Tokenizer::IntegerMaxLength) {
    std::ostringstream error;
    error << "Illegal integer: \"" << tokenString << "\". Has a length of ";
    error << tokenString.length() << ". The length of an integer may not ";
    error << "exceed " << Tokenizer::IntegerMaxLength << ".";
    throw error.str();
  }

  CurrentToken->setToken(TokenType::integer, tokenString);
  return tokenString.size();
}
