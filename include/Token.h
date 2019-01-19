//===--- Token.h ----------------------------------------------------------===//
//
// Author: Kesi Maduka
// Description: TBD
//
//===----------------------------------------------------------------------===//

#include <string>

namespace tok {

enum TokenType {
  // Undefined Token (error)
  undefined = 0, // undefined token (error)

  // Reserved Words 1-13
  rw_program, rw_begin, rw_end, rw_int, rw_if, rw_then, rw_else, rw_while,
  rw_loop, rw_read, rw_write, rw_and, rw_or,

  // Special Symbols
  semicolon, comma, equal, exclamation_mark,

  l_square_bracket, r_square_bracket, l_round_bracket, r_round_bracket,

  plus, minus, star,

  comp_not_equal, comp_equal, comp_greater_than_equal, comp_less_than_equal,
  comp_greater_than, comp_less_than,

  // Other
  integer,
  identifier,
  eof
};

}; // end namespace tok

class Token {

  // The type of token the `Data` represents.
  tok::TokenType Type;

  // The underlying data behind the token.
  std::string Data;


public:

  Token () {}

  tok::TokenType getType() const { return Type; }
  std::string getData() const { return Data; }

  void setToken(tok::TokenType T, std::string D) {
    Type = T;
    Data = D;
  }

};
