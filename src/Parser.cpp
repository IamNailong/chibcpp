#include "Parser.h"

namespace chibcpp {

//===----------------------------------------------------------------------===//
// Parser Implementation
//===----------------------------------------------------------------------===//

// Token management methods

void Parser::nextToken() {
  CurTok = Lex.lex();
}

bool Parser::match(const char *Op) {
  if (check(Op)) {
    nextToken();
    return true;
  }
  return false;
}

bool Parser::match(tok::TokenKind Kind) {
  if (check(Kind)) {
    nextToken();
    return true;
  }
  return false;
}

void Parser::expect(const char *Op) {
  if (!match(Op)) {
    errorTok(CurTok.get(), "expected '%s'", Op);
  }
}

bool Parser::check(const char *Op) {
  return CurTok && Lexer::equal(CurTok.get(), Op);
}

bool Parser::check(tok::TokenKind Kind) {
  return CurTok && CurTok->Kind == Kind;
}

// AST node creation helpers

std::unique_ptr<Node> Parser::newNode(NodeKind Kind) {
  return std::make_unique<Node>(Kind);
}

std::unique_ptr<Node> Parser::newBinary(NodeKind Kind,
                                        std::unique_ptr<Node> Lhs,
                                        std::unique_ptr<Node> Rhs) {
  auto N = newNode(Kind);
  N->Lhs = std::move(Lhs);
  N->Rhs = std::move(Rhs);
  return N;
}

std::unique_ptr<Node> Parser::newUnary(NodeKind Kind,
                                       std::unique_ptr<Node> Expr) {
  auto N = newNode(Kind);
  N->Lhs = std::move(Expr);
  return N;
}

std::unique_ptr<Node> Parser::newNum(int Val) {
  auto N = newNode(NodeKind::Num);
  N->Val = Val;
  return N;
}

// Grammar rules

// expr = equality
std::unique_ptr<Node> Parser::expr() {
  return equality();
}

// equality = relational ("==" relational | "!=" relational)*
std::unique_ptr<Node> Parser::equality() {
  auto N = relational();

  for (;;) {
    if (match("==")) {
      N = newBinary(NodeKind::Eq, std::move(N), relational());
      continue;
    }

    if (match("!=")) {
      N = newBinary(NodeKind::Ne, std::move(N), relational());
      continue;
    }

    return N;
  }
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
std::unique_ptr<Node> Parser::relational() {
  auto N = add();

  for (;;) {
    if (match("<")) {
      N = newBinary(NodeKind::Lt, std::move(N), add());
      continue;
    }

    if (match("<=")) {
      N = newBinary(NodeKind::Le, std::move(N), add());
      continue;
    }

    if (match(">")) {
      N = newBinary(NodeKind::Lt, add(), std::move(N));
      continue;
    }

    if (match(">=")) {
      N = newBinary(NodeKind::Le, add(), std::move(N));
      continue;
    }

    return N;
  }
}

// add = mul ("+" mul | "-" mul)*
std::unique_ptr<Node> Parser::add() {
  auto N = mul();

  for (;;) {
    if (match("+")) {
      N = newBinary(NodeKind::Add, std::move(N), mul());
      continue;
    }

    if (match("-")) {
      N = newBinary(NodeKind::Sub, std::move(N), mul());
      continue;
    }

    return N;
  }
}

// mul = unary ("*" unary | "/" unary)*
std::unique_ptr<Node> Parser::mul() {
  auto N = unary();

  for (;;) {
    if (match("*")) {
      N = newBinary(NodeKind::Mul, std::move(N), unary());
      continue;
    }

    if (match("/")) {
      N = newBinary(NodeKind::Div, std::move(N), unary());
      continue;
    }

    return N;
  }
}

// unary = ("+" | "-") unary
//       | primary
std::unique_ptr<Node> Parser::unary() {
  if (match("+"))
    return unary();

  if (match("-"))
    return newUnary(NodeKind::Neg, unary());

  return primary();
}

// primary = "(" expr ")" | num
std::unique_ptr<Node> Parser::primary() {
  if (match("(")) {
    auto N = expr();
    expect(")");
    return N;
  }

  if (check(tok::numeric_constant)) {
    auto N = newNum(CurTok->IntegerValue);
    nextToken();
    return N;
  }

  errorTok(CurTok.get(), "expected an expression");
  return nullptr; // Never reached
}

std::unique_ptr<Node> Parser::parse() {
  // Initialize by reading first token
  nextToken();

  auto N = expr();

  if (!check(tok::eof))
    errorTok(CurTok.get(), "extra token");

  return N;
}

} // namespace chibcpp
