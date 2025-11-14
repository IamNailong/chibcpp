#ifndef CHIBCC_PARSER_H
#define CHIBCC_PARSER_H

#include "AST.h"
#include "Diagnostic.h"
#include "Tokenizer.h"

namespace chibcpp {

//===----------------------------------------------------------------------===//
// Parser - Recursive descent parser with integrated lexer
//===----------------------------------------------------------------------===//

class Parser {
private:
  Lexer &Lex;
  DiagnosticEngine &Diags;
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

  // Lookahead and backtracking
  Token *peekToken(unsigned N = 1); // Peek ahead N tokens

  // Parser state for backtracking
  struct ParserState {
    const char *LexerPos;
    const Token *CurrentToken;
  };

  /// \brief Save current parser state for backtracking
  ParserState saveState() const {
    return ParserState{Lex.savePosition(), CurTok.get()};
  }

  /// \brief Restore parser to a previously saved state
  void restoreState(const ParserState &State) {
    Lex.resetPosition(State.LexerPos);
    // Re-lex the current token
    CurTok = Lex.lex();
  }

  // Grammar rules
  std::unique_ptr<Node> expr();
  std::unique_ptr<Node> stmt();
  std::unique_ptr<Node> expr_stmt();
  std::unique_ptr<Node> equality();
  std::unique_ptr<Node> relational();
  std::unique_ptr<Node> add();
  std::unique_ptr<Node> mul();
  std::unique_ptr<Node> unary();
  std::unique_ptr<Node> primary();

public:
  explicit Parser(Lexer &L, DiagnosticEngine &D) : Lex(L), Diags(D) {}

  std::unique_ptr<Node> parse();
};

} // namespace chibcpp

#endif // CHIBCC_PARSER_H
