Author: ケジ

Files:
  - include/Token.h: A class for holding token information. Token types are
    stored as enums.
  - include/Tokenizer.h: Defines a class that takes input from a file, reads
    each character and forms valid tokens, breaking when an invalid token is
    registered. This operation is greedy, so `===` is split into to:
    `==` + `=`.
  - lib/Tokenizer.cpp: Implementation file for the Tokenizer class described
    above.
  - CMakeLists.txt: Sets up compiler flags, header search paths, and source
    files. Defines the executable.
  - TokenizerMain.cpp: The main Tokenizer program that sets up a Tokenizer
    object and using the first argument (file path to a CORE language source
    file) passed in to the program, outputs the corresponding token type
    numbers for the source file.

Compilation:
  1. Run `cmake ./` in the root directory of the project to export a Makefile
    from the `CMakeLists.txt` file.
  2. Run `make` to build the `Tokenizer` executable.

Execution:
  3. Use the executable as such: `./Tokenizer testFile.core`, passing in a CORE
    language source file.
