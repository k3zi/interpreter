//===--- Parser.h ---------------------------------------------------------===//
//
// Author: ケジ
// Description: An interface displaying the public and private members of a CORE
// Parser.
//
//===----------------------------------------------------------------------===//

#ifndef CORE_PARSER_H
#define CORE_PARSER_H

#include "core/Diag/Diag.h"
#include "core/Tokenizer/Tokenizer.h"

class AST;

/// A class to perform the parsing a CORE language translation unit into an
/// abstract syntax tree.
class Parser {
  /// The tokenizer attached to this parser.
  Tokenizer *T;

  /// A reference to the abstract syntax tree that we will build on.
  AST &AST;

  /// Construct a parser from the given tokenizer and abstract syntax tree.
  Parser(Tokenizer *t, class AST &A);

public:
  /// Retrieves the context (symbol table) for the abstract syntax tree.
  /// By providing this methods
  ASTContext &getContext() const;

  /// Returns the last tokenized Token object.
  Token currentToken() const;

  /// Initializes and returns the pointer to a parser tied to a tokenizer
  /// constructed with the specified string representation of a CORE translation
  /// unit.
  /// \param String a string representing a CORE translation unit.
  /// \param A an empty AST.
  /// \throw any error that a Tokenizer might throw on construction.
  /// \return a pointer to a newly allocated Parser object.
  static Parser *CreateFromString(std::string String, class AST &A);

  /// Initializes and returns the pointer to a parser tied to a tokenizer
  /// constructed with the specified file representation of a CORE translation
  /// unit.
  /// \param FilePath a file path to a file representing a CORE translation
  /// unit.
  /// \param A an empty AST.
  /// \throw any error that a Tokenizer might throw
  /// on construction.
  /// \return a pointer to a newly allocated Parser object.
  static Parser *CreateFromFile(std::string FilePath, class AST &A);

  /// Retrieves the next token essentialy skipping past the current token or
  /// "consuming" it.
  void ConsumeToken();

  /// Convinence method that calls `ConsumeToken` if the current token is of the
  /// specified type.
  bool ConsumeIf(TokenType::TokenType Type);

  /// Convinence method that calls `ConsumeToken` if the current token is of the
  /// specified type. If it is not it throws the passed in diagnostic type.
  template <typename... Args>
  void ConsumeIf(TokenType::TokenType Type, DiagType::DiagType Error,
                 Args &&... args) {
    if (ConsumeIf(Type)) return;
    throw Diag(Error, std::forward<Args>(args)...);
  }

  /// Returns whether the current token is the specified type.
  bool isToken(TokenType::TokenType Type) { return currentToken().is(Type); }

  /// Decorates errors that occur from actual parsing in `UndecoratedParse`
  /// with line / column numbers and token information.
  void Parse();

  /// Attempts to parse the CORE translation unit starting from it's first
  /// nonterminal.
  ///
  /// \throw std::string, Diag, LocDiag: will throw any errors received during
  /// parsing.
  void UndecoratedParse();
};

#endif
