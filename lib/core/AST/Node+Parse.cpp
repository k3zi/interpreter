//===--- Node+Parse.cpp ---------------------------------------------------===//
//
// Author: ケジ
// Description: Implements methods for parsing `Node`s.
//
//===----------------------------------------------------------------------===//

#include "core/AST/AST.h"
#include "core/AST/ASTContext.h"
#include "core/AST/Node.h"
#include "core/Diag/Diag.h"
#include "core/Parser/Parser.h"

#include <algorithm> // std::find
#include <iterator>  // std::inserter

//===----------------------------------------------------------------------===//
// Parsing: helper functions for analysis (initialization checks)
//===----------------------------------------------------------------------===//

void StmtSeq::SetRootSeq() { InitializedIds = new std::set<std::string>; }

void StmtSeq::SetRootSeq(StmtSeq *PrevRoot) {
  InitializedIds = new std::set<std::string>(*(PrevRoot->InitializedIds));
}

void StmtSeq::Initialize(IdList *IL) {
  Initialize(IL->getId());
  if (IL->getSeq() != nullptr) {
    Initialize(IL->getSeq());
  }
}

void StmtSeq::Initialize(Id *I) { InitializedIds->insert(I->getName()); }

void StmtSeq::Initialize(std::set<std::string> S) {
  InitializedIds->insert(S.begin(), S.end());
}

void StmtSeq::AssertInitialized(Id *I) {
  bool Result = std::find(InitializedIds->begin(), InitializedIds->end(),
                          I->getName()) != InitializedIds->end();
  if (!Result) {
    throw LocDiag(I->getToken(), DiagType::parser_uninitialized_identifier_flow,
                  I->getName().c_str());
  }
}

void StmtSeq::AssertInitialized(IdList *IL) {
  AssertInitialized(IL->getId());

  if (IL->getSeq() != nullptr) {
    AssertInitialized(IL->getSeq());
  }
}

//===----------------------------------------------------------------------===//
// Parsing: top level
//===----------------------------------------------------------------------===//

/// <prog> ::= program <decl-seq> begin <stmt-seq> end
Prog::Prog(Parser *P, class StmtSeq *SeqContext, bool SplitsContext)
    : Tok(P->currentToken()) {
  P->ConsumeIf(TokenType::rw_program, DiagType::parser_missing_reserved_word,
               "program");

  DeclSeq = new class DeclSeq(P, SeqContext);
  P->ConsumeIf(TokenType::rw_begin,
               DiagType::parser_missing_reserved_word_x_after_y, "begin",
               "declaration sequence");

  StmtSeq = new class StmtSeq(P, SeqContext);
  P->ConsumeIf(TokenType::rw_end,
               DiagType::parser_missing_reserved_word_x_after_y, "end",
               "statement sequence");
}

//===----------------------------------------------------------------------===//
// Parsing: sequence-like grammar rules (<x-seq> ::= <x> <x-seq>)
//===----------------------------------------------------------------------===//

/// <decl-seq> ::= <decl> | <decl> <decl-seq>
DeclSeq::DeclSeq(Parser *P, class StmtSeq *SeqContext, bool SplitsContext)
    : Tok(P->currentToken()) {
  Decl = new class Decl(P, SeqContext);

  if (DeclSeq::canParse(P)) {
    Seq = new DeclSeq(P, SeqContext);
  }
}

bool DeclSeq::canParse(Parser *P) { return Decl::canParse(P); }

/// <stmt-seq> ::= <stmt> | <stmt> <stmt-seq>
StmtSeq::StmtSeq(Parser *P, class StmtSeq *SeqContext, bool SplitsContext)
    : Tok(P->currentToken()) {
  // Establish the context.
  StmtSeq *TopSeqContext = SeqContext;
  if (SeqContext == nullptr) {
    // This is a root sequence.
    TopSeqContext = this;
    SetRootSeq();
  } else if (SplitsContext) {
    // This is a sub-root sequence.
    TopSeqContext = this;
    SetRootSeq(SeqContext);
  }

  Stmt = new class Stmt(P, TopSeqContext);

  if (StmtSeq::canParse(P)) {
    Seq = new StmtSeq(P, TopSeqContext);
  }
}

bool StmtSeq::canParse(Parser *P) { return Stmt::canParse(P); }

/// <id-list> ::= <id> | <id>, <id-list>
IdList::IdList(Parser *P, class StmtSeq *SeqContext, bool SplitsContext)
    : Tok(P->currentToken()) {
  Id = new class Id(P, SeqContext);

  // We have already parsed an <id> which constitutes a valid starting node for
  // an <id-list>. In order to call IdList::canParse here would mean we would
  // have to pass more information so that it knows that we have already parsed
  // an <id-list>.
  if (P->ConsumeIf(TokenType::comma)) {
    Seq = new IdList(P, SeqContext);
  }
}

bool IdList::canParse(Parser *P) { return Id::canParse(P); }

//===----------------------------------------------------------------------===//
// Parsing: elements of sequence-like grammar rules
//===----------------------------------------------------------------------===//

/// <decl> ::= int <id-list>;
Decl::Decl(Parser *P, class StmtSeq *SeqContext, bool SplitsContext)
    : Tok(P->currentToken()) {
  P->ConsumeIf(TokenType::rw_int,
               DiagType::parser_missing_reserved_word_x_at_start_of_y, "int",
               "declaration");
  Seq = new IdList(P, SeqContext);
  P->getContext().Declare(Seq);
  P->ConsumeIf(TokenType::semicolon,
               DiagType::parser_missing_x_token_after_y_in_z, ";",
               "identifier list", "decleration");
}

bool Decl::canParse(Parser *P) { return P->isToken(TokenType::integer); }

/// <stmt> ::= <assign> | <if> | <loop> | <in> | <out>
Stmt::Stmt(Parser *P, class StmtSeq *SeqContext, bool SplitsContext)
    : Tok(P->currentToken()) {
  if (Assign::canParse(P)) {
    Node = new Assign(P, SeqContext);
  } else if (If::canParse(P)) {
    Node = new If(P, SeqContext);
  } else if (Loop::canParse(P)) {
    Node = new Loop(P, SeqContext);
  } else if (In::canParse(P)) {
    Node = new In(P, SeqContext);
  } else if (Out::canParse(P)) {
    Node = new Out(P, SeqContext);
  } else {
    std::string Error = "Unrecognized statement. Valid statements include: "
                        "[assignment, if, loop, read, write].";
    throw Error;
  }
}

bool Stmt::canParse(Parser *P) {
  return Assign::canParse(P) || If::canParse(P) || Loop::canParse(P) ||
         In::canParse(P) || Out::canParse(P);
}

/// <id> ::= <let-seq> | <let-seq><int>
/// <id> is just a token here
Id::Id(Parser *P, class StmtSeq *SeqContext, bool SplitsContext)
    : Tok(P->currentToken()) {
  Name = P->currentToken().getData();
  P->ConsumeIf(TokenType::identifier, DiagType::parser_missing_x_found_y,
               "identifier", Name.c_str());
}

bool Id::canParse(Parser *P) { return P->isToken(TokenType::identifier); }

//===----------------------------------------------------------------------===//
// Parsing: specific statements
//===----------------------------------------------------------------------===//

/// <assign> ::= <id> = <exp>;
Assign::Assign(Parser *P, class StmtSeq *SeqContext, bool SplitsContext)
    : Tok(P->currentToken()) {
  Id = new class Id(P, SeqContext);
  P->ConsumeIf(TokenType::equal, DiagType::parser_missing_x_token_after_y_in_z,
               "=", "identifier", "assign-statement");
  Exp = new class Exp(P, SeqContext);
  P->ConsumeIf(TokenType::semicolon,
               DiagType::parser_missing_x_token_after_y_in_z, ";", "expression",
               "assignment");

  // Initialize the Id **after** parsing the expression. Reason: Analyze the
  // expression first. Error if the identifier being assigned to is used in the
  // expression and hasn't been initialized.

  // Initialize identifier in global context also checks that this identifier is
  // declared.
  P->getContext().Initialize(Id);
  SeqContext->Initialize(Id);
}

bool Assign::canParse(Parser *P) {
  // This can be false if used in the wrong context. I.e: decleration instead
  // of assignment.
  return P->isToken(TokenType::identifier);
}

/// <if> ::= if <cond> then <stmt-seq> end;
///        | if <cond> then <stmt-seq> else <stmt-seq> end;
If::If(Parser *P, class StmtSeq *SeqContext, bool SplitsContext)
    : Tok(P->currentToken()) {
  P->ConsumeIf(TokenType::rw_if,
               DiagType::parser_missing_reserved_word_x_at_start_of_y, "if",
               "if-statement");
  Cond = new class Cond(P, SeqContext);
  P->ConsumeIf(TokenType::rw_then,
               DiagType::parser_missing_reserved_word_x_after_y_in_z, "then",
               "conditional", "if-(else)-statement");

  // These sequences establish new contexts for flow (specifically
  // iniitialization of variables).
  IfSeq = new StmtSeq(P, SeqContext, true);
  if (P->ConsumeIf(TokenType::rw_else)) {
    ElseSeq = new StmtSeq(P, SeqContext, true);

    // Calculate the intersection between the if and else sequences and send it
    // to the root sequence. If an identifier exists in both sequences then it
    // has been 100% initialized within the if-else statement.
    std::set<std::string> IntersectOfInits;
    std::set<std::string> *IfSeqInits = IfSeq->getInitializedIds();
    std::set<std::string> *ElseSeqInits = ElseSeq->getInitializedIds();

    std::set_intersection(
        IfSeqInits->begin(), IfSeqInits->end(), ElseSeqInits->begin(),
        ElseSeqInits->end(),
        std::inserter(IntersectOfInits, IntersectOfInits.begin()));

    SeqContext->Initialize(IntersectOfInits);
  }
  // Why no else? If an if-statement initializes an Id, unless we can guarantee
  // the confition will always be true we can not pass initialized identifiers
  // to parent sequences. If both if and else sequences initialize an identifier
  // then we can.

  P->ConsumeIf(TokenType::rw_end, DiagType::parser_missing_x_token_after_y_in_z,
               "end", "statement sequence",
               ElseSeq != nullptr ? "if-else-statement" : "if-statement");
  P->ConsumeIf(TokenType::semicolon,
               DiagType::parser_missing_x_token_after_y_in_z, ";", "end",
               ElseSeq != nullptr ? "if-else-statement" : "if-statement");
}

bool If::canParse(Parser *P) { return P->isToken(TokenType::rw_if); }

/// <loop> ::= while <cond> loop <stmt-seq> end;
Loop::Loop(Parser *P, class StmtSeq *SeqContext, bool SplitsContext)
    : Tok(P->currentToken()) {
  P->ConsumeIf(TokenType::rw_while,
               DiagType::parser_missing_reserved_word_x_at_start_of_y, "while",
               "while-statement");
  Cond = new class Cond(P, SeqContext);
  P->ConsumeIf(TokenType::rw_loop,
               DiagType::parser_missing_x_token_after_y_in_z, "loop",
               "conditional", "while-statement");
  // If a identifier is initialized inside of a while statement we cannot
  // guarantee that the while will be called so mut split and not pass up the
  // initialized identifiers.
  Seq = new StmtSeq(P, SeqContext, true);
  P->ConsumeIf(TokenType::rw_end, DiagType::parser_missing_x_token_after_y_in_z,
               "end", "statement sequence", "while-statement");
  P->ConsumeIf(TokenType::semicolon,
               DiagType::parser_missing_x_token_after_y_in_z, ";", "end",
               "while-statement");
}

bool Loop::canParse(Parser *P) { return P->isToken(TokenType::rw_while); }

/// <in> ::= read <id-list>;
In::In(Parser *P, class StmtSeq *SeqContext, bool SplitsContext)
    : Tok(P->currentToken()) {
  P->ConsumeIf(TokenType::rw_read,
               DiagType::parser_missing_reserved_word_x_at_start_of_y, "read",
               "read-statement");
  Seq = new IdList(P, SeqContext);
  // Initialize identifiers in global context also checks that these identifiers
  // are declared.
  P->getContext().Initialize(Seq);
  // We are reading in values, thus initializing them.
  SeqContext->Initialize(Seq);
  P->ConsumeIf(TokenType::semicolon,
               DiagType::parser_missing_x_token_after_y_in_z, ";", "identifier",
               "read-statement");
}

bool In::canParse(Parser *P) { return P->isToken(TokenType::rw_read); }

/// <out> ::= write <id-list>;
Out::Out(Parser *P, class StmtSeq *SeqContext, bool SplitsContext)
    : Tok(P->currentToken()) {
  P->ConsumeIf(TokenType::rw_write,
               DiagType::parser_missing_reserved_word_x_at_start_of_y, "write",
               "out-statement");
  Seq = new IdList(P, SeqContext);
  // Check that this identifier is declared and globally initialized.
  P->getContext().Reference(Seq);

  // Check that this identifier has been initialized in this context.
  SeqContext->AssertInitialized(Seq);
  P->ConsumeIf(TokenType::semicolon,
               DiagType::parser_missing_x_token_after_y_in_z, ";", "identifier",
               "write-statement");
}

bool Out::canParse(Parser *P) { return P->isToken(TokenType::rw_write); }

/// <cond> ::= <comp> | !<cond> | [ <cond> and <cond> ] | [ <cond> or <cond> ]
Cond::Cond(Parser *P, class StmtSeq *SeqContext, bool SplitsContext)
    : Tok(P->currentToken()) {
  if (Comp::canParse(P)) {
    Comp = new class Comp(P, SeqContext);
  } else if (P->ConsumeIf(TokenType::exclamation_mark)) {
    // This basically serves as a way to check which alternative this is.
    CondType = TokenType::exclamation_mark;
    RHSCond = new Cond(P, SeqContext);
  } else if (P->ConsumeIf(TokenType::l_square_bracket)) {
    LHSCond = new Cond(P, SeqContext);
    if (P->ConsumeIf(TokenType::rw_and)) {
      CondType = TokenType::rw_and;
    } else {
      P->ConsumeIf(TokenType::rw_or,
                   DiagType::parser_unexpected_conditional_type_x,
                   P->currentToken().getData().c_str());
      CondType = TokenType::rw_or;
    }
    RHSCond = new Cond(P, SeqContext);
    P->ConsumeIf(TokenType::r_square_bracket,
                 DiagType::parser_missing_x_token_after_y_in_z, "]",
                 "conditional", "if-statement");
  }
}

bool Cond::canParse(Parser *P) {
  return Comp::canParse(P) || P->isToken(TokenType::exclamation_mark) ||
         P->isToken(TokenType::l_square_bracket);
}

/// <comp> ::= ( <fac> <comp-op> <fac> )
Comp::Comp(Parser *P, class StmtSeq *SeqContext, bool SplitsContext)
    : Tok(P->currentToken()) {
  P->ConsumeIf(TokenType::l_round_bracket,
               DiagType::parser_missing_x_token_at_start_of_y, "(",
               "comparison");
  LHSFac = new Fac(P, SeqContext);

  CompType = P->currentToken().getType();
  if (CompType >= TokenType::comp_start && CompType <= TokenType::comp_end) {
    CompType = P->currentToken().getType();
    P->ConsumeToken();
  } else {
    P->ConsumeIf(TokenType::comp_equal,
                 DiagType::parser_unexpected_comparison_type_x,
                 P->currentToken().getData().c_str());
  }
  RHSFac = new Fac(P, SeqContext);
  P->ConsumeIf(TokenType::r_round_bracket,
               DiagType::parser_missing_x_token_at_end_of_y, ")", "comparison");
}

bool Comp::canParse(Parser *P) {
  return P->isToken(TokenType::l_round_bracket);
}

//===----------------------------------------------------------------------===//
// Parsing: math related statements
//===----------------------------------------------------------------------===//

/// <fac> ::= <int> | <id> | ( <exp> )
Fac::Fac(Parser *P, class StmtSeq *SeqContext, bool SplitsContext)
    : Tok(P->currentToken()) {
  if (Id::canParse(P)) {
    Id = new class Id(P, SeqContext);
    // Check that this identifier is declared and globally initialized.
    P->getContext().Reference(Id);

    // Check that this identifier has been initialized in this context.
    SeqContext->AssertInitialized(Id);
  } else if (P->ConsumeIf(TokenType::l_round_bracket)) {
    Exp = new class Exp(P, SeqContext);
    P->ConsumeIf(TokenType::r_round_bracket,
                 DiagType::parser_missing_x_token_at_end_of_y_in_z, ")",
                 "expression", "factor");
  } else {
    std::string IntText = P->currentToken().getData();
    P->ConsumeIf(TokenType::integer, DiagType::parser_unexpected_factor_type_x,
                 IntText.c_str());
    Int = std::stoi(IntText);
  }
}

bool Fac::canParse(Parser *P) {
  return P->isToken(TokenType::integer) || Id::canParse(P) ||
         P->isToken(TokenType::l_round_bracket);
}

/// <exp> ::= <term> | <term> + <exp> | <term> - <exp>
Exp::Exp(Parser *P, class StmtSeq *SeqContext, bool SplitsContext)
    : Tok(P->currentToken()) {
  LHSTerm = new Term(P, SeqContext);
  ExpType = 0;
  if (P->ConsumeIf(TokenType::plus)) {
    ExpType = TokenType::plus;
    RHSExp = new Exp(P, SeqContext);
  } else if (P->ConsumeIf(TokenType::minus)) {
    ExpType = TokenType::minus;
    RHSExp = new Exp(P, SeqContext);
  }
}

bool Exp::canParse(Parser *P) { return Term::canParse(P); }

/// <term> ::= <fac> | <fac> * <term>
Term::Term(Parser *P, class StmtSeq *SeqContext, bool SplitsContext)
    : Tok(P->currentToken()) {
  LHSFac = new Fac(P, SeqContext);
  if (P->ConsumeIf(TokenType::star)) {
    RHSTerm = new Term(P, SeqContext);
  }
}

bool Term::canParse(Parser *P) { return Fac::canParse(P); }
