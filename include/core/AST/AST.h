//===--- AST.h ------------------------------------------------------------===//
//
// Author: ケジ
// Description: An abstract syntax tree containing a translation unit and a
// context object.
//
//===----------------------------------------------------------------------===//

#ifndef CORE_AST_H
#define CORE_AST_H

#include <sstream>

// Forward declarations:
class ASTContext;
class Node;

/// An abstract syntax tree for the CORE language.
class AST {
  /// Parser is the only one who needs to directly access the translation unit
  /// in order to build the AST.
  friend class Parser;

  /// The translation unit for a CORE language program.
  Node *TranslationUnit = nullptr;

  /// Contextual informatiom about the the tree to be used for semantical
  /// analysis and the execution phase. Only the Parser needs to acces this.
  ASTContext &Context;

public:
  /// A constructor for an empty abstract syntax tree.
  AST();

  /// A deconstructor that should handle deleting the tree.
  ~AST();

  /// Prints the AST to std::cout.
  void Print();

  /// Prints the AST to the passed in string stream.
  void Print(std::ostringstream &X);

  /// Executes the AST using std::cin for user input and std::cout for any
  /// output.
  void Execute();
};

#endif
