#include "Tokenizer.h"
#include <iostream>

namespace chibcc {

//===----------------------------------------------------------------------===//
// Error Handling Implementation
//===----------------------------------------------------------------------===//

static const char *CurrentInput = nullptr;

void error(const char *Fmt, ...) {
  va_list Ap;
  va_start(Ap, Fmt);
  vfprintf(stderr, Fmt, Ap);
  fprintf(stderr, "\n");
  exit(1);
}

static void verrorAt(const char *Loc, const char *Fmt, va_list Ap) {
  int Pos = Loc - CurrentInput;
  fprintf(stderr, "%s\n", CurrentInput);
  fprintf(stderr, "%*s", Pos, ""); // print Pos spaces.
  fprintf(stderr, "^ ");
  vfprintf(stderr, Fmt, Ap);
  fprintf(stderr, "\n");
  exit(1);
}

void errorAt(const char *Loc, const char *Fmt, ...) {
  va_list Ap;
  va_start(Ap, Fmt);
  verrorAt(Loc, Fmt, Ap);
}

void errorTok(Token *Tok, const char *Fmt, ...) {
  va_list Ap;
  va_start(Ap, Fmt);
  verrorAt(Tok->Loc, Fmt, Ap);
}

//===----------------------------------------------------------------------===//
// Lexer Implementation
//===----------------------------------------------------------------------===//

Lexer::Lexer(const char *InputStart, const char *InputEnd,
             DiagnosticEngine &Diags)
    : BufferStart(InputStart), BufferPtr(InputStart), BufferEnd(InputEnd),
      Diags(Diags) {
  CurrentInput = InputStart;
}

std::unique_ptr<Token> Lexer::formToken(tok::TokenKind Kind,
                                        const char *TokStart) {
  auto Tok = std::make_unique<Token>(Kind, TokStart, BufferPtr - TokStart);
  return Tok;
}

bool Lexer::skipWhitespace() {
  while (BufferPtr != BufferEnd) {
    switch (*BufferPtr) {
    case ' ':
    case '\t':
    case '\f':
    case '\v':
    case '\r':
    case '\n':
      ++BufferPtr;
      break;
    default:
      return false;
    }
  }
  return true;
}

void Lexer::lexNumericConstant(Token &Result) {
  const char *CurPtr = BufferPtr;

  // Lex the number
  while (BufferPtr != BufferEnd && isdigit(*BufferPtr))
    ++BufferPtr;

  Result.Kind = tok::numeric_constant;
  Result.Loc = CurPtr;
  Result.Len = BufferPtr - CurPtr;

  // Convert to integer value
  std::string NumStr(CurPtr, BufferPtr - CurPtr);
  Result.IntegerValue = std::stoull(NumStr);
}

void Lexer::lexIdentifier(Token &Result, const char *CurPtr) {
  // Match [a-zA-Z_][a-zA-Z0-9_]*
  while (BufferPtr != BufferEnd && isIdentifierBody(*BufferPtr))
    ++BufferPtr;

  Result.Kind = tok::identifier;
  Result.Loc = CurPtr;
  Result.Len = BufferPtr - CurPtr;

  // Check if this is a keyword
  std::string Spelling(CurPtr, BufferPtr - CurPtr);

  // Simple keyword lookup - in a real implementation this would use a hash
  // table
  if (Spelling == "auto")
    Result.Kind = tok::kw_auto;
  else if (Spelling == "break")
    Result.Kind = tok::kw_break;
  else if (Spelling == "case")
    Result.Kind = tok::kw_case;
  else if (Spelling == "char")
    Result.Kind = tok::kw_char;
  else if (Spelling == "const")
    Result.Kind = tok::kw_const;
  else if (Spelling == "continue")
    Result.Kind = tok::kw_continue;
  else if (Spelling == "default")
    Result.Kind = tok::kw_default;
  else if (Spelling == "do")
    Result.Kind = tok::kw_do;
  else if (Spelling == "double")
    Result.Kind = tok::kw_double;
  else if (Spelling == "else")
    Result.Kind = tok::kw_else;
  else if (Spelling == "enum")
    Result.Kind = tok::kw_enum;
  else if (Spelling == "extern")
    Result.Kind = tok::kw_extern;
  else if (Spelling == "float")
    Result.Kind = tok::kw_float;
  else if (Spelling == "for")
    Result.Kind = tok::kw_for;
  else if (Spelling == "goto")
    Result.Kind = tok::kw_goto;
  else if (Spelling == "if")
    Result.Kind = tok::kw_if;
  else if (Spelling == "int")
    Result.Kind = tok::kw_int;
  else if (Spelling == "long")
    Result.Kind = tok::kw_long;
  else if (Spelling == "register")
    Result.Kind = tok::kw_register;
  else if (Spelling == "return")
    Result.Kind = tok::kw_return;
  else if (Spelling == "short")
    Result.Kind = tok::kw_short;
  else if (Spelling == "signed")
    Result.Kind = tok::kw_signed;
  else if (Spelling == "sizeof")
    Result.Kind = tok::kw_sizeof;
  else if (Spelling == "static")
    Result.Kind = tok::kw_static;
  else if (Spelling == "struct")
    Result.Kind = tok::kw_struct;
  else if (Spelling == "switch")
    Result.Kind = tok::kw_switch;
  else if (Spelling == "typedef")
    Result.Kind = tok::kw_typedef;
  else if (Spelling == "union")
    Result.Kind = tok::kw_union;
  else if (Spelling == "unsigned")
    Result.Kind = tok::kw_unsigned;
  else if (Spelling == "void")
    Result.Kind = tok::kw_void;
  else if (Spelling == "volatile")
    Result.Kind = tok::kw_volatile;
  else if (Spelling == "while")
    Result.Kind = tok::kw_while;
}

tok::TokenKind Lexer::tryMatchPunctuator(const char *CurPtr, unsigned &Size) {
  switch (*CurPtr) {
  case '[':
    Size = 1;
    return tok::l_square;
  case ']':
    Size = 1;
    return tok::r_square;
  case '(':
    Size = 1;
    return tok::l_paren;
  case ')':
    Size = 1;
    return tok::r_paren;
  case '{':
    Size = 1;
    return tok::l_brace;
  case '}':
    Size = 1;
    return tok::r_brace;
  case '.':
    if (CurPtr + 2 < BufferEnd && CurPtr[1] == '.' && CurPtr[2] == '.') {
      Size = 3;
      return tok::ellipsis;
    }
    Size = 1;
    return tok::period;
  case '&':
    if (CurPtr + 1 < BufferEnd && CurPtr[1] == '&') {
      Size = 2;
      return tok::ampamp;
    }
    if (CurPtr + 1 < BufferEnd && CurPtr[1] == '=') {
      Size = 2;
      return tok::ampequal;
    }
    Size = 1;
    return tok::amp;
  case '*':
    if (CurPtr + 1 < BufferEnd && CurPtr[1] == '=') {
      Size = 2;
      return tok::starequal;
    }
    Size = 1;
    return tok::star;
  case '+':
    if (CurPtr + 1 < BufferEnd && CurPtr[1] == '+') {
      Size = 2;
      return tok::plusplus;
    }
    if (CurPtr + 1 < BufferEnd && CurPtr[1] == '=') {
      Size = 2;
      return tok::plusequal;
    }
    Size = 1;
    return tok::plus;
  case '-':
    if (CurPtr + 1 < BufferEnd && CurPtr[1] == '>') {
      Size = 2;
      return tok::arrow;
    }
    if (CurPtr + 1 < BufferEnd && CurPtr[1] == '-') {
      Size = 2;
      return tok::minusminus;
    }
    if (CurPtr + 1 < BufferEnd && CurPtr[1] == '=') {
      Size = 2;
      return tok::minusequal;
    }
    Size = 1;
    return tok::minus;
  case '~':
    Size = 1;
    return tok::tilde;
  case '!':
    if (CurPtr + 1 < BufferEnd && CurPtr[1] == '=') {
      Size = 2;
      return tok::exclaimequal;
    }
    Size = 1;
    return tok::exclaim;
  case '/':
    if (CurPtr + 1 < BufferEnd && CurPtr[1] == '=') {
      Size = 2;
      return tok::slashequal;
    }
    Size = 1;
    return tok::slash;
  case '%':
    if (CurPtr + 1 < BufferEnd && CurPtr[1] == '=') {
      Size = 2;
      return tok::percentequal;
    }
    Size = 1;
    return tok::percent;
  case '<':
    if (CurPtr + 1 < BufferEnd && CurPtr[1] == '<') {
      if (CurPtr + 2 < BufferEnd && CurPtr[2] == '=') {
        Size = 3;
        return tok::lesslessequal;
      }
      Size = 2;
      return tok::lessless;
    }
    if (CurPtr + 1 < BufferEnd && CurPtr[1] == '=') {
      Size = 2;
      return tok::lessequal;
    }
    Size = 1;
    return tok::less;
  case '>':
    if (CurPtr + 1 < BufferEnd && CurPtr[1] == '>') {
      if (CurPtr + 2 < BufferEnd && CurPtr[2] == '=') {
        Size = 3;
        return tok::greatergreaterequal;
      }
      Size = 2;
      return tok::greatergreater;
    }
    if (CurPtr + 1 < BufferEnd && CurPtr[1] == '=') {
      Size = 2;
      return tok::greaterequal;
    }
    Size = 1;
    return tok::greater;
  case '^':
    if (CurPtr + 1 < BufferEnd && CurPtr[1] == '=') {
      Size = 2;
      return tok::caretequal;
    }
    Size = 1;
    return tok::caret;
  case '|':
    if (CurPtr + 1 < BufferEnd && CurPtr[1] == '|') {
      Size = 2;
      return tok::pipepipe;
    }
    if (CurPtr + 1 < BufferEnd && CurPtr[1] == '=') {
      Size = 2;
      return tok::pipeequal;
    }
    Size = 1;
    return tok::pipe;
  case '?':
    Size = 1;
    return tok::question;
  case ':':
    Size = 1;
    return tok::colon;
  case ';':
    Size = 1;
    return tok::semi;
  case '=':
    if (CurPtr + 1 < BufferEnd && CurPtr[1] == '=') {
      Size = 2;
      return tok::equalequal;
    }
    Size = 1;
    return tok::equal;
  case ',':
    Size = 1;
    return tok::comma;
  case '#':
    if (CurPtr + 1 < BufferEnd && CurPtr[1] == '#') {
      Size = 2;
      return tok::hashhash;
    }
    if (CurPtr + 1 < BufferEnd && CurPtr[1] == '@') {
      Size = 2;
      return tok::hashat;
    }
    Size = 1;
    return tok::hash;
  default:
    Size = 0;
    return tok::unknown;
  }
}

std::unique_ptr<Token> Lexer::lex() {
  // Skip whitespace
  if (skipWhitespace()) {
    return formToken(tok::eof, BufferPtr);
  }

  const char *TokStart = BufferPtr;

  // Handle end of file
  if (BufferPtr >= BufferEnd) {
    return formToken(tok::eof, BufferPtr);
  }

  unsigned char Char = *BufferPtr;

  // Identifier: [a-zA-Z_]
  if (isIdentifierHead(Char)) {
    auto Result = std::make_unique<Token>();
    lexIdentifier(*Result, TokStart);
    return Result;
  }

  // Numeric constant: [0-9]
  if (isdigit(Char)) {
    auto Result = std::make_unique<Token>();
    lexNumericConstant(*Result);
    return Result;
  }

  // Punctuator
  unsigned Size;
  tok::TokenKind Kind = tryMatchPunctuator(TokStart, Size);
  if (Kind != tok::unknown) {
    BufferPtr += Size;
    return formToken(Kind, TokStart);
  }

  // Unknown character - report diagnostic
  SourceLocation Loc(TokStart);
  Diags.report(Loc, diag::err_invalid_character,
               std::string("invalid character '") + char(*TokStart) + "'");
  ++BufferPtr;
  return formToken(tok::unknown, TokStart);
}

bool Lexer::equal(Token *Tok, const char *Op) {
  return Tok->getSpelling() == Op;
}

bool Lexer::equal(Token *Tok, tok::TokenKind Kind) { return Tok->Kind == Kind; }

Token *Lexer::skip(Token *Tok, const char *Op) {
  if (!equal(Tok, Op))
    errorTok(Tok, "expected '%s'", Op);
  return Tok->Next.get();
}

void Lexer::dumpTokens() {
  std::cerr << "=== Token Dump ===\n";

  // Save current position
  const char *SavedPtr = BufferPtr;

  // Reset to beginning
  BufferPtr = BufferStart;

  // Lex and dump all tokens
  while (true) {
    auto Tok = lex();
    if (!Tok)
      break;

    Tok->dump(BufferStart);

    if (Tok->Kind == tok::eof)
      break;
  }

  std::cerr << "=== End Token Dump ===\n\n";

  // Restore position
  BufferPtr = SavedPtr;
}

} // namespace chibcc
