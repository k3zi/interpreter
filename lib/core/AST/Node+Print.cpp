//===--- Node+Print.cpp ---------------------------------------------------===//
//
// Author: ケジ
// Description: Implements methods for printing `Node`s.
//
//===----------------------------------------------------------------------===//

#include "core/AST/Node.h"
#include "core/Parser/Parser.h"

#include <iostream> // std::endl
#include <sstream>  // std::ostringstream

using std::endl;
using std::ostringstream;

std::string Indent(unsigned Ind) {
  // return std::string(Ind * 4, ' ');
  // TODO: In the future we would want to allow specifying the indentation by
  // command line arguments instead of assuming everybody likes 2 spaces of
  // indentation.
  return std::string(Ind * 2, ' ');
}

//===----------------------------------------------------------------------===//
// Printing: top level
//===----------------------------------------------------------------------===//

/// <prog> ::= program <decl-seq> begin <stmt-seq> end
void Prog::Print(ostringstream &X, unsigned Ind) {
  // TODO: It seems we need to add a space after program since several provided
  // test cases established it as a standard for consistency so...
  X << Indent(Ind) << "program " << endl;
  DeclSeq->Print(X, Ind + 1);
  X << Indent(Ind + 1) << "begin" << endl;
  StmtSeq->Print(X, Ind + 2);
  X << Indent(Ind + 1) << "end" << endl;
}

//===----------------------------------------------------------------------===//
// Printing: sequence-like grammar rules (<x-seq> ::= <x> <x-seq>)
//===----------------------------------------------------------------------===//

/// <decl-seq> ::= <decl> | <decl> <decl-seq>
void DeclSeq::Print(ostringstream &X, unsigned Ind) {
  Decl->Print(X, Ind);
  if (Seq != nullptr) {
    Seq->Print(X, Ind);
  }
}

/// <stmt-seq> ::= <stmt> | <stmt> <stmt-seq>
void StmtSeq::Print(ostringstream &X, unsigned Ind) {
  Stmt->Print(X, Ind);
  if (Seq != nullptr) {
    Seq->Print(X, Ind);
  }
}

/// <id-list> ::= <id> | <id> <id-list>
void IdList::Print(ostringstream &X, unsigned Ind) {
  Id->Print(X, Ind);
  if (Seq != nullptr) {
    X << ", ";
    Seq->Print(X, 0);
  }
}

//===----------------------------------------------------------------------===//
// Printing: elements of sequence-like grammar rules
//===----------------------------------------------------------------------===//

/// <decl> ::= int <id-list>;
void Decl::Print(ostringstream &X, unsigned Ind) {
  X << Indent(Ind) << "int ";
  Seq->Print(X, 0);
  X << ";" << endl;
}

/// <stmt> ::= <assign> | <if> | <loop> | <in> | <out>
void Stmt::Print(ostringstream &X, unsigned Ind) { Node->Print(X, Ind); }

/// <id> ::= <let-seq> | <let-seq><int>
/// <id> is just a token here
void Id::Print(ostringstream &X, unsigned Ind) { X << Indent(Ind) << Name; }

//===----------------------------------------------------------------------===//
// Printing: specific statements
//===----------------------------------------------------------------------===//

/// <assign> ::= <id> = <exp>;
void Assign::Print(ostringstream &X, unsigned Ind) {
  Id->Print(X, Ind);
  X << " = ";
  Exp->Print(X, 0);
  X << ";" << endl;
}

/// <if> ::= if <cond> then <stmt-seq> end;
///        | if <cond> then <stmt-seq> else <stmt-seq> end;
void If::Print(ostringstream &X, unsigned Ind) {
  X << Indent(Ind) << "if ";
  Cond->Print(X, 0);
  X << " then" << endl;
  IfSeq->Print(X, Ind + 1);
  if (ElseSeq != nullptr) {
    X << Indent(Ind) << "else" << endl;
    ElseSeq->Print(X, Ind + 1);
  }
  X << Indent(Ind) << "end;" << endl;
}

/// <loop> ::= while <cond> loop <stmt-seq> end;
void Loop::Print(ostringstream &X, unsigned Ind) {
  X << Indent(Ind) << "while ";
  Cond->Print(X, 0);
  X << " loop" << endl;
  Seq->Print(X, Ind + 1);
  X << Indent(Ind) << "end;" << endl;
}

/// <in> ::= read <id-list>;
void In::Print(ostringstream &X, unsigned Ind) {
  X << Indent(Ind) << "read ";
  Seq->Print(X, 0);
  X << ";" << endl;
}

/// <out> ::= write <id-list>;
void Out::Print(ostringstream &X, unsigned Ind) {
  X << Indent(Ind) << "write ";
  Seq->Print(X, 0);
  X << ";" << endl;
}

/// <cond> ::= <comp> | !<cond> | [ <cond> and <cond> ] | [ <cond> or <cond> ]
void Cond::Print(ostringstream &X, unsigned Ind) {
  if (Comp != nullptr) {
    Comp->Print(X, Ind);
  } else if (CondType == TokenType::exclamation_mark) {
    X << Indent(Ind) << "!";
    RHSCond->Print(X, 0);
  } else {
    X << Indent(Ind) << "[ ";
    LHSCond->Print(X, 0);
    if (CondType == TokenType::rw_and) {
      X << " and ";
    } else {
      X << " or ";
    }
    RHSCond->Print(X, 0);
    X << " ]";
  }
}

/// <comp> ::= ( <fac> <comp-op> <fac> )
void Comp::Print(ostringstream &X, unsigned Ind) {
  X << Indent(Ind) << "( ";
  LHSFac->Print(X, 0);
  X << " ";
  switch (CompType) {
  case TokenType::comp_not_equal: X << "!="; break;
  case TokenType::comp_less_than: X << "<"; break;
  case TokenType::comp_greater_than: X << ">"; break;
  case TokenType::comp_less_than_equal: X << "<="; break;
  case TokenType::comp_greater_than_equal: X << ">="; break;
  case TokenType::comp_equal: X << "=="; break;
  }
  X << " ";
  RHSFac->Print(X, 0);
  X << " )";
}

//===----------------------------------------------------------------------===//
// Printing: math related statements
//===----------------------------------------------------------------------===//

/// <fac> ::= <int> | <id> | ( <exp> )
void Fac::Print(ostringstream &X, unsigned Ind) {
  if (Id != nullptr) {
    Id->Print(X, Ind);
  } else if (Exp != nullptr) {
    X << Indent(Ind) << "( ";
    Exp->Print(X, 0);
    X << " )";
  } else {
    X << Int;
  }
}

/// <exp> ::= <term> | <term> + <exp> | <term> - <exp>
void Exp::Print(ostringstream &X, unsigned Ind) {
  LHSTerm->Print(X, Ind);
  if (ExpType == TokenType::plus) {
    X << " + ";
    RHSExp->Print(X, 0);
  } else if (ExpType == TokenType::minus) {
    X << " - ";
    RHSExp->Print(X, 0);
  }
}

/// <term> ::= <fac> | <fac> * <term>
void Term::Print(ostringstream &X, unsigned Ind) {
  LHSFac->Print(X, Ind);
  if (RHSTerm != nullptr) {
    X << " * ";
    RHSTerm->Print(X, 0);
  }
}
