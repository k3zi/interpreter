//===--- test_parser.cpp --------------------------------------------------===//
//
// Author: ケジ
// Description: Runs parser tests.
//
//===----------------------------------------------------------------------===//

#include "doctest.h"

#include "core/AST/AST.h"
#include "core/Diag/Diag.h"
#include "core/Parser/Parser.h"

#include <iostream> // std::cout, std::endl
#include <vector>   // std::vector

void testPrint(std::string Test) {
  AST A;
  Parser P = *Parser::CreateFromString(Test, A);
  P.Parse();

  std::ostringstream X;
  A.Print(X);
  CHECK(X.str() == Test);
}

TEST_SUITE("parser") {
  //===--------------------------------------------------------------------===//
  // Reserved words.
  //===--------------------------------------------------------------------===//
  TEST_CASE("throws for duplicate decleration") {
    AST A;
    Parser P = *Parser::CreateFromString(
        "program\r\nint X,X;\r\nbegin \r\n  X=13;\r\n  while(X>10)loop\r\n    "
        "write X;\r\n    read X;\r\n    X=X+13;\r\n    X=X-15;\r\n    "
        "X=X*3;\r\n    end;\r\nend",
        A);

    CHECK_THROWS_WITH(P.UndecoratedParse(),
                      "Redeclaration of identifier: 'X'.");
  }

  TEST_CASE("throws for missing decleration") {
    AST A;
    Parser P = *Parser::CreateFromString(
        "program\r\nint X;\r\nbegin \r\n  X=13;\r\n  while(X>10)loop\r\n    "
        "write X;\r\n    read Y;\r\n    X=X+13;\r\n    X=X-15;\r\n    "
        "X=X*3;\r\n    end;\r\nend",
        A);

    CHECK_THROWS_WITH(P.UndecoratedParse(),
                      "Missing decleration for identifier: 'Y'.");
  }

  TEST_CASE("throws for uninitialized variable at root statement sequence") {
    AST A;
    Parser P = *Parser::CreateFromString(
        "program\r\nint X, Y;\r\nbegin \r\n  X=Y+13;\r\n  while(X>10)loop\r\n  "
        "write X;\r\n    read X;\r\n    X=X+13;\r\n    X=X-15;\r\n    "
        "X=X*3;\r\n    end;\r\nend",
        A);

    CHECK_THROWS_WITH(P.UndecoratedParse(),
                      "Identifier used before initialization: 'Y'.");
  }

  TEST_CASE("throws for uninitialized variable within while") {
    AST A;
    Parser P = *Parser::CreateFromString(
        "program\r\nint X, Y;\r\nbegin \r\n  X=13;\r\n  while(X>10)loop\r\n  "
        "write X;\r\n    read X;\r\n    X=Y+13;\r\n    X=X-15;\r\n    "
        "X=X*3;\r\n    end;\r\nend",
        A);
  }

  TEST_CASE("throws for possible uninitialized variable used after while") {
    AST A;
    Parser P = *Parser::CreateFromString(
        "program\r\nint X, Y;\r\nbegin \r\n  X=5;\r\n  while(X>10)loop\r\n  "
        "write X;\r\n    read X;\r\n    Y=5;\r\n    X=X-15;\r\n    "
        "X=X*3;\r\n    end;\r\n write Y;\r\nend",
        A);

    CHECK_THROWS_WITH(P.UndecoratedParse(),
                      "Not all paths of the program initialize 'Y' before it "
                      "is used here. This may be a false-positive but can be "
                      "indicative of a design flaw in your program.");
  }

  TEST_CASE("throws for uninitialized variable complex logic") {
    AST A;
    Parser P = *Parser::CreateFromString("program\n  int X, Y;\n  begin\n    "
                                         "read X;\n    if ( X > 10 ) then\n  "
                                         "    read Y;\n      X = X - Y;\n    "
                                         "else\n      write X;\n    end;\n   "
                                         " write Y;\n  end\n",
                                         A);

    CHECK_THROWS_WITH(P.UndecoratedParse(),
                      "Not all paths of the program initialize 'Y' before it "
                      "is used here. This may be a false-positive but can be "
                      "indicative of a design flaw in your program.");
  }

  TEST_CASE("throws error for missing end and semicolon") {
    AST A;
    Parser P = *Parser::CreateFromString(
        "program\r\nint X,Y;\r\nbegin \r\n  X=13;\r\n  Y=0;\r\n  if (X>10) "
        "then\r\n    write X,Y;\r\n    X=X-1;\r\n  else\r\n    write "
        "Y,X;\r\n  "
        "  X=X+1;\r\nend",
        A);

    CHECK_THROWS_WITH(P.UndecoratedParse(),
                      "Expected ';' token after 'end' in if-else-statement.");
  }

  TEST_CASE("throws error for junk at end of program") {
    AST A;
    Parser P = *Parser::CreateFromString(
        "program\r\nint X, Y;\r\nbegin \r\n  X=13;\r\n  while (X>10) "
        "loop\r\n  "
        "  write X;\r\n    read X;\r\n    "
        "end;\r\nend\r\n;\r\n;"
        "\r\nloop\r\nwhile\r\nif\r\nbegin\r\nend\r\nprogram\r\n\r\n\r\n\r\npr"
        "og"
        "ram\r\n\r\nbegin\r\nX\r\nY 103 12345678\r\nX\r\n12\r\nread",
        A);

    CHECK_THROWS_WITH(
        P.UndecoratedParse(),
        "Token found after end of program: ';'. Expected to reach "
        "end-of-file after parsing a program.");
  }

  TEST_CASE("throws error for malformed loop") {
    AST A;
    Parser P = *Parser::CreateFromString(
        "program\r\nint X, Y;\r\nbegin \r\n  X=13;\r\n  while (X>10) \r\n    "
        "write X;\r\n    read X;\r\n    end;\r\nend",
        A);

    CHECK_THROWS(P.UndecoratedParse());
  }

  TEST_CASE("throws error for malformed assign #1") {
    AST A;
    Parser P = *Parser::CreateFromString(
        "program\r\nint X,Y;\r\nbegin \r\n  X=13;\r\n  Y=0;\r\n  X=;\r\nend",
        A);

    CHECK_THROWS(P.UndecoratedParse());
  }

  TEST_CASE("throws error for malformed assign #2") {
    AST A;
    Parser P = *Parser::CreateFromString("program\r\nint X,Y;\r\nbegin \r\n  "
                                         "X=13;\r\n  Y=0;\r\n  =X+1;\r\nend",
                                         A);

    CHECK_THROWS(P.UndecoratedParse());
  }

  TEST_CASE("throws error for invalid symbol (tokenizer error)") {
    AST A;
    Parser P = *Parser::CreateFromString(
        "program\r\nint X, Y;\r\nbegin \r\n  X=13;\r\n  while [(X>10) && "
        "(X<12)] loop\r\n    write X;\r\n    read X;\r\n    end;\r\nend\r\n",
        A);

    CHECK_THROWS(P.UndecoratedParse());
  }

  TEST_CASE("parses valid program #1") {
    AST A;
    Parser P =
        *Parser::CreateFromString("program int X, Y; begin X=13; while (X>10) "
                                  "loop write X; read X; end; end",
                                  A);

    P.Parse();
  }

  TEST_CASE("prints: validAllOneLine") {
    testPrint("program \n  int X, Y;\n  begin\n    X = 13;\n    while ( X > "
              "10 ) loop\n      write X;\n      read X;\n    end;\n  end\n");
  }

  TEST_CASE("prints: validAllOneLineMinimalWhitespace") {
    testPrint("program \n  int X, Y;\n  begin\n    X = 13;\n    while ( X > "
              "10 ) loop\n      write X;\n      read X;\n    end;\n  "
              "end\n");
  }

  TEST_CASE("prints: validAllSimpleExpressions") {
    testPrint("program \n  int X, Y;\n  begin\n    X = 13;\n    while ( X > "
              "10 ) loop\n      write X;\n      read X;\n      X = X + 13;\n "
              "     X = X - 15;\n      X = X * 3;\n    end;\n  end\n");
  }

  TEST_CASE("prints: validBooleanComplex") {
    testPrint("program \n  int X, Y;\n  begin\n    X = 20;\n    Y = "
              "0;\n    while [ [ ( X > 10 ) and ( Y < 10 ) ] or [ ( X < 20 ) "
              "and ( Y > 0 ) ] ] loop\n      write X, Y;\n      X = X - "
              "1;\n      Y = Y - 1;\n    end;\n  end\n");
  }

  TEST_CASE("prints: validComplexExpressions") {
    testPrint("program \n  int X, Y;\n  begin\n    X = 13;\n    while ( X > "
              "10 ) loop\n      write X;\n      read X;\n      X = ( X + 13 ) "
              "* X - ( ( X * 3 ) - 2 * X );\n    end;\n  end\n");
  }

  TEST_CASE("prints: validMinimalWhitespace") {
    testPrint("program \n  int X, Y;\n  begin\n    X = 13;\n    while ( X > 10 "
              ") loop\n      write X;\n      read X;\n    end;\n  end\n");
  }

  TEST_CASE("prints: validNotCond") {
    testPrint("program \n  int X, Y;\n  begin\n    X = 35;\n    Y = 25;\n    "
              "while !( X < Y ) loop\n      X = X - Y;\n    end;\n  end\n");
  }

  TEST_CASE("prints: validOneTokenPerLine") {
    testPrint("program \n  int X, Y;\n  begin\n    X = 13;\n    while ( X > 10 "
              ") loop\n      write X;\n      read X;\n    end;\n  end\n");
  }

  TEST_CASE("prints: validTypicalIfElse") {
    testPrint("program \n  int X, Y;\n  begin\n    X = 13;\n    Y = 0;\n    if "
              "( X > 10 ) then\n      write X, Y;\n      X = X - 1;\n    "
              "else\n      write Y, X;\n      X = X + 1;\n    end;\n  end\n");
  }

  TEST_CASE("prints: validTypicalLoop") {
    testPrint("program \n  int X, Y;\n  begin\n    X = 13;\n    while ( X > 10 "
              ") loop\n      write X;\n      read X;\n    end;\n  end\n");
  }

  TEST_CASE("prints: validWhitespaceBetweenTokens") {
    testPrint("program \n  int X, Y;\n  begin\n    X = 13;\n    if ( X > 10 ) "
              "then\n      write X;\n      read X;\n    else\n      X = X + "
              "1;\n    end;\n  end\n");
  }
}
