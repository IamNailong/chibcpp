#ifndef CHIBCC_TOKENIZER_H
#define CHIBCC_TOKENIZER_H

#include "Diagnostic.h"
#include "Token.h"

namespace chibcpp {

//===----------------------------------------------------------------------===//
// Lexer - This provides a simple interface that turns a text buffer into a
// stream of tokens.  This provides no support for file reading or buffering,
// or buffering/seeking of tokens, only forward lexing is supported.  It relies
// on the specified Preprocessor object to handle preprocessor directives, etc.
//===----------------------------------------------------------------------===//

class Lexer {
private:
  const char *BufferStart; // Start of the buffer.
  const char *BufferPtr;   // Current pointer into the buffer.
  const char *BufferEnd;   // End of the buffer.
  DiagnosticEngine &Diags; // Diagnostic engine for error reporting.

  // Lookahead cache
  std::vector<std::unique_ptr<Token>> LookaheadCache;

  /// \brief Create a new token with the specified information.
  std::unique_ptr<Token> formToken(tok::TokenKind Kind, const char *TokStart);

  /// \brief Skip whitespace and comments, return the first non-whitespace
  /// character after skipping whitespace and comments.
  bool skipWhitespace();

  /// \brief We have just read the // characters, skip until we find the
  /// newline character that terminates the comment.  Then update BufferPtr.
  bool skipLineComment();

  /// \brief We have just read the /* characters, skip until we find the */
  /// characters that terminate the comment.  Then update BufferPtr.
  bool skipBlockComment();

  /// \brief Lex a number: integer-constant, floating-constant.
  void lexNumericConstant(Token &Result);

  /// \brief Lex a string literal or character constant.
  void lexStringLiteral(Token &Result, const char *CurPtr);

  /// \brief Lex an identifier or keyword.
  void lexIdentifier(Token &Result, const char *CurPtr);

  /// \brief Return true if the specified string is the body of an identifier.
  static bool isIdentifierBody(unsigned char c) {
    return isalnum(c) || c == '_';
  }

  /// \brief Return true if the specified string is the start of an identifier.
  static bool isIdentifierHead(unsigned char c) {
    return isalpha(c) || c == '_';
  }

  /// \brief Matches punctuation tokens.
  tok::TokenKind tryMatchPunctuator(const char *CurPtr, unsigned &Size);

public:
  /// \brief Construct a Lexer for the given buffer.
  Lexer(const char *InputStart, const char *InputEnd, DiagnosticEngine &Diags);

  /// \brief Lex the next token and return it.
  std::unique_ptr<Token> lex();

  /// \brief Peek at the next token without consuming it (lookahead by 1).
  /// Returns nullptr if at end of input.
  Token *peek();

  /// \brief Peek ahead N tokens without consuming them.
  /// N=1 is equivalent to peek(). Returns nullptr if not enough tokens.
  Token *peek(unsigned N);

  /// \brief Save current lexer position for potential backtracking.
  /// Returns a position marker that can be used with reset().
  const char *savePosition() const { return BufferPtr; }

  /// \brief Reset lexer to a previously saved position.
  /// Clears the lookahead cache.
  void resetPosition(const char *Pos) {
    BufferPtr = Pos;
    LookaheadCache.clear();
  }

  /// \brief Return true if the specified token kind is a literal (like a
  /// numeric constant, string, etc).
  static bool isLiteral(tok::TokenKind K) { return tok::isLiteral(K); }

  /// \brief Utility functions for token matching
  static bool equal(Token *Tok, const char *Op);
  static bool equal(Token *Tok, tok::TokenKind Kind);

  /// \brief Dump all tokens to stderr for debugging
  void dumpTokens();
};

} // namespace chibcpp

#endif // CHIBCC_TOKENIZER_H
