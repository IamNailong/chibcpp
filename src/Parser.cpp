#include "Parser.h"

namespace chibcc {

//===----------------------------------------------------------------------===//
// Parser Implementation
//===----------------------------------------------------------------------===//

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

// expr = equality
std::unique_ptr<Node> Parser::expr(Token **Rest, Token *Tok) {
  return equality(Rest, Tok);
}

// equality = relational ("==" relational | "!=" relational)*
std::unique_ptr<Node> Parser::equality(Token **Rest, Token *Tok) {
  auto N = relational(&Tok, Tok);

  for (;;) {
    if (Lexer::equal(Tok, "==")) {
      N = newBinary(NodeKind::Eq, std::move(N),
                    relational(&Tok, Tok->Next.get()));
      continue;
    }

    if (Lexer::equal(Tok, "!=")) {
      N = newBinary(NodeKind::Ne, std::move(N),
                    relational(&Tok, Tok->Next.get()));
      continue;
    }

    *Rest = Tok;
    return N;
  }
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
std::unique_ptr<Node> Parser::relational(Token **Rest, Token *Tok) {
  auto N = add(&Tok, Tok);

  for (;;) {
    if (Lexer::equal(Tok, "<")) {
      N = newBinary(NodeKind::Lt, std::move(N), add(&Tok, Tok->Next.get()));
      continue;
    }

    if (Lexer::equal(Tok, "<=")) {
      N = newBinary(NodeKind::Le, std::move(N), add(&Tok, Tok->Next.get()));
      continue;
    }

    if (Lexer::equal(Tok, ">")) {
      N = newBinary(NodeKind::Lt, add(&Tok, Tok->Next.get()), std::move(N));
      continue;
    }

    if (Lexer::equal(Tok, ">=")) {
      N = newBinary(NodeKind::Le, add(&Tok, Tok->Next.get()), std::move(N));
      continue;
    }

    *Rest = Tok;
    return N;
  }
}

// add = mul ("+" mul | "-" mul)*
std::unique_ptr<Node> Parser::add(Token **Rest, Token *Tok) {
  auto N = mul(&Tok, Tok);

  for (;;) {
    if (Lexer::equal(Tok, "+")) {
      N = newBinary(NodeKind::Add, std::move(N), mul(&Tok, Tok->Next.get()));
      continue;
    }

    if (Lexer::equal(Tok, "-")) {
      N = newBinary(NodeKind::Sub, std::move(N), mul(&Tok, Tok->Next.get()));
      continue;
    }

    *Rest = Tok;
    return N;
  }
}

// mul = unary ("*" unary | "/" unary)*
std::unique_ptr<Node> Parser::mul(Token **Rest, Token *Tok) {
  auto N = unary(&Tok, Tok);

  for (;;) {
    if (Lexer::equal(Tok, "*")) {
      N = newBinary(NodeKind::Mul, std::move(N), unary(&Tok, Tok->Next.get()));
      continue;
    }

    if (Lexer::equal(Tok, "/")) {
      N = newBinary(NodeKind::Div, std::move(N), unary(&Tok, Tok->Next.get()));
      continue;
    }

    *Rest = Tok;
    return N;
  }
}

// unary = ("+" | "-") unary
//       | primary
std::unique_ptr<Node> Parser::unary(Token **Rest, Token *Tok) {
  if (Lexer::equal(Tok, "+"))
    return unary(Rest, Tok->Next.get());

  if (Lexer::equal(Tok, "-"))
    return newUnary(NodeKind::Neg, unary(Rest, Tok->Next.get()));

  return primary(Rest, Tok);
}

// primary = "(" expr ")" | num
std::unique_ptr<Node> Parser::primary(Token **Rest, Token *Tok) {
  if (Lexer::equal(Tok, "(")) {
    auto N = expr(&Tok, Tok->Next.get());
    *Rest = Lexer::skip(Tok, ")");
    return N;
  }

  if (Tok->Kind == tok::numeric_constant) {
    auto N = newNum(Tok->IntegerValue);
    *Rest = Tok->Next.get();
    return N;
  }

  errorTok(Tok, "expected an expression");
  return nullptr; // Never reached
}

std::unique_ptr<Node> Parser::parse(Token *Tok) {
  auto N = expr(&Tok, Tok);
  if (Tok->Kind != tok::eof)
    errorTok(Tok, "extra token");
  return N;
}

} // namespace chibcc
