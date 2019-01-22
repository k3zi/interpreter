Author: ケジ

Files:
  - include/core/AST/*: Defines members and classes for the abstract syntax
    tree. This includes interfaces for printing and interpreting as well.
  - include/core/Diag/*: Defines & implements diagnostic errors.
  - include/core/Parser/*: Defines the Parser class.
  - include/core/Tokenizer/*: Defines the Tokenizer class and other structures.
  - lib/core/*: Implementations for the above headers.

Compilation (Read all steps carefully before starting):
  1. Run `CC=/usr/bin/clang CXX=/usr/bin/clang++ cmake ./` in the root
    directory of the project to export a Makefile from the `CMakeLists.txt`
    file.
  2. Run `make` to build the `Tokenizer` / `Parser` / `Interpreter` / `Tester`
    executables.

Execution:
  1. Use the executables as such:
    - `Tokenizer testFile.core`
    - `Parser testFile.core`
    - `Interpreter testFile.core`
    - `Tester`
    , passing in a CORE language source file.
  2. Note on some operating systems you may have to prefix the program name
    with the current working directory `./` -> `./Tokenizer testFile.core`.
