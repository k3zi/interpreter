//===--- Tokenizer.cpp ----------------------------------------------------===//
//
// Author: ケジ
// Description: Implementation file for the Tokenizer class.
//
//===----------------------------------------------------------------------===//

#include "core/Tokenizer/Tokenizer.h"
#include <cassert> // assert
#include <cstdio>  // EOF (constant)
#include <sstream> // std::ostringstream, std::stringstream

// Tokenizer constructor. Pass in a file path to a CORE code file.
Tokenizer *Tokenizer::CreateFromFile(std::string FilePath) {
  auto S = new std::ifstream(FilePath);

  // Throw error if stream could not be opened.
  if (!S || !S->is_open()) {
    std::ostringstream error;
    error << "Could not open file: \"" << FilePath << "\".";
    throw error.str();
  }

  return new Tokenizer(S);
}

Tokenizer *Tokenizer::CreateFromString(std::string String) {
  auto S = new std::stringstream(String);
  return new Tokenizer(S);
}

Tokenizer::Tokenizer(std::istream *S) : Stream(S) {
  // Setup token to an undefined state.
  CurrentToken = Token();
  CurrentToken.setToken(TokenType::undefined, "");
  CurrentToken.setLocation(LineNumber, ColumnNumber);
}

// Just a wrapper for `InternalNextToken` that adds some error information into
// the error it will re-throw.
void Tokenizer::NextToken() {
  assert(CurrentToken.getType() != TokenType::eof && "End of token stream.");

  try {
    InternalNextToken();
  } catch (std::string internalError) {
    std::ostringstream error;
    error << "Tokenizer Error [Line " << LineNumber << ":" << ColumnNumber
          << "]. " << internalError;
    throw error.str();
  }
}

// Processes the next token character by character.
void Tokenizer::InternalNextToken() {
  // Use a while loop so we can skip over whitespace and handle end of line
  // symbols.
  while (1) {
    // Get the next character.
    std::string tokenString;
    char tokenChar = Stream->get();
    tokenString += tokenChar;

    // Initialize here since switch can't bypass variable initialization.
    std::ostringstream error;
    unsigned charactersHandled = 0;

    // The resultant token type to be set.
    TokenType::TokenType type = TokenType::undefined;

    switch (tokenChar) {
    default: // Placing default at the top ensures no fall-through to default.
      if (isupper(tokenChar)) {
        // 100% must be an identifier if valid.
        charactersHandled = NextIdentifier(tokenString);
      }

      if (islower(tokenChar)) {
        // 100% must be a reserved token if valid.
        charactersHandled = NextReservedToken(tokenString);
      }

      if (isdigit(tokenChar)) {
        // 100% must be an integer if valid.
        charactersHandled = NextInteger(tokenString);
      }

      CurrentToken.setLocation(LineNumber, ColumnNumber);

      // If any of the above functions finished successfully
      // `charactersHandled` should have a value > 0. Otherwise there would be
      // an exception (that would skip over this flow).
      if (charactersHandled > 0) {
        ColumnNumber += charactersHandled;
        return;
      }

      error << "Unknown token: \"" << tokenString << "\".";
      throw error.str();

    case '\n':
      LineNumber += 1;
      // Because "line break" is technically not a column but should reset the
      // column number.
      ColumnNumber = 0;
    case '\r':
    case '\t':
    case ' ':
    case EOF:
      if (isEOF()) {
        // In the case of reaching EOF we just need to send the last token.
        CurrentToken.setToken(TokenType::eof, "eof");
        CurrentToken.setLocation(LineNumber, ColumnNumber);
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
      if (Stream->peek() == '=') {
        tokenString += Stream->get();
        type = TokenType::comp_greater_than_equal;
      } else {
        type = TokenType::comp_greater_than;
      }
      break;
    case '<':
      if (Stream->peek() == '=') {
        tokenString += Stream->get();
        type = TokenType::comp_less_than_equal;
      } else {
        type = TokenType::comp_less_than;
      }
      break;
    case '=':
      if (Stream->peek() == '=') {
        tokenString += Stream->get();
        type = TokenType::comp_equal;
      } else {
        type = TokenType::equal;
      }
      break;
    case '!':
      if (Stream->peek() == '=') {
        tokenString += Stream->get();
        type = TokenType::comp_not_equal;
      } else {
        type = TokenType::exclamation_mark;
      }
      break;
    }

    if (type != TokenType::undefined) {
      CurrentToken.setToken(type, tokenString);
      CurrentToken.setLocation(LineNumber, ColumnNumber);
      ColumnNumber += tokenString.size();
      return;
    }
  }
}

// Identifier = /[A-Z]+[0-9]*/ where the entire length doesn't exceed
// `IdentifierMaxLength`.
unsigned Tokenizer::NextIdentifier(std::string tokenString) {
  assert(tokenString.size() == 1 && isupper(tokenString.at(0)) &&
         "Start of identifier expected to be a single uppercase character.");

  char peekValue = Stream->peek();
  bool containsLowercaseCharacter = false;
  // [A-Z]* (The initial character has already been taken care of).
  while (isalpha(peekValue)) {
    if (islower(peekValue)) {
      containsLowercaseCharacter = true;
    }

    tokenString += Stream->get();
    peekValue = Stream->peek();
  }

  // [0-9]* (But we should scan alphanumerically to capture the entire error)
  bool containsNonNumericCharacter = false;
  while (isalnum(peekValue)) {
    if (!isdigit(peekValue)) {
      containsNonNumericCharacter = true;
    }

    tokenString += Stream->get();
    peekValue = Stream->peek();
  }

  // Just add these two errors together.
  if (containsLowercaseCharacter || containsNonNumericCharacter) {
    std::ostringstream error;
    error << "Illegal identifier: \"" << tokenString << "\".";
    if (containsLowercaseCharacter) {
      error << " May not contain lowercase characters.";
    }
    if (containsNonNumericCharacter) {
      error << " May not contain non-digit characters once a digit sequence "
               "has started.";
    }
    throw error.str();
  }

  // Must not exceed `IdentifierMaxLength`.
  if (tokenString.length() > Tokenizer::IdentifierMaxLength) {
    std::ostringstream error;
    error << "Illegal identifier: \"" << tokenString << "\". Has a length of "
          << tokenString.length()
          << ". The length of an identifier may not exceed "
          << Tokenizer::IdentifierMaxLength << ".";
    throw error.str();
  }

  CurrentToken.setToken(TokenType::identifier, tokenString);
  return tokenString.size();
}

// Reserved Token = /[a-z]+/ and must be one of the predefined reserved tokens.
unsigned Tokenizer::NextReservedToken(std::string tokenString) {
  assert(islower(tokenString.at(0)) &&
         "Start of reserved token expected to be lower.");

  char peekValue = Stream->peek();
  bool containsInvalidCharacter = false;
  // Scan over all alphanumeric as groupings of alphanumeric characters
  // determine token boundaries.
  while (isalnum(peekValue)) {
    if (!islower(peekValue)) {
      containsInvalidCharacter = true;
    }

    tokenString += Stream->get();
    peekValue = Stream->peek();
  }

  TokenType::TokenType type = TokenType::undefined;
  if (!containsInvalidCharacter) {
    // Every character is lowercase. Check for matches.
    if (tokenString == "program") {
      type = TokenType::rw_program;
    } else if (tokenString == "begin") {
      type = TokenType::rw_begin;
    } else if (tokenString == "end") {
      type = TokenType::rw_end;
    } else if (tokenString == "int") {
      type = TokenType::rw_int;
    } else if (tokenString == "if") {
      type = TokenType::rw_if;
    } else if (tokenString == "then") {
      type = TokenType::rw_then;
    } else if (tokenString == "else") {
      type = TokenType::rw_else;
    } else if (tokenString == "while") {
      type = TokenType::rw_while;
    } else if (tokenString == "loop") {
      type = TokenType::rw_loop;
    } else if (tokenString == "read") {
      type = TokenType::rw_read;
    } else if (tokenString == "write") {
      type = TokenType::rw_write;
    } else if (tokenString == "and") {
      type = TokenType::rw_and;
    } else if (tokenString == "or") {
      type = TokenType::rw_or;
    } else {
      containsInvalidCharacter = true;
    }
  }

  if (containsInvalidCharacter) {
    std::ostringstream error;
    error << "Illegal token: \"" << tokenString
          << "\". Contains invalid combination of characters.";
    throw error.str();
  }

  CurrentToken.setToken(type, tokenString);
  return tokenString.size();
}

// Identifier = /0|[1-9][0-9]*/ where the entire length doesn't exceed
// `IntegerMaxLength`.
unsigned Tokenizer::NextInteger(std::string tokenString) {
  assert(isdigit(tokenString.at(0)) &&
         "Start of integer expected to be digit.");

  char peekValue = Stream->peek();
  bool containsInvalidCharacter = false;
  // Scan over all alphanumeric as groupings of alphanumeric characters
  // determine token boundaries.
  while (isalnum(peekValue)) {
    if (!isdigit(peekValue)) {
      containsInvalidCharacter = true;
    }

    tokenString += Stream->get();
    peekValue = Stream->peek();
  }

  if (containsInvalidCharacter) {
    std::ostringstream error;
    error << "Illegal integer: \"" << tokenString
          << "\". May not contain non-digit characters.";
    throw error.str();
  }

  // Integers can't start with 0 (except 0).
  if (tokenString.length() > 1 && tokenString.at(0) == '0') {
    std::ostringstream error;
    error << "Illegal integer: \"" << tokenString
          << "\". May not contain leading zeros.";
    throw error.str();
  }

  // Integer can not exceed `IntegerMaxLength` characters.
  if (tokenString.length() > Tokenizer::IntegerMaxLength) {
    std::ostringstream error;
    error << "Illegal integer: \"" << tokenString << "\". Has a length of "
          << tokenString.length()
          << ". The length of an integer may not exceed "
          << Tokenizer::IntegerMaxLength << ".";
    throw error.str();
  }

  CurrentToken.setToken(TokenType::integer, tokenString);
  return tokenString.size();
}
