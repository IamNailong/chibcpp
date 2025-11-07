#ifndef CHIBCC_PARSER_H
#define CHIBCC_PARSER_H

#include "AST.h"
#include "Tokenizer.h"

namespace chibcc {

//===----------------------------------------------------------------------===//
// Parser - Recursive descent parser with integrated lexer
//===----------------------------------------------------------------------===//

class Parser {
private:
  Lexer &Lex;
  std::unique_ptr<Token> CurTok; // Current token

  // Helper methods for AST node creation
  std::unique_ptr<Node> newNode(NodeKind Kind);
  std::unique_ptr<Node> newBinary(NodeKind Kind, std::unique_ptr<Node> Lhs,
                                  std::unique_ptr<Node> Rhs);
  std::unique_ptr<Node> newUnary(NodeKind Kind, std::unique_ptr<Node> Expr);
  std::unique_ptr<Node> newNum(int Val);

  // Token management
  void nextToken(); // Advance to next token
  bool match(const char *Op); // Check and consume if matches
  bool match(tok::TokenKind Kind); // Check and consume if matches
  void expect(const char *Op); // Consume or error
  bool check(const char *Op); // Check without consuming
  bool check(tok::TokenKind Kind); // Check without consuming

  // Grammar rules
  std::unique_ptr<Node> expr();
  std::unique_ptr<Node> equality();
  std::unique_ptr<Node> relational();
  std::unique_ptr<Node> add();
  std::unique_ptr<Node> mul();
  std::unique_ptr<Node> unary();
  std::unique_ptr<Node> primary();

public:
  explicit Parser(Lexer &L) : Lex(L) {}

  std::unique_ptr<Node> parse();
};

} // namespace chibcc

#endif // CHIBCC_PARSER_H
