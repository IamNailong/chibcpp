#ifndef CHIBCC_AST_H
#define CHIBCC_AST_H

#include "Common.h"

namespace chibcpp {

//===----------------------------------------------------------------------===//
// AST Node Types
//===----------------------------------------------------------------------===//

enum class NodeKind {
  Add, // +
  Sub, // -
  Mul, // *
  Div, // /
  Neg, // unary -
  Eq,  // ==
  Ne,  // !=
  Lt,  // <
  Le,  // <=
  Num, // Integer
};

class Node {
public:
  NodeKind Kind;
  std::unique_ptr<Node> Lhs;
  std::unique_ptr<Node> Rhs;
  int Val;

  explicit Node(NodeKind K) : Kind(K), Lhs(nullptr), Rhs(nullptr), Val(0) {}

  // Dump AST to stderr for debugging
  void dump() const;
  void dump(int Indent) const;

private:
  // Get string representation of node kind
  const char *getKindName() const;
};

} // namespace chibcpp

#endif // CHIBCC_AST_H
