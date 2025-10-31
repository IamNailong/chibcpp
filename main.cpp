#include "CodeGenerator.h"
#include "Diagnostic.h"
#include "Parser.h"
#include "Tokenizer.h"
#include <iostream>

using namespace chibcc;

int main(int Argc, char **Argv) {
  if (Argc != 2) {
    std::cerr << "Usage: " << Argv[0] << " <expression>" << std::endl;
    return 1;
  }

  const char *Input = Argv[1];
  
  // Create diagnostic engine
  DiagnosticEngine Diags(Input, "<command-line>");

  // Create lexer
  Lexer Lex(Input, Input + strlen(Input), Diags);
  
  // Tokenize all input into a linked list
  std::unique_ptr<Token> Head = std::make_unique<Token>();
  Token *Current = Head.get();
  
  while (true) {
    auto Tok = Lex.lex();
    Current->Next = std::move(Tok);
    Current = Current->Next.get();
    if (Current->Kind == tok::eof) {
      break;
    }
  }

  // Check for lexical errors
  if (Diags.hasErrorOccurred()) {
    return 1;
  }

  // Parse tokens into AST
  Parser P(Lex);
  auto Ast = P.parse(Head->Next.get());

  // Check for parse errors
  if (Diags.hasErrorOccurred()) {
    return 1;
  }

  // Generate assembly code
  CodeGenerator CG;
  CG.codegen(Ast.get());

  return 0;
}