#include "CodeGenerator.h"
#include <cstring>

namespace chibcpp {

//===----------------------------------------------------------------------===//
// Code Generator Implementation
//===----------------------------------------------------------------------===//

bool CodeGenerator::setOutputFile(const char *Filename) {
  if (!Filename || strcmp(Filename, "-") == 0) {
    Output = stdout;
    ShouldCloseFile = false;
    return true;
  }

  Output = fopen(Filename, "w");
  if (!Output) {
    fprintf(stderr, "Error: Cannot open output file '%s'\n", Filename);
    return false;
  }

  ShouldCloseFile = true;
  return true;
}

void CodeGenerator::push() {
  fprintf(Output, "  push %%rax\n");
  Depth++;
}

void CodeGenerator::pop(const char *Arg) {
  fprintf(Output, "  pop %s\n", Arg);
  Depth--;
}

void CodeGenerator::genExpr(Node *N) {
  switch (N->Kind) {
  case NodeKind::Num:
    fprintf(Output, "  mov $%d, %%rax\n", N->Val);
    return;
  case NodeKind::Neg:
    genExpr(N->Lhs.get());
    fprintf(Output, "  neg %%rax\n");
    return;
  default:
    break;
  }

  genExpr(N->Rhs.get());
  push();
  genExpr(N->Lhs.get());
  pop("%rdi");

  switch (N->Kind) {
  case NodeKind::Add:
    fprintf(Output, "  add %%rdi, %%rax\n");
    return;
  case NodeKind::Sub:
    fprintf(Output, "  sub %%rdi, %%rax\n");
    return;
  case NodeKind::Mul:
    fprintf(Output, "  imul %%rdi, %%rax\n");
    return;
  case NodeKind::Div:
    fprintf(Output, "  cqo\n");
    fprintf(Output, "  idiv %%rdi\n");
    return;
  case NodeKind::Eq:
  case NodeKind::Ne:
  case NodeKind::Lt:
  case NodeKind::Le:
    fprintf(Output, "  cmp %%rdi, %%rax\n");

    if (N->Kind == NodeKind::Eq)
      fprintf(Output, "  sete %%al\n");
    else if (N->Kind == NodeKind::Ne)
      fprintf(Output, "  setne %%al\n");
    else if (N->Kind == NodeKind::Lt)
      fprintf(Output, "  setl %%al\n");
    else if (N->Kind == NodeKind::Le)
      fprintf(Output, "  setle %%al\n");

    fprintf(Output, "  movzb %%al, %%rax\n");
    return;
  default:
    break;
  }

  error("invalid expression");
}

void CodeGenerator::codegen(Node *N) {
  fprintf(Output, ".globl main\n");
  fprintf(Output, "main:\n");

  genExpr(N);
  fprintf(Output, "  ret\n");

  // Add GNU stack note to prevent executable stack warning
  fprintf(Output, ".section .note.GNU-stack,\"\",%%progbits\n");

  assert(Depth == 0);
}

} // namespace chibcpp
