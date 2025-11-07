#ifndef CHIBCC_CODEGENERATOR_H
#define CHIBCC_CODEGENERATOR_H

#include "AST.h"

namespace chibcc {

//===----------------------------------------------------------------------===//
// Code Generator
//===----------------------------------------------------------------------===//

class CodeGenerator {
private:
  int Depth;

  void push();
  void pop(const char *Arg);
  void genExpr(Node *N);

public:
  CodeGenerator() : Depth(0) {}

  void codegen(Node *N);
};

} // namespace chibcc

#endif // CHIBCC_CODEGENERATOR_H
