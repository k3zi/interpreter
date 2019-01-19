//===--- Tokenizer.h ------------------------------------------------------===//
//
// Author: ケジ
// Description: TBD
//
//===----------------------------------------------------------------------===//

#include <fstream> // std::ifstream

class Token;

class Tokenizer {

private:
  static const int IdentifierMaxLength = 8;
  static const int IntegerMaxLength = 8;

  Token *CurrentToken;
  std::ifstream FileStream;
  unsigned LineNumber = 1;

  void nextIdentifier(std::string tokenString);
  void nextReservedToken(std::string tokenString);
  void nextInteger(std::string tokenString);

public:
  Tokenizer (std::string FilePath);
  virtual ~Tokenizer () {
    if (FileStream.is_open())
      FileStream.close();
  };

  unsigned lineNumber() const { return LineNumber; };

  std::string currentToken();
  void nextToken();
  int currentTokenNumber();
  bool isEOF();
};
