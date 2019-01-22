//===--- Token.h ----------------------------------------------------------===//
//
// Author: ケジ
// Description: A class for holding token information.
//
//===----------------------------------------------------------------------===//

#ifndef CORE_TOKENIZER_TOKEN_H
#define CORE_TOKENIZER_TOKEN_H

#include "SourceLoc.h"

#include <string> // std::string
#include <utility>

namespace TokenType {

// This enum list is organized by the predetermined numbering of tokens.
enum TokenType {
  // Undefined Token (error)
  undefined = 0,

  // Reserved Words 1 - 13
  rw_program,
  rw_begin,
  rw_end,
  rw_int,
  rw_if,
  rw_then,
  rw_else,
  rw_while,
  rw_loop,
  rw_read,
  rw_write,
  rw_and,
  rw_or,

  // Special Symbols 14 - 30
  // - Punctuation 14 - 17
  semicolon,
  comma,
  equal,
  exclamation_mark,

  // - Brackets 18 - 20
  l_square_bracket,
  r_square_bracket,
  l_round_bracket,
  r_round_bracket,

  // - Operations 21 - 24
  plus,
  minus,
  star,

  // - Comparison 25 - 30
  comp_not_equal,
  comp_equal,
  comp_greater_than_equal,
  comp_less_than_equal,
  comp_greater_than,
  comp_less_than,

  // Other 31 - 33
  integer,
  identifier,
  eof,

  comp_start = comp_not_equal,
  comp_end = comp_less_than
};

}; // end namespace TokenType

class Token {

  // The type of token the `Data` represents.
  TokenType::TokenType Type;

  // The underlying data behind the token.
  std::string Data;

  // The line number of this token.
  SourceLoc Loc;

  // The column number of this token.
  unsigned ColumnNumber;

public:
  Token() = default;

  // Define getters for various members
  TokenType::TokenType getType() const { return Type; }
  std::string getData() const { return Data; }
  SourceLoc getLocation() const { return Loc; }

  bool is(TokenType::TokenType T) const { return Type == T; }
  bool isNot(TokenType::TokenType T) const { return !is(T); }

  // Just a simple setter for the token properties.
  void setToken(TokenType::TokenType T, std::string D) {
    Type = T;
    Data = D;
  }

  // Just a simple setter for the token's location in the source file.
  void setLocation(unsigned L, unsigned C) {
    Loc.LineNumber = L;
    Loc.ColumnNumber = C;
  }
};

#endif
