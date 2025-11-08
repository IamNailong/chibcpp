#include "CodeGenerator.h"
#include "CommandLine.h"
#include "Diagnostic.h"
#include "Parser.h"
#include "Tokenizer.h"
#include <cstring>
#include <iostream>

using namespace chibcc;

// Command line options
static bool DumpTokens = false;
static bool DumpAST = false;
static std::string InputExpr;
static std::string OutputFile = "output.s";

static cl::opt_bool OptDumpTokens("dump-tokens", "Dump all tokens to stderr",
                                  DumpTokens);

static cl::opt_bool OptDumpAST("dump-ast", "Dump the AST to stderr", DumpAST);

static cl::opt_string OptOutput("o", "Output file (default: output.s)",
                                OutputFile, "output.s");

static cl::opt_positional OptInput("expression", "Input expression to compile",
                                   InputExpr);

int main(int Argc, char **Argv) {
  // Parse command line options
  if (!cl::ParseCommandLineOptions(
          Argc, Argv, "chibcpp - A small C compiler inspired by chibcc")) {
    return 1;
  }

  const char *Input = InputExpr.c_str();

  // Create diagnostic engine
  DiagnosticEngine Diags(Input, "chibcc");

  // Create lexer
  Lexer Lex(Input, Input + strlen(Input), Diags);

  // Dump tokens if requested
  if (DumpTokens) {
    Lex.dumpTokens();
  }

  // Parse input into AST (lexer is called on-demand during parsing)
  Parser P(Lex);
  auto Ast = P.parse();

  // Check for errors
  if (Diags.hasErrorOccurred()) {
    return 1;
  }

  // Dump AST if requested
  if (DumpAST) {
    std::cerr << "=== AST Dump ===\n";
    Ast->dump();
    std::cerr << "=== End AST Dump ===\n\n";
  }

  // Generate assembly code
  CodeGenerator CG;

  // Set output file
  if (!CG.setOutputFile(OutputFile.c_str())) {
    return 1;
  }

  CG.codegen(Ast.get());

  return 0;
}
