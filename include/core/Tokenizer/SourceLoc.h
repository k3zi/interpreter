//===--- SourceLoc.h ------------------------------------------------------===//
//
// Author: ケジ
// Description: A simple struct for location in a source file.
//
//===----------------------------------------------------------------------===//

#ifndef CORE_TOKENIZER_LOC_H
#define CORE_TOKENIZER_LOC_H

/// Defines a location in the source file
struct SourceLoc {
  /// The line number.
  unsigned LineNumber;

  /// The column number.
  unsigned ColumnNumber;
};

#endif
