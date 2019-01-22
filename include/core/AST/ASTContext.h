//===--- ASTContext.h -----------------------------------------------------===//
//
// Author: ケジ
// Description: The context element that supports the CORE abstract syntaxt tree
// and maintains the symbol table during run-time; as well as handles I/O.
//
//===----------------------------------------------------------------------===//

#ifndef CORE_AST_CONTEXT_H
#define CORE_AST_CONTEXT_H

#include <map>    // std::map
#include <string> // std::string

class IdList;
class Id;

// TODO: Possibly move creation of identifier symbols to the Tokenizer and keep
// a `Declared` property on the symbol.
struct IdSym {
  /// \brief The current value of the variable. We only support integers.
  int Value;

  /// \brief Whether the value has been initialized or not. This symbol's
  /// existence means that the identifier is declared.
  bool Initialized = false;
};

class ASTContext {
  /// The raw symbol table which is just a mapping of identifiers to integers.
  std::map<std::string, IdSym *> SM;

  /// Feteches the symbol for the given `Id`.
  /// \param I an `Id` expected to be in the symbol table.
  /// \throw if the `Id` is not found.
  IdSym *FetchId(Id *I);

public:
  /// \brief Constructs an abstract syntax tree context with an initialized
  /// empty symbol table.
  ASTContext() {}

  /// \brief Declares the Id list in to the symbol tabel
  ///
  /// \param L an `IdList` node that does not have any declared `Id`s.
  /// \throw std::string if an Id has already been declared.
  void Declare(IdList *L);

  /// \brief A convinence method to call `Initialize` on each `Id` in an
  /// `IdList`.
  ///
  /// \see ASTContext::Initialize(Id *I)
  void Initialize(IdList *L);

  /// A method called whenever an identifier is having it's value set; whenever
  /// the l-value appears on the left hand side of an equals sign or is read
  /// into.
  void Initialize(Id *I);

  /// \brief A convinence method to call `Reference` on each `Id` in an
  /// `IdList`.
  ///
  /// \see ASTContext::Reference(Id *I)
  void Reference(IdList *L);

  /// \brief A method called for every reference (r-value) to an `Id` in a CORE
  /// statement.
  ///
  /// This method will ensure the Id is in the symbol table and has
  /// been initialized. This should not be called when an `Id` is being
  /// declared.
  ///
  /// \param I an Id that has been referenced in the `Parser`.
  /// \throw std::string if the `Id` is not in the symbol table.
  void Reference(Id *I);

  /// Checks and returns whether the given `Id` exists in the symbol tabel.
  bool Has(Id *I);

  /// \brief Set an existing `Id` to a given value.
  /// \throw std::sting if the `Id` doesn't exist
  void Set(Id *I, int Value);

  /// \brief Get the value of an existing `Id`.
  /// \throw std::sting if the `Id` doesn't exist
  /// \throw std::sting if the `Id` hasn't been initialized
  /// \return the value in the symbol table of the `Id`.
  int Get(Id *I);

  /// \brief Sets the value of each `Id` in the `IdList` to a coresponding value
  /// that the user inputs.
  /// \throw std::sting if `an` Id doesn't exist
  void SetFromIn(IdList *L);

  /// \brief Writes each `Id` of the `IdList` to `std::cout`.
  /// \throw std::sting if an `Id` doesn't exist
  /// \throw std::sting if an `Id` hasn't been initialized
  void WriteToOut(IdList *L);
};

#endif
