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
  
  // Parse input into AST (lexer is called on-demand during parsing)
  Parser P(Lex);
  auto Ast = P.parse();

  // Check for errors
  if (Diags.hasErrorOccurred()) {
    return 1;
  }

  // Generate assembly code
  CodeGenerator CG;
  CG.codegen(Ast.get());

  return 0;
}