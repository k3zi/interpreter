//===--- Tokenizer.h -----------------------------------------------------===//
//
// Author: Kesi Maduka
// Description: Defines a class that takes input from a file, reads each
//   character and forms valid tokens, breaking when an invalid token is
//   registered. This operation is greedy, so `===` is split into: `==` + `=`.
//
//===---------------------------------------------------------------------===//

#include <fstream> // std::ifstream
#include "Token.h"

// The Tokenizer class header.
class Tokenizer {

  // The maximum length of a valid identifier in CORE.
  static const unsigned IdentifierMaxLength = 8;

  // The maximum length of a valid integer in CORE.
  static const unsigned IntegerMaxLength = 8;

  // The most recent token. Upon initialization this is the undefined token
  // type: `TokenType::undefined`.
  Token CurrentToken;

  // The file stream to be opened upon initialization and closed on
  // destruction.
  std::ifstream FileStream;

  // The line number that the tokenizer is processing. Incremented on line
  // breaks. Line 1 is considered the first line. Counting starts from 1. Not
  // 0.
  unsigned LineNumber = 1;

  // The column number that the tokenizer is processing. Resets on line
  // breaks. Incremenets on character consumption. Column 1 is considered the
  // first column. Counting starts from 1. Not 0.
  unsigned ColumnNumber = 1;

  // Processes the next identifier token from the file stream. The passed in
  // `tokenString` must contain a single uppercase character. This function
  // processes up to (but not including) a character that is non-alphanumeric.
  // Identifier Format = [A-Z]+[0-9]*
  // This function will throw if:
  //   - Any of the characters after the initial character are non-uppercase
  //       Ex: ABc123.
  //   - Any of the characters after the initial character and all uppercase
  //      characters are non-digits. Ex: ABC123X.
  //   - The resulting identifier exceeds `IdentifierMaxLength`.
  // Returns: The number of characters consumed. AKA The resultant
  //   token's length.
  unsigned nextIdentifier(std::string tokenString);

  // Processes the next reserved token from the file stream. The passed in
  // `tokenString` must contain a single lowercase character.
  // Reserved Token Format = [a-z]+ and must match one of the predefined
  // tokens.
  // This function will throw if:
  //   - Any of the characters after the initial character are non-lowercase
  //       Ex: ABc123.
  //   - The result matches none of the languages identifiers.
  // Returns: The number of characters consumed. AKA The resultant
  //   token's length.
  unsigned nextReservedToken(std::string tokenString);

  // Processes the next integer token from the file stream. The passed in
  // `tokenString` must contain a single numeric character. This function
  // processes up to (but not including) a character that is non-alphanumeric.
  // Identifier Format = 0|[1-9][0-9]*
  // This function will throw if:
  //   - Any of the characters after the initial character are non-numeric
  //       Ex: 123c, 123ABC, 1x4, 45e8.
  //   - The resulting identifier exceeds `IntegerMaxLength`.
  //   - Has leading zeros. Ex: 0001, 0000, 01234.
  // Returns: The number of characters consumed. AKA The resultant
  //   token's length.
  unsigned nextInteger(std::string tokenString);

  // Internal method that retrieves the next token from the file stream. Is
  // called by public member `nextToken` and utilises: `nextIdentifier`,
  // `nextInteger`, and `nextIdentifier`.
  void consumeToken();

public:

  // The only constructor for the Tokenizer. Takes in a file path to a CORE
  // source file and opens a file stream with the initial token set to an
  // undefined token.
  Tokenizer (std::string FilePath);

  // Destructor should just close the file stream.
  virtual ~Tokenizer () {
    if (FileStream.is_open())
      FileStream.close();
  };

  // Returns the current line number of the scanner.
  unsigned lineNumber() const { return LineNumber; };

  // Returns the current line number of the scanner.
  unsigned columnNumber() const { return ColumnNumber; };

  // Getter for the current token.
  Token currentToken() {
    return CurrentToken;
  }

  // Retrieves the next token from the file stream. That token can then
  // subsequently be read by calling `Tokenizer::currentToken`. Handles error
  // outputing (specifically line numbers).
  void nextToken();

  // Whether the tokenizer (and file stream) has reached the end of token
  // output. End of file.
  bool isEOF() {
    return FileStream.eof();
  }
};
