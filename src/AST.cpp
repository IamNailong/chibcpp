#include "AST.h"
#include <iostream>

namespace chibcpp {

const char *Node::getKindName() const {
  switch (Kind) {
  case NodeKind::Add:
    return "Add";
  case NodeKind::Sub:
    return "Sub";
  case NodeKind::Mul:
    return "Mul";
  case NodeKind::Div:
    return "Div";
  case NodeKind::Neg:
    return "Neg";
  case NodeKind::Eq:
    return "Eq";
  case NodeKind::Ne:
    return "Ne";
  case NodeKind::Lt:
    return "Lt";
  case NodeKind::Le:
    return "Le";
  case NodeKind::Num:
    return "Num";
  }
  return "Unknown";
}

void Node::dump() const { dump(0); }

void Node::dump(int Indent) const {
  // Print indentation
  for (int I = 0; I < Indent; ++I)
    std::cerr << "  ";

  // Print node kind
  std::cerr << getKindName();

  // Print value for numeric literals
  if (Kind == NodeKind::Num) {
    std::cerr << " " << Val;
  }

  std::cerr << "\n";

  // Recursively dump children
  if (Lhs) {
    Lhs->dump(Indent + 1);
  }
  if (Rhs) {
    Rhs->dump(Indent + 1);
  }
}

} // namespace chibcpp
