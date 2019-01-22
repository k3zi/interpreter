//===--- test_tokenizer.cpp -----------------------------------------------===//
//
// Author: ケジ
// Description: Runs tokenizer tests.
//
//===----------------------------------------------------------------------===//

#include "doctest.h"

#include "core/Tokenizer/Tokenizer.h"

#include <vector> // std::vector

Token getToken(std::string S) {
  auto t = Tokenizer::CreateFromString(S);
  t->NextToken();
  return t->currentToken();
}

TEST_SUITE("tokenizer") {
  //===--------------------------------------------------------------------===//
  // Reserved words.
  //===--------------------------------------------------------------------===//
  TEST_CASE("tokenizes reserved word: program") {
    Token t = getToken("program");
    CHECK(t.is(TokenType::rw_program));
    CHECK(t.getLocation().LineNumber == 1);
    CHECK(t.getLocation().ColumnNumber == 1);
  }

  TEST_CASE("tokenizes reserved word: int") {
    Token t = getToken("int");
    CHECK(t.is(TokenType::rw_int));
    CHECK(t.getLocation().LineNumber == 1);
    CHECK(t.getLocation().ColumnNumber == 1);
  }

  //===--------------------------------------------------------------------===//
  // Identifiers.
  //===--------------------------------------------------------------------===//
  TEST_CASE("tokenizes valid identifier: 'X'") {
    std::string id = "X";
    Token t = getToken(id);
    CHECK(t.getType() == TokenType::identifier);
    CHECK(t.getData() == id);
    CHECK(t.getLocation().LineNumber == 1);
    CHECK(t.getLocation().ColumnNumber == 1);
  }

  TEST_CASE("tokenizes valid identifier: 'Y'") {
    std::string id = "Y";
    Token t = getToken(id);
    CHECK(t.getType() == TokenType::identifier);
    CHECK(t.getData() == id);
    CHECK(t.getLocation().LineNumber == 1);
    CHECK(t.getLocation().ColumnNumber == 1);
  }

  TEST_CASE("tokenizes a valid identifier 8-characters alpha: 'ABCDEFGH'") {
    std::string id = "ABCDEFGH";
    Token t = getToken(id);
    CHECK(t.getType() == TokenType::identifier);
    CHECK(t.getData() == id);
    CHECK(t.getLocation().LineNumber == 1);
    CHECK(t.getLocation().ColumnNumber == 1);
  }

  TEST_CASE("tokenizes a valid identifier 8-characters mixed: 'A1234567'") {
    std::string id = "A1234567";
    Token t = getToken(id);
    CHECK(t.getType() == TokenType::identifier);
    CHECK(t.getData() == id);
    CHECK(t.getLocation().LineNumber == 1);
    CHECK(t.getLocation().ColumnNumber == 1);
  }

  TEST_CASE("tokenizes a valid identifier 8-characters mixed: 'ABC12345'") {
    std::string id = "ABC12345";
    Token t = getToken(id);
    CHECK(t.getType() == TokenType::identifier);
    CHECK(t.getData() == id);
    CHECK(t.getLocation().LineNumber == 1);
    CHECK(t.getLocation().ColumnNumber == 1);
  }

  TEST_CASE("throws for invalid identifier 9-character alpha: 'ABCDEFGHI'") {
    CHECK_THROWS(getToken("ABCDEFGHI"));
  }

  TEST_CASE("throws for invalid identifier 9-character mixed: 'A12345678'") {
    CHECK_THROWS(getToken("A12345678"));
  }

  TEST_CASE("throws for invalid identifier mixed: 'A1A'") {
    CHECK_THROWS(getToken("A1A"));
  }

  TEST_CASE("throws for invalid identifier mixed: 'A123456A'") {
    CHECK_THROWS(getToken("A123456A"));
  }

  //===--------------------------------------------------------------------===//
  // Integers.
  //===--------------------------------------------------------------------===//
  TEST_CASE("tokenizes a valid integer: '1'") {
    std::string integer = "1";
    Token t = getToken(integer);
    CHECK(t.getType() == TokenType::integer);
    CHECK(t.getData() == integer);
    CHECK(t.getLocation().LineNumber == 1);
    CHECK(t.getLocation().ColumnNumber == 1);
  }

  TEST_CASE("tokenizes a valid integer: '2'") {
    std::string integer = "2";
    Token t = getToken(integer);
    CHECK(t.getType() == TokenType::integer);
    CHECK(t.getData() == integer);
    CHECK(t.getLocation().LineNumber == 1);
    CHECK(t.getLocation().ColumnNumber == 1);
  }

  TEST_CASE("tokenizes a valid integer: '12345678'") {
    std::string integer = "12345678";
    Token t = getToken(integer);
    CHECK(t.getType() == TokenType::integer);
    CHECK(t.getData() == integer);
    CHECK(t.getLocation().LineNumber == 1);
    CHECK(t.getLocation().ColumnNumber == 1);
  }

  TEST_CASE("throws for random lowercase text") {
    CHECK_THROWS(getToken("gheg"));
    CHECK_THROWS(getToken("54e"));
    CHECK_THROWS(getToken("ghwy5eg"));
    CHECK_THROWS(getToken("5wy5w"));
    CHECK_THROWS(getToken("five"));
    CHECK_THROWS(getToken("not_a_reserved_word"));
  }

  //===--------------------------------------------------------------------===//
  // Misc.
  //===--------------------------------------------------------------------===//
  TEST_CASE("determines eof correctly") {
    auto t = *Tokenizer::CreateFromString("program");
    CHECK(!t.isEOF());
    t.NextToken();
    CHECK(t.isEOF());
    t.NextToken();
    CHECK(t.isEOF());
  }

  TEST_CASE("counts lines correctly and skips over breaks / spaces") {
    auto t = *Tokenizer::CreateFromString("program \nbegin \n X");
    t.NextToken();
    auto tok = t.currentToken();
    CHECK(tok.getType() == TokenType::rw_program);
    CHECK(tok.getLocation().LineNumber == 1);
    CHECK(tok.getLocation().ColumnNumber == 1);

    t.NextToken();
    tok = t.currentToken();
    CHECK(tok.getType() == TokenType::rw_begin);
    CHECK(tok.getLocation().LineNumber == 2);
    CHECK(tok.getLocation().ColumnNumber == 1);

    t.NextToken();
    tok = t.currentToken();
    CHECK(tok.getType() == TokenType::identifier);
    CHECK(tok.getLocation().LineNumber == 3);
    CHECK(tok.getLocation().ColumnNumber == 2);

    t.NextToken();
    CHECK(t.isEOF());
  }

  TEST_CASE("tokenizes a whole program #1") {
    auto t =
        *Tokenizer::CreateFromString("program\n\n  int X, Y, Z;\nbegin\n read X, "
                               "Y, Z;\n write X, Y, Z;\nend");

    std::vector<TokenType::TokenType> stream = {
        TokenType::rw_program, TokenType::rw_int,     TokenType::identifier,
        TokenType::comma,      TokenType::identifier, TokenType::comma,
        TokenType::identifier, TokenType::semicolon,  TokenType::rw_begin,
        TokenType::rw_read,    TokenType::identifier, TokenType::comma,
        TokenType::identifier, TokenType::comma,      TokenType::identifier,
        TokenType::semicolon,  TokenType::rw_write,   TokenType::identifier,
        TokenType::comma,      TokenType::identifier, TokenType::comma,
        TokenType::identifier, TokenType::semicolon,  TokenType::rw_end};

    Token tok;
    for (TokenType::TokenType &T : stream) {
      t.NextToken();
      tok = t.currentToken();
      CHECK(tok.getType() == T);
    }

    t.NextToken();
    CHECK(t.isEOF());
  }
}
