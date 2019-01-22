//===--- Node.h -----------------------------------------------------------===//
//
// Author: ケジ
// Description: Details the various types of nodes for the AST and their
// members.
//
//===----------------------------------------------------------------------===//

#ifndef CORE_AST_NODE_H
#define CORE_AST_NODE_H

#include "core/Tokenizer/Token.h"

#include <cassert> // assert
#include <set>     // std::set
#include <string>  // std::string
#include <vector>  // std::vector

// Forward decleration
class Parser;
class ASTContext;
class IdSym;

/// A generic node of the AST. Subclasses need to override the virtual methods.
class Node {
public:
  /// Pretty print the node back to the source language.
  /// \param X the stream which to append to.
  /// \param Indent the level of indent to print at in the stream.
  virtual void Print(std::ostringstream &X, unsigned Indent) {}

  /// Executes / evaluates the node.
  /// \param C the context against which the node will execute / evaluate for.
  virtual void Execute(ASTContext &C) {}
};

class StmtSeq;

// This macro allows us to simplify the definitions of the nodes for the CORE
// abstract syntax tree.
#define DEFINE_NODE(CLASS, PRIVATE_MEMBERS, DESTRUCTION)                       \
  class CLASS : public Node {                                                  \
    /* The staring location of the first token parsed by this node. */         \
    Token Tok;                                                                 \
                                                                               \
    PRIVATE_MEMBERS                                                            \
                                                                               \
  public:                                                                      \
    CLASS(Parser *P, class StmtSeq *SeqContext, bool SplitsContext);           \
    CLASS(Parser *P, class StmtSeq *SeqContext)                                \
        : CLASS(P, SeqContext, false){};                                       \
                                                                               \
    /*                                                                         \
     * \brief Whether the node can parse against the grammar at the current    \
     * token.                                                                  \
     *                                                                         \
     * \discussion By keeping a seperate `canParse` method we are essemtialy   \
     * supporting DRY and information hiding. The more general alternative     \
     * would be to test if the current token is of a kind that our node        \
     * supports parsing (or any of the starting nonterminals in one of our     \
     * node's production rules supports parsing); this is essemtialy what      \
     * `canParse` does but it keeps associated code together. A node that      \
     * parses a <fac> only has to know that it can parse a <fac>.              \
     * Parsing a <fac> is defined in `Fac::canParse` as the conjuction of      \
     * being able to parse an <id>, <int>, or ( <expr> ). The advantage is     \
     * that the implementer of a node does not have to worry about what it's   \
     * starting nonterminals look like. They only have to pay attenion to      \
     * their production rules. Also if this parser ever expanded and a lower   \
     * leaf aquired a new production rule the changes wouldn't have to         \
     * propogate up.                                                           \
     */                                                                        \
    static bool canParse(Parser *P);                                           \
                                                                               \
    /*                                                                         \
     * Print the given no with the passed in indetntation to the end of the    \
     * stream.                                                                 \
     */                                                                        \
    void Print(std::ostringstream &X, unsigned Indent) override;               \
    void Execute(ASTContext &C) override;                                      \
    Token getToken() const { return Tok; }                                     \
    virtual ~CLASS(){DESTRUCTION};                                             \
  };

// clang-format off
/// The Node class representing `<id>` in CORE.
DEFINE_NODE(Id,
  /// The name of an identifier as it appears in the source code.
  std::string Name;
public:
  // Getters for private members we want public.
  std::string getName() const { return Name; }
,)

/// The Node class representing `<id-list>` in CORE.
DEFINE_NODE(IdList,
  /// The identifier that appears in either alternate of an <id-list>.
  Id *Id = nullptr;

  /// The sequence present in the second alternative. After initialization this
  /// value may be empty.
  IdList *Seq = nullptr;
public:
  // Getters for private members we want public.
  class Id *getId() const { return Id; }
  IdList *getSeq() const { return Seq; }
, delete Id; if (Seq != nullptr) delete Seq;)

/// The Node class representing `<decl>` in CORE.
DEFINE_NODE(Decl,
  IdList *Seq = nullptr;
, delete Seq;)

/// The Node class representing `<decl-seq>` in CORE.
DEFINE_NODE(DeclSeq,
  /// The decleration that appears in either alternate of a <decl-seq>.
  Decl *Decl = nullptr;

  /// The sequence present in the second alternative. After initialization this
  /// value may be empty.
  DeclSeq *Seq = nullptr;
, delete Decl; if (Seq != nullptr) delete Seq;)

/// The Node class representing `<stmt>` in CORE.
DEFINE_NODE(Stmt,
  Node *Node = nullptr;
, delete Node;)

/// The Node class representing `<stmt-seq>` in CORE.
DEFINE_NODE(StmtSeq,
  Stmt *Stmt;
  StmtSeq *Seq = nullptr;
  /// The set of identifiers that have been initialized within this statement
  /// sequence. Only one of these will exist for a sequence chain.
  std::set<std::string> *InitializedIds;

public:
  /// Initializes an Id node in the given context. To be called when an Id's
  /// value is changed.
  void Initialize(Id *I);

  /// Initializes an Id, given by the passed in string, in the given context.
  /// To be called when an Id's value is changed.
  void Initialize(std::set<std::string> S);

  /// A convinence method to call `Initialize` on each `Id` in an `IdList`.
  ///
  /// \see StmtSeq::Initialize(Id *I)
  void Initialize(IdList *IL);

  /// Checks that the passed in identifier has been initialized in this context.
  /// \throw LocDiag if it has not been initialized.
  void AssertInitialized(Id *I);

  /// A convinence method to call `AssertInitialized` on each `Id` in an
  /// `IdList`.
  ///
  /// \see StmtSeq::AssertInitialized(Id *I)
  void AssertInitialized(IdList *IL);
  void SetRootSeq();
  void SetRootSeq(StmtSeq *PrevRoot);
  std::set<std::string> *getInitializedIds() const {
    assert(InitializedIds != nullptr && "Should not get initialized ids here.");
    return InitializedIds;
  };
, delete Stmt; if (Seq != nullptr) delete Seq;)

/// The Node class representing `<prog>` in CORE.
DEFINE_NODE(Prog,
  /// The string of decleration sequences parsed by the program. This should not
  /// be null after initialization.
  DeclSeq *DeclSeq= nullptr;

  /// The string of statement sequences parsed by the program. This should not
  /// be null after initialization.
  StmtSeq *StmtSeq = nullptr;
, delete DeclSeq; delete StmtSeq;)

class Exp;

/// The Node class representing `<fac>` in CORE.
DEFINE_NODE(Fac,
  int Int;

  Id *Id = nullptr;

  // POSSIBLE TODO: Move all the destructors to the implementation file.
  /// This will be the express part of a factor but since Exp is not yet
  /// declared we do not have the full type information for it so C++ will
  /// complain that it is not safe to destroy it.
  Node *Exp = nullptr;

  /// The value of a factor to be properly set after it is interpreted.
  int Value = 0;
public:
  // Getters for private members we want public.
  int getValue() const { return Value; }
, if (Id != nullptr) delete Id; if (Exp != nullptr) delete Exp;)

/// The Node class representing `<term>` in CORE.
DEFINE_NODE(Term,
  Fac *LHSFac = nullptr;
  Term *RHSTerm = nullptr;

  /// The value of a term to be properly set after it is interpreted.
  int Value = 0;
public:
  // Getters for private members we want public.
  int getValue() const { return Value; }
, delete LHSFac; delete RHSTerm;)

/// The Node class representing `<exp>` in CORE.
DEFINE_NODE(Exp,
  unsigned ExpType = 0;

  Term *LHSTerm = nullptr;
  Exp *RHSExp = nullptr;

  /// The value of an expression to be properly set after it is interpreted.
  int Value = 0;
public:
  // Getters for private members we want public.
  int getValue() const { return Value; }
, delete LHSTerm; delete RHSExp;)

/// The Node class representing `<comp>` in CORE.
DEFINE_NODE(Comp,
  unsigned CompType = 0;

  Fac *LHSFac = nullptr;
  Fac *RHSFac = nullptr;

  /// The value of a condition to be properly set after it is interpreted.
  bool Value = false;
public:
  // Getters for private members we want public.
  bool getValue() const { return Value; }
, delete LHSFac; delete RHSFac;)

/// The Node class representing `<cond>` in CORE.
DEFINE_NODE(Cond,
  Comp *Comp = nullptr;
  Cond *LHSCond = nullptr;
  Cond *RHSCond = nullptr;

  unsigned CondType = 0;

  /// The value of a condition to be properly set after it is interpreted.
  bool Value = false;
public:
  // Getters for private members we want public.
  bool getValue() const { return Value; }
, delete LHSCond; delete RHSCond; delete Comp;)

/// The Node class representing `<if>` in CORE.
DEFINE_NODE(If,
  Cond *Cond = nullptr;
  StmtSeq *IfSeq = nullptr;
  StmtSeq *ElseSeq = nullptr;
, delete Cond; delete IfSeq; delete ElseSeq;)

/// The Node class representing `<in>` in CORE.
DEFINE_NODE(In,
  /// The <id-list> to read in from the user.
  IdList *Seq = nullptr;
, delete Seq;)

/// The Node class representing `<out>` in CORE.
DEFINE_NODE(Out,
  /// The <id-list> to write out to the user.
  IdList *Seq = nullptr;
, delete Seq;)

/// The Node class representing `<assign>` in CORE.
DEFINE_NODE(Assign,
  /// The identifier to be assigned to.
  Id *Id = nullptr;

  /// The expression that will be assigned to the identifier.
  Exp *Exp = nullptr;
, delete Id; delete Exp;)

/// The Node class representing `<loop>` in CORE.
DEFINE_NODE(Loop,
  /// The condition required to be true to enter and continue executing the
  /// loop.
  Cond *Cond = nullptr;

  /// The sequence of statements to be executed as the body of the loop.
  StmtSeq *Seq = nullptr;
, delete Cond; delete Seq;)

// clang-format on
#undef DEFINE_NODE

#endif
