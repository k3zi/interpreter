//===--- Parser.cpp -------------------------------------------------------===//
//
// Author: ケジ
// Description: Implements methods for the CORE Parser.
//
//===----------------------------------------------------------------------===//

#include "core/AST/AST.h"
#include "core/AST/Node.h"
#include "core/Parser/Parser.h"

#include <iostream> // std::cerr, std::endl
#include <sstream>  // std::ostringstream

Parser::Parser(Tokenizer *t, class AST &A) : T(t), AST(A) {}

Parser *Parser::CreateFromString(std::string String, class AST &A) {
  return new Parser(Tokenizer::CreateFromString(String), A);
}

Parser *Parser::CreateFromFile(std::string FilePath, class AST &A) {
  return new Parser(Tokenizer::CreateFromFile(FilePath), A);
}

ASTContext &Parser::getContext() const { return AST.Context; }

Token Parser::currentToken() const { return T->currentToken(); }

void Parser::ConsumeToken() { T->NextToken(); }

bool Parser::ConsumeIf(TokenType::TokenType Type) {
  if (currentToken().is(Type)) {
    ConsumeToken();
    return true;
  }

  return false;
}

// Just a wrapper for `UndecoratedParse` that adds some error information into
// the error it will re-throw.
void Parser::Parse() {
  try {
    UndecoratedParse();
  } catch (LocDiag &D) {
    std::ostringstream error;
    Token t = D.getToken();
    error << "Parser Error [Line " << t.getLocation().LineNumber << ":"
          << t.getLocation().ColumnNumber << "] at token: \"" << t.getData()
          << "\". " << D.what();
    throw error.str();
  } catch (Diag &D) {
    std::ostringstream error;
    error << "Parser Error [Line " << T->lineNumber() << ":"
          << T->columnNumber() << "] at token: \"" << currentToken().getData()
          << "\". " << D.what();
    throw error.str();
  }
}

void Parser::UndecoratedParse() {
  // Read in the first token.
  ConsumeToken();
  // At the top level we only have a single program.
  AST.TranslationUnit = new Prog(this, nullptr);

  if (T->currentToken().isNot(TokenType::eof)) {
    throw Diag(DiagType::parser_expected_eof,
               T->currentToken().getData().c_str());
  }
}
