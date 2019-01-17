//===--- Tokenizer.h ------------------------------------------------===//
//
// Author: Kesi Maduka
// Description: TBD
//
//===----------------------------------------------------------------------===//

#include <fstream>
#include <string>
#include <vector>
#include <array>

class Tokenizer {
private:
  static const std::string Delimiter;
  static const std::array<std::string, 30> LanguageTokens;
  static const int IntegerTokenRepresentation = 31;
  static const int IdentifierTokenRepresentation = 32;
  static const int EOFTokenRepresentation = 33;
  static const int IdentifierMaxLength = 8;

  std::vector <std::string> Tokens;
  int CurrentTokenPosition;
  std::ifstream InputFile;

  void scanIn(std::string Input);

public:
  Tokenizer (std::string FilePath);
  virtual ~Tokenizer () {};

  std::string currentToken();
  void nextToken();
  bool isEOF();
};
