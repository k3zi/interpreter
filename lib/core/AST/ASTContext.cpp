//===--- ASTContext.cpp ---------------------------------------------------===//
//
// Author: ケジ
// Description: Implements the context (symbol table) management for a CORE
//   abstract syntax tree.
//
//===----------------------------------------------------------------------===//

#include "core/AST/ASTContext.h"
#include "core/AST/Node.h"
#include "core/Diag/Diag.h"

#include <iostream> // std::cout, std::endl

void ASTContext::Declare(IdList *L) {
  if (Has(L->getId())) {
    throw Diag(DiagType::parser_identifier_redecleration,
               L->getId()->getName().c_str());
  }

  SM[L->getId()->getName()] = new IdSym;

  if (L->getSeq() != nullptr) {
    Declare(L->getSeq());
  }
}

void ASTContext::Reference(IdList *L) {
  Reference(L->getId());

  if (L->getSeq() != nullptr) {
    Reference(L->getSeq());
  }
}

void ASTContext::Reference(Id *I) {
  IdSym *Sym = SM[I->getName()];

  if (Sym == nullptr) {
    throw Diag(DiagType::parser_undeclared_identifier, I->getName().c_str());
  }

  if (!Sym->Initialized) {
    throw Diag(DiagType::parser_uninitialized_identifier, I->getName().c_str());
  }
}

bool ASTContext::Has(Id *I) {
  std::map<std::string, IdSym *>::iterator IT;
  IT = SM.find(I->getName());
  return IT != SM.end();
}

IdSym *ASTContext::FetchId(Id *I) {
  IdSym *Sym = SM[I->getName()];

  if (Sym == nullptr) {
    throw Diag(DiagType::parser_undeclared_identifier, I->getName().c_str());
  }

  return Sym;
}

void ASTContext::Initialize(Id *I) {
  IdSym *Sym = FetchId(I);

  Sym->Initialized = true;
}

void ASTContext::Initialize(IdList *L) {
  Initialize(L->getId());

  if (L->getSeq() != nullptr) {
    Initialize(L->getSeq());
  }
}

void ASTContext::Set(Id *I, int Value) {
  IdSym *Sym = FetchId(I);

  Sym->Value = Value;
  Sym->Initialized = true;
}

int ASTContext::Get(Id *I) {
  IdSym *Sym = FetchId(I);

  if (!Sym->Initialized) {
    throw Diag(DiagType::parser_uninitialized_identifier, I->getName().c_str());
  }

  return Sym->Value;
}

void ASTContext::SetFromIn(IdList *L) {
  int i;
  std::cout << L->getId()->getName() << " =? ";
  std::cin >> i;
  if (std::cin.fail()) {
    std::string Error = "Invalid integer input.";
    throw Error;
  }
  Set(L->getId(), i);

  if (L->getSeq() != nullptr) {
    SetFromIn(L->getSeq());
  }
}

void ASTContext::WriteToOut(IdList *L) {
  int i = Get(L->getId());
  std::cout << L->getId()->getName() << " = " << i << std::endl;

  if (L->getSeq() != nullptr) {
    WriteToOut(L->getSeq());
  }
}
