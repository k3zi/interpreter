//===--- Tokenizer.h ------------------------------------------------===//
//
// Author: ケジ
// Description: TBD
//
//===----------------------------------------------------------------------===//

#include <fstream>
#include <string>
#include <vector>

class Tokenizer {
private:
  std::string Delimiter;
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
