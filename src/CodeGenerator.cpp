#include "CodeGenerator.h"

namespace chibcc {

//===----------------------------------------------------------------------===//
// Code Generator Implementation
//===----------------------------------------------------------------------===//

void CodeGenerator::push() {
  printf("  push %%rax\n");
  Depth++;
}

void CodeGenerator::pop(const char *Arg) {
  printf("  pop %s\n", Arg);
  Depth--;
}

void CodeGenerator::genExpr(Node *N) {
  switch (N->Kind) {
  case NodeKind::Num:
    printf("  mov $%d, %%rax\n", N->Val);
    return;
  case NodeKind::Neg:
    genExpr(N->Lhs.get());
    printf("  neg %%rax\n");
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
    printf("  add %%rdi, %%rax\n");
    return;
  case NodeKind::Sub:
    printf("  sub %%rdi, %%rax\n");
    return;
  case NodeKind::Mul:
    printf("  imul %%rdi, %%rax\n");
    return;
  case NodeKind::Div:
    printf("  cqo\n");
    printf("  idiv %%rdi\n");
    return;
  case NodeKind::Eq:
  case NodeKind::Ne:
  case NodeKind::Lt:
  case NodeKind::Le:
    printf("  cmp %%rdi, %%rax\n");

    if (N->Kind == NodeKind::Eq)
      printf("  sete %%al\n");
    else if (N->Kind == NodeKind::Ne)
      printf("  setne %%al\n");
    else if (N->Kind == NodeKind::Lt)
      printf("  setl %%al\n");
    else if (N->Kind == NodeKind::Le)
      printf("  setle %%al\n");

    printf("  movzb %%al, %%rax\n");
    return;
  default:
    break;
  }

  error("invalid expression");
}

void CodeGenerator::codegen(Node *N) {
  printf("  .globl main\n");
  printf("main:\n");

  genExpr(N);
  printf("  ret\n");

  assert(Depth == 0);
}

} // namespace chibcc