//===--- Node+Execute.cpp -------------------------------------------------===//
//
// Author: ケジ
// Description: Implements methods for interpreting `Node`s.
//
//===----------------------------------------------------------------------===//

#include "core/AST/ASTContext.h"
#include "core/AST/Node.h"
#include "core/Parser/Parser.h"

#include <limits.h>

//===----------------------------------------------------------------------===//
// Executing: top level
//===----------------------------------------------------------------------===//

/// <prog> ::= program <decl-seq> begin <stmt-seq> end
void Prog::Execute(ASTContext &C) {
  // No need to execute declerations as they have already been processed during
  // parsing.
  // DeclSeq->Execute(C);
  StmtSeq->Execute(C);
}

//===----------------------------------------------------------------------===//
// Interpreting: sequence-like grammar rules (<x-seq> ::= <x> <x-seq>)
//===----------------------------------------------------------------------===//

/// <decl-seq> ::= <decl> | <decl> <decl-seq>
void DeclSeq::Execute(ASTContext & /*C*/) {
  assert(false && "DeclSeq should not be envoked for interpreting.");
}

/// <stmt-seq> ::= <stmt> | <stmt> <stmt-seq>
void StmtSeq::Execute(ASTContext &C) {
  Stmt->Execute(C);
  if (Seq != nullptr) {
    Seq->Execute(C);
  }
}

/// <id-list> ::= <id> | <id> <id-list>
void IdList::Execute(ASTContext & /*C*/) {
  assert(false && "IdList should not be envoked for interpreting.");
}

//===----------------------------------------------------------------------===//
// Interpreting: elements of sequence-like grammar rules
//===----------------------------------------------------------------------===//

/// <decl> ::= int <id-list>;
void Decl::Execute(ASTContext & /*C*/) {
  // We can skip running C.Initialize(Seq) because we do that before execution.
  assert(false && "Decl should not be envoked for interpreting.");
}

/// <stmt> ::= <assign> | <if> | <loop> | <in> | <out>
void Stmt::Execute(ASTContext &C) { Node->Execute(C); }

/// <id> ::= <let-seq> | <let-seq><int>
/// <id> is just a token here
void Id::Execute(ASTContext & /*C*/) {
  assert(false && "Id should not be envoked for interpreting.");
}

//===----------------------------------------------------------------------===//
// Interpreting: specific statements
//===----------------------------------------------------------------------===//

/// <assign> ::= <id> = <exp>;
void Assign::Execute(ASTContext &C) {
  assert(C.Has(Id) && "An undeclared identifier made it past the parser.");
  Exp->Execute(C);
  C.Set(Id, Exp->getValue());
}

/// <if> ::= if <cond> then <stmt-seq> end;
///        | if <cond> then <stmt-seq> else <stmt-seq> end;
void If::Execute(ASTContext &C) {
  Cond->Execute(C);
  if (Cond->getValue()) {
    IfSeq->Execute(C);
  } else if (ElseSeq != nullptr) {
    ElseSeq->Execute(C);
  }
}

/// <loop> ::= while <cond> loop <stmt-seq> end;
void Loop::Execute(ASTContext &C) {
  Cond->Execute(C);
  while (Cond->getValue()) {
    Seq->Execute(C);
    Cond->Execute(C);
  }
}

/// <in> ::= read <id-list>;
void In::Execute(ASTContext &C) { C.SetFromIn(Seq); }

/// <out> ::= write <id-list>;
void Out::Execute(ASTContext &C) { C.WriteToOut(Seq); }

/// <cond> ::= <comp> | !<cond> | [ <cond> and <cond> ] | [ <cond> or <cond> ]
void Cond::Execute(ASTContext &C) {
  if (Comp != nullptr) {
    Comp->Execute(C);
    Value = Comp->getValue();
  } else if (CondType == TokenType::exclamation_mark) {
    RHSCond->Execute(C);
    Value = !RHSCond->getValue();
  } else {
    LHSCond->Execute(C);
    RHSCond->Execute(C);
    if (CondType == TokenType::rw_and) {
      Value = LHSCond->getValue() && RHSCond->getValue();
    } else {
      Value = LHSCond->getValue() || RHSCond->getValue();
    }
  }
}

/// <comp> ::= ( <fac> <comp-op> <fac> )
void Comp::Execute(ASTContext &C) {
  LHSFac->Execute(C);
  RHSFac->Execute(C);
  int L = LHSFac->getValue();
  int R = RHSFac->getValue();
  switch (CompType) {
  case TokenType::comp_not_equal: Value = L != R; break;
  case TokenType::comp_less_than: Value = L < R; break;
  case TokenType::comp_greater_than: Value = L > R; break;
  case TokenType::comp_less_than_equal: Value = L <= R; break;
  case TokenType::comp_greater_than_equal: Value = L >= R; break;
  case TokenType::comp_equal: Value = L == R; break;
  }
}

//===----------------------------------------------------------------------===//
// Interpreting: math related statements
//===----------------------------------------------------------------------===//

/// <fac> ::= <int> | <id> | ( <exp> )
void Fac::Execute(ASTContext &C) {
  class Exp *ExpNode;
  if (Id != nullptr) {
    Value = C.Get(Id);
  } else if ((ExpNode = dynamic_cast<class Exp *>(Exp)) != nullptr) {
    ExpNode->Execute(C);
    Value = ExpNode->getValue();
  } else {
    Value = Int;
  }
}

/// <exp> ::= <term> | <term> + <exp> | <term> - <exp>
/// Remove checks if you want performance.
void Exp::Execute(ASTContext &C) {
  LHSTerm->Execute(C);
  Value = LHSTerm->getValue();
  if (ExpType == TokenType::plus) {
    RHSExp->Execute(C);
    int RHS = RHSExp->getValue();
    // If we ever added throwing to a compiled solution we could make throwing
    // for oveflow errors optional and only generate these checks when these
    // operations are wrapped in a try-catch.
    if (Value > 0 && RHS > (INT_MAX - Value)) {
      throw LocDiag(Tok, DiagType::runtime_arithmitic_x_causes_y, "addition",
                    "overflow");
    }
    if (Value < 0 && RHS < (INT_MIN - Value)) {
      throw LocDiag(Tok, DiagType::runtime_arithmitic_x_causes_y, "addition",
                    "underflow");
    }
    Value += RHS;
  } else if (ExpType == TokenType::minus) {
    RHSExp->Execute(C);
    int RHS = RHSExp->getValue();
    if (RHS > 0 && Value < (INT_MIN + RHS)) {
      throw LocDiag(Tok, DiagType::runtime_arithmitic_x_causes_y, "subtraction",
                    "underflow");
    }
    if (RHS < 0 && Value > (INT_MAX + RHS)) {
      throw LocDiag(Tok, DiagType::runtime_arithmitic_x_causes_y, "subtraction",
                    "overflow");
    }

    Value -= RHS;
  }
}

/// <term> ::= <fac> | <fac> * <term>
/// Remove checks if you want performance.
void Term::Execute(ASTContext &C) {
  LHSFac->Execute(C);
  Value = LHSFac->getValue();
  if (RHSTerm != nullptr) {
    RHSTerm->Execute(C);
    int RHS = RHSTerm->Value;
    if (RHS == 0) {
      // Short-circuit for multiplication by 0.
      Value = 0;
    } else {
      if (Value > INT_MAX / RHS) {
        throw LocDiag(Tok, DiagType::runtime_arithmitic_x_causes_y,
                      "multiplication", "overflow");
      }
      if (Value < INT_MIN / RHS) {
        throw LocDiag(Tok, DiagType::runtime_arithmitic_x_causes_y,
                      "multiplication", "underflow");
      }

      Value *= RHS;
    }
  }
}
