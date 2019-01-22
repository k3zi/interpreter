//===--- AST.cpp ----------------------------------------------------------===//
//
// Author: ケジ
// Description: TBD
//
//===----------------------------------------------------------------------===//

#include "core/AST/AST.h"
#include "core/AST/ASTContext.h"
#include "core/AST/Node.h"
#include "core/Diag/Diag.h"

#include <iostream> // std::cout

AST::AST() : Context(*new ASTContext()) {}

AST::~AST() {
  if (TranslationUnit != nullptr) delete TranslationUnit;
}

void AST::Print() {
  assert(TranslationUnit != nullptr && "Can not print an empty AST.");

  std::ostringstream X;
  Print(X);
  std::cout << X.str();
}

void AST::Print(std::ostringstream &X) {
  assert(TranslationUnit != nullptr && "Can not print an empty AST.");

  TranslationUnit->Print(X, 0);
}

void AST::Execute() {
  assert(TranslationUnit != nullptr && "Can not interpret an empty AST.");

  try {
    TranslationUnit->Execute(Context);
  } catch (LocDiag &D) {
    std::ostringstream error;
    Token t = D.getToken();
    error << "Runtime Error [Line " << t.getLocation().LineNumber << ":"
          << t.getLocation().ColumnNumber << "] at token: \"" << t.getData()
          << "\". " << D.what();
    throw error.str();
  } catch (Diag &D) {
    std::ostringstream error;
    error << "Runtime Error: " << D.what();
    throw error.str();
  }
}
