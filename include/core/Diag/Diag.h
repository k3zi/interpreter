//===--- Diag.h -----------------------------------------------------------===//
//
// Author: ケジ
// Description: TBD
//
//===----------------------------------------------------------------------===//

#ifndef CORE_DIAG_H
#define CORE_DIAG_H

#include "core/Tokenizer/Token.h"

#include <map>    // std::map
#include <string> // std::string

// Types of diagnostics.
namespace DiagType {
enum DiagType {
  parser_undefined = 0, // undefined diagnostic

  parser_identifier_redecleration,
  parser_undeclared_identifier,
  parser_uninitialized_identifier,
  parser_uninitialized_identifier_flow,

  parser_missing_x_found_y,
  parser_missing_reserved_word,
  parser_missing_reserved_word_x_after_y,
  parser_missing_reserved_word_x_after_y_in_z,
  parser_missing_reserved_word_x_at_start_of_y,
  parser_missing_x_token_at_start_of_y,
  parser_missing_x_token_after_y_in_z,
  parser_missing_x_token_at_end_of_y,
  parser_missing_x_token_at_end_of_y_in_z,

  parser_unexpected_factor_type_x,
  parser_unexpected_comparison_type_x,
  parser_unexpected_conditional_type_x,

  parser_expected_eof,

  runtime_arithmitic_x_causes_y
};
} // namespace DiagType

using namespace std;

/// A custom runtime error type that formats bassed on arguments passed into the
/// constructor.
class Diag : public std::runtime_error {
  string WhatMessage;

public:
  /// Returns a diagnostic error based on the type and formatted with the passed
  /// in arguments.
  template <typename... Args>
  Diag(DiagType::DiagType D, Args &&... args)
      : WhatMessage(Diag::ToString(D, std::forward<Args>(args)...)),
        std::runtime_error("") {}

  /// Format the actual error to a string.
  template <typename... Args>
  static string ToString(DiagType::DiagType D, Args &&... args) {
    static map<DiagType::DiagType, string> mapping = {
        {DiagType::parser_undefined, "Undefined error occured."},
        {DiagType::parser_missing_x_found_y, "Expected %s. Found '%s'."},
        {DiagType::parser_missing_reserved_word,
         "Expected reserved word: '%s'."},
        {DiagType::parser_identifier_redecleration,
         "Redeclaration of identifier: '%s'."},
        {DiagType::parser_undeclared_identifier,
         "Missing decleration for identifier: '%s'."},
        {DiagType::parser_uninitialized_identifier,
         "Identifier used before initialization: '%s'."},
        {DiagType::parser_uninitialized_identifier_flow,
         "Not all paths of the program initialize '%s' before it is used here. "
         "This may be a false-positive but can be indicative of a design flaw "
         "in your program."},
        {DiagType::parser_missing_reserved_word_x_after_y,
         "Expected reserved word: '%s' after '%s'."},
        {DiagType::parser_missing_reserved_word_x_after_y_in_z,
         "Expected reserved word: '%s' after '%s' in %s."},
        {DiagType::parser_missing_reserved_word_x_at_start_of_y,
         "Expected reserved word: '%s' at start of %s."},
        {DiagType::parser_missing_x_token_at_start_of_y,
         "Expected '%s' token at start of %s."},
        {DiagType::parser_missing_x_token_after_y_in_z,
         "Expected '%s' token after '%s' in %s."},
        {DiagType::parser_missing_x_token_at_end_of_y_in_z,
         "Expected '%s' token at end of '%s' in %s."},
        {DiagType::parser_missing_x_token_at_end_of_y,
         "Expected '%s' token at end of '%s'."},
        {DiagType::parser_expected_eof,
         "Token found after end of program: '%s'. Expected to "
         "reach end-of-file after parsing a program."},
        {DiagType::runtime_arithmitic_x_causes_y,
         "Performing %s here will cause %s and "
         "unexpected behavior."},
        {DiagType::parser_unexpected_factor_type_x,
         "Unexpected factor type: %s. Expected one of [integer, constant, "
         "identifier, expression]."},
        {DiagType::parser_unexpected_comparison_type_x,
         "Unexpected comparison type: %s. Expected one of ['!=', '==', '<', "
         "'>', '<=', '>=']."},
        {DiagType::parser_unexpected_conditional_type_x,
         R"(Unexpected conditional type: %s. Expected one of ["and", "or"].)"}};

    string result = mapping[D];
    // Dismiss this warning that occurs when no arguments are passed into
    // `snprintf` even when they are.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-security"
    if (sizeof...(args) == 0) {
      return result;
    }

    auto size = snprintf(nullptr, 0, result.c_str(), forward<Args>(args)...);
    string output(size + 1, '\0');
    sprintf(&output[0], result.c_str(), forward<Args>(args)...);
    return output;
#pragma clang diagnostic pop
  }

  const char *what() const noexcept override { return WhatMessage.c_str(); }
};

// A diagnostic that contains location information.
class LocDiag : public Diag {

  /// The location where this diagnostic applies.
  Token Tok;

public:
  template <typename... Args>
  LocDiag(Token T, DiagType::DiagType D, Args &&... args)
      : Diag(D, std::forward<Args>(args)...), Tok(T) {}

  // Define getters.
  Token getToken() const { return Tok; }
};

#endif
