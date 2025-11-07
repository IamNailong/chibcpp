#include "Token.h"

namespace chibcc {
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
} // namespace chibcc
