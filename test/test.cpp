//===--- test.cpp ---------------------------------------------------------===//
//
// Author: ケジ
// Description: Runs all testing suites.
//
//===----------------------------------------------------------------------===//

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

int main(int argc, char **argv) {
  doctest::Context context;

  // defaults
  context.setOption("success", true);
  context.applyCommandLine(argc, argv);

  return context.run();
}
