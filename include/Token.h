#ifndef CHIBCC_TOKEN_H
#define CHIBCC_TOKEN_H

#include "Common.h"

namespace chibcpp {

//===----------------------------------------------------------------------===//
// Token Types
//===----------------------------------------------------------------------===//

namespace tok {
enum TokenKind : unsigned short {
#define TOK(X) X,
#include "TokenKinds.def"
  NUM_TOKENS
};

/// \brief Determines the name of a token as used within the front end.
///
/// The name of a token will be an internal name (such as "l_square")
/// and should not be used as part of diagnostic messages.
const char *getTokenName(TokenKind Kind);

/// \brief Determines the spelling of simple punctuator tokens like
/// '!' or '%', and returns NULL for literal and annotation tokens.
///
/// This routine only retrieves the "simple" spelling of the token,
/// and will not produce any alternative spellings (e.g., a
/// digraph spelling, an escaped newline, etc.).  For the actual
/// spelling of a given Token, use Preprocessor::getSpelling().
const char *getPunctuatorSpelling(TokenKind Kind);

/// \brief Determines the spelling of simple keyword and contextual keyword
/// tokens like 'int' and 'dynamic_cast'. Returns NULL for other token kinds.
const char *getKeywordSpelling(TokenKind Kind);

/// \brief Return true if this is a raw identifier or an identifier kind.
inline bool isAnyIdentifier(TokenKind K) { return (K == tok::identifier); }

/// \brief Return true if this is a C or C++ string-literal (or
/// C++11 user-defined-string-literal) token.
inline bool isStringLiteral(TokenKind K) { return K == tok::string_literal; }

/// \brief Return true if this is a "literal" kind, like a numeric
/// constant, string, etc.
inline bool isLiteral(TokenKind K) {
  return K == tok::numeric_constant || K == tok::char_constant ||
         isStringLiteral(K);
}

/// \brief Return true if this is any of tok::annot_* kinds.
inline bool isAnnotation(TokenKind) {
  return false; // No annotations in this simple implementation
}
} // namespace tok

class Token {
public:
  tok::TokenKind Kind;
  std::unique_ptr<Token> Next;

  /// The location of the token. This is actually a pointer into the original
  /// source buffer.
  const char *Loc;

  /// The length of the token.
  unsigned Len;

  union {
    /// The actual value of a numeric constant token.
    uint64_t IntegerValue;

    /// A pointer to the start of the literal data for string literals.
    const char *LiteralData;
  };

  Token() : Kind(tok::unknown), Next(nullptr), Loc(nullptr), Len(0) {
    IntegerValue = 0;
  }

  Token(tok::TokenKind K, const char *Location, unsigned Length)
      : Kind(K), Next(nullptr), Loc(Location), Len(Length) {
    IntegerValue = 0;
  }

  /// \brief Return true if this token is a literal value.
  bool isLiteral() const { return tok::isLiteral(Kind); }

  /// \brief Return true if this token is an identifier.
  bool isAnyIdentifier() const { return tok::isAnyIdentifier(Kind); }

  /// \brief Return a source location identifier for the specified
  /// offset in the current file.
  const char *getLocation() const { return Loc; }

  /// \brief Return the length of the token.
  unsigned getLength() const { return Len; }

  /// \brief Return the actual spelling of this token.
  std::string getSpelling() const { return std::string(Loc, Len); }

  /// \brief Given a token representing an identifier, return true if it has a
  /// specific spelling.
  bool is(tok::TokenKind K) const { return Kind == K; }
  bool isNot(tok::TokenKind K) const { return Kind != K; }
  bool isOneOf(tok::TokenKind K1, tok::TokenKind K2) const {
    return is(K1) || is(K2);
  }
  template <typename... Ts>
  bool isOneOf(tok::TokenKind K1, tok::TokenKind K2, Ts... Ks) const {
    return is(K1) || isOneOf(K2, Ks...);
  }

  // Dump token to stderr for debugging (LLVM-style)
  void dump() const;
  void dump(const char *InputStart) const;
};



} // namespace chibcpp

#endif // CHIBCC_TOKEN_H
