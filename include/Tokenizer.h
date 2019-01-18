//===--- Tokenizer.h ------------------------------------------------===//
//
// Author: ケジ
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
  static const std::array<std::string, 13> LanguageKeywordTokens;
  static const std::array<std::string, 17> LanguageDelimiterTokens;
  static const int IntegerTokenRepresentation = 31;
  static const int IdentifierTokenRepresentation = 32;
  static const int EOFTokenRepresentation = 33;
  static const int IdentifierMaxLength = 8;
  static const int IntegerMaxLength = 8;

  std::vector <std::string> Tokens;
  int CurrentTokenPosition = 0;
  std::ifstream InputFile;

  void scanIn(std::string Input);
  std::string findLongestDelimiterTokenAt(std::string Input, size_t Position);

  bool isValid(std::string Token);
  bool isIdentifier(std::string Token);
  bool isInteger(std::string Token);
  void add(std::string Token);

public:
  Tokenizer (std::string FilePath);
  virtual ~Tokenizer () {};

  std::string currentToken();
  void nextToken();
  int numberForToken(std::string Token);
  bool isEOF();
};
