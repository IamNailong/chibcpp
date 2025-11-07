#ifndef CHIBCC_PARSER_H
#define CHIBCC_PARSER_H

#include "AST.h"
#include "Tokenizer.h"

namespace chibcc {

//===----------------------------------------------------------------------===//
// Parser
//===----------------------------------------------------------------------===//

class Parser {
private:
  std::unique_ptr<Node> newNode(NodeKind Kind);
  std::unique_ptr<Node> newBinary(NodeKind Kind, std::unique_ptr<Node> Lhs,
                                  std::unique_ptr<Node> Rhs);
  std::unique_ptr<Node> newUnary(NodeKind Kind, std::unique_ptr<Node> Expr);
  std::unique_ptr<Node> newNum(int Val);

  std::unique_ptr<Node> expr(Token **Rest, Token *Tok);
  std::unique_ptr<Node> equality(Token **Rest, Token *Tok);
  std::unique_ptr<Node> relational(Token **Rest, Token *Tok);
  std::unique_ptr<Node> add(Token **Rest, Token *Tok);
  std::unique_ptr<Node> mul(Token **Rest, Token *Tok);
  std::unique_ptr<Node> unary(Token **Rest, Token *Tok);
  std::unique_ptr<Node> primary(Token **Rest, Token *Tok);

  Lexer &Lex;

public:
  explicit Parser(Lexer &L) : Lex(L) {}

  std::unique_ptr<Node> parse(Token *Tok);
};

} // namespace chibcc

#endif // CHIBCC_PARSER_H
