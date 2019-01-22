//===--- Tokenizer.h ------------------------------------------------------===//
//
// Author: ケジ
// Description: Defines a class that takes input from a file, reads each
//  character and forms valid tokens, breaking when an invalid token is
//  registered. This operation is greedy, so `===` is split into: `==` + `=`.
//
//===----------------------------------------------------------------------===//

#ifndef CORE_TOKENIZER_H
#define CORE_TOKENIZER_H

#include "Token.h"

#include <fstream> // std::ifstream
#include <istream> // std::istream

/// The Tokenizer for the CORE language.
class Tokenizer {

  /// The maximum length of a valid identifier in CORE.
  static const unsigned IdentifierMaxLength = 8;

  /// The maximum length of a valid integer in CORE.
  static const unsigned IntegerMaxLength = 8;

  /// The most recent token. Upon initialization this is the undefined token
  /// type: `TokenType::undefined`.
  Token CurrentToken;

  /// The file stream to be opened upon initialization and closed on
  /// destruction.
  std::istream *Stream;

  /// The line number that the tokenizer is processing. Incremented on line
  /// breaks. Line 1 is considered the first line. Counting starts from 1. Not
  /// 0.
  unsigned LineNumber = 1;

  /// The column number that the tokenizer is processing. Resets on line
  /// breaks. Incremenets on character consumption. Column 1 is considered the
  /// first column. Counting starts from 1. Not 0.
  unsigned ColumnNumber = 1;

  /// Processes the next identifier token from the file stream. The passed in
  /// `tokenString` must contain a single uppercase character. This function
  /// processes up to (but not including) a character that is non-alphanumeric.
  /// Identifier Format = [A-Z]+[0-9]*
  ///
  /// \param tokenString a string containg one character that begins a match for
  ///   a production rule of an <id>.
  /// \throw std::string if the identifier lexed breaks any of these rules:
  ///   - Any of the characters are non-uppercase Ex: ABc123.
  ///   - Any of the characters after a <digit> string has started are
  ///     non-digits. Ex: ABC123X.
  ///   - The resulting identifier exceeds `IdentifierMaxLength`.
  /// \return the number of characters of for the final <id> consumed. AKA The
  ///   resultant token's length.
  unsigned NextIdentifier(std::string tokenString);

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
  unsigned NextReservedToken(std::string tokenString);

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
  unsigned NextInteger(std::string tokenString);

  // Internal method that retrieves the next token from the file stream. Is
  // called by public member `NextToken` and utilises: `NextIdentifier`,
  // `NextInteger`, and `NextIdentifier`.
  void InternalNextToken();

  explicit Tokenizer(std::istream *S);

public:
  /// Constructs a tokenizer for a source file at a specifc path.
  /// \param FilePath an accessible file path to a CORE source file
  /// \throw caller must handle any exceptions from opening a std::ifstream.
  /// \return a new Tokenizer with any empty current token.
  static Tokenizer *CreateFromFile(std::string FilePath);

  /// Constructs a tokenizer for a given in-memory string.
  /// \param String a string containng a CORE language translation unit.
  /// \return a new Tokenizer with any empty current token.
  static Tokenizer *CreateFromString(std::string String);
  // The only constructor for the Tokenizer. Takes in a file path to a CORE
  // source file and opens a file stream with the initial token set to an
  // undefined token.

  // Destructor should just close the file stream.
  virtual ~Tokenizer() {
    if (auto *FileStream = dynamic_cast<std::ifstream *>(Stream)) {
      if (FileStream->is_open()) {
        FileStream->close();
      }
    }
  }

  /// Returns the current line number of the scanner.
  unsigned lineNumber() const { return LineNumber; }

  /// Returns the current line number of the scanner.
  unsigned columnNumber() const { return ColumnNumber; }

  /// Getter for the current token.
  Token currentToken() const { return CurrentToken; }

  /// Retrieves the next token from the file stream. That token can then
  /// subsequently be read by calling `Tokenizer::currentToken`. Handles error
  /// outputing (specifically line numbers).
  void NextToken();

  // Whether the tokenizer (and file stream) has reached the end of token
  // output. End of file.
  bool isEOF() const { return Stream->eof(); }
};

#endif
