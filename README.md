# chibcpp - A C Compiler Implementation in C++

A C++ implementation inspired by [chibicc](https://github.com/rui314/chibcc).

## Project Overview

This is an educational C compiler project that demonstrates compiler construction principles using modern C++. The compiler translates C expressions into x86-64 assembly code.

## Build & Usage

```bash
# Build
mkdir -p build && cd build
cmake ..
make

# Run
./bin/chibcpp "1+2*3"

# Test
./test_compiler.sh
```

## Development Log

This section documents the incremental development process, explaining what each commit accomplishes.

#### [`651a633`](https://github.com/IamNailong/chibcpp/commit/651a633) - Support basic numeric expressions
#### [`25b4797`](https://github.com/IamNailong/chibcpp/commit/25b4797) - Refactor parser to integrate lexer
#### [`5ba64ca`](https://github.com/IamNailong/chibcpp/commit/72b335c030f3454ae39707a34a7a260f85ba64ca) - Add LLVM-style debugging and CLI parser
