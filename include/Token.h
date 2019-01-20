//===--- Token.h ---------------------------------------------------------===//
//
// Author: ケジ
// Description: A class for holding token information.
//
//===---------------------------------------------------------------------===//

#include <string> // std::string

namespace TokenType {

// This enum list is organized by the predetermined numbering of tokens.
enum TokenType {
  // Undefined Token (error)
  undefined = 0, // undefined token (error)

  // Reserved Words 1 - 13
  rw_program, rw_begin, rw_end, rw_int, rw_if, rw_then, rw_else, rw_while,
  rw_loop, rw_read, rw_write, rw_and, rw_or,

  // Special Symbols 14 - 30
  // - Punctuation 14 - 17
  semicolon, comma, equal, exclamation_mark,

  // - Brackets 18 - 20
  l_square_bracket, r_square_bracket, l_round_bracket, r_round_bracket,

  // - Operations 22 - 24
  plus, minus, star,

  // - Comparison 25 - 30
  comp_not_equal, comp_equal, comp_greater_than_equal, comp_less_than_equal,
  comp_greater_than, comp_less_than,

  // Other 31 - 33
  integer,
  identifier,
  eof
};

}; // end namespace tok

class Token {

  // The type of token the `Data` represents.
  TokenType::TokenType Type;

  // The underlying data behind the token.
  std::string Data;

  // The line number of this token.
  unsigned LineNumber;

  // The column number of this token.
  unsigned ColumnNumber;


public:

  Token () {}

  // Define getters for various members
  TokenType::TokenType getType() const { return Type; }
  std::string getData() const { return Data; }
  unsigned getLineNumber() const { return LineNumber; }
  unsigned getColumnNumber() const { return ColumnNumber; }


  // Just a simple setter for the token properties.
  void setToken(TokenType::TokenType T, std::string D) {
    Type = T;
    Data = D;
  }

  // Just a simple setter for the token's location in the source file.
  void setLocation(unsigned L, unsigned C) {
    LineNumber = L;
    ColumnNumber = C;
  }

};
