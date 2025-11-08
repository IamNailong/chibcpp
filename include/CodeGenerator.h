#ifndef CHIBCC_CODEGENERATOR_H
#define CHIBCC_CODEGENERATOR_H

#include "AST.h"
#include "Diagnostic.h"
#include <cstdio>
#include <string>

namespace chibcpp {

//===----------------------------------------------------------------------===//
// Code Generator
//===----------------------------------------------------------------------===//

class CodeGenerator {
private:
  int Depth;
  FILE *Output;
  bool ShouldCloseFile;
  DiagnosticEngine &Diags;

  void push();
  void pop(const char *Arg);
  void genExpr(Node *N);

public:
  CodeGenerator(DiagnosticEngine &D)
      : Depth(0), Output(stdout), ShouldCloseFile(false), Diags(D) {}
  ~CodeGenerator() {
    if (ShouldCloseFile && Output) {
      fclose(Output);
    }
  }

  // Set output file (nullptr or "-" for stdout)
  bool setOutputFile(const char *Filename);

  void codegen(Node *N);
};

} // namespace chibcpp

#endif // CHIBCC_CODEGENERATOR_H
