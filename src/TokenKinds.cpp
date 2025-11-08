#include "Token.h"
#include <iostream>

namespace chibcpp {
namespace tok {

static const char *const TokNames[] = {
#define TOK(X) #X,
#define KEYWORD(X, Y) #X,
#include "TokenKinds.def"
    nullptr};

const char *getTokenName(TokenKind Kind) {
  if (Kind < NUM_TOKENS)
    return TokNames[Kind];
  return nullptr;
}

const char *getPunctuatorSpelling(TokenKind Kind) {
  switch (Kind) {
#define PUNCTUATOR(X, Y)                                                       \
  case X:                                                                      \
    return Y;
#include "TokenKinds.def"
  default:
    break;
  }
  return nullptr;
}

const char *getKeywordSpelling(TokenKind Kind) {
  switch (Kind) {
#define KEYWORD(X, Y)                                                          \
  case kw_##X:                                                                 \
    return #X;
#include "TokenKinds.def"
  default:
    break;
  }
  return nullptr;
}

} // namespace tok

void Token::dump() const {
  dump(nullptr);
}

void Token::dump(const char *InputStart) const {
  std::cerr << "Token: " << tok::getTokenName(Kind);

  if (Loc && Len > 0) {
    std::cerr << " '" << std::string(Loc, Len) << "'";
  }

  if (Kind == tok::numeric_constant) {
    std::cerr << " (value: " << IntegerValue << ")";
  }

  // Print offset from input start if available
  if (Loc) {
    if (InputStart) {
      std::cerr << " at offset " << (Loc - InputStart);
    } else {
      std::cerr << " at " << static_cast<const void*>(Loc);
    }
  } else {
    std::cerr << " at (null)";
  }

  std::cerr << "\n";
}

} // namespace chibcpp
