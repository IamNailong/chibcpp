#include "Diagnostic.h"
#include <iostream>

namespace chibcpp {

//===----------------------------------------------------------------------===//
// Diagnostic Level and Text Tables
//===----------------------------------------------------------------------===//

static const DiagnosticLevel DiagnosticLevels[] = {
#define DIAG(ENUM, LEVEL, DESC) DiagnosticLevel::LEVEL,
#include "DiagnosticKinds.def"
};

static const char *DiagnosticTexts[] = {
#define DIAG(ENUM, LEVEL, DESC) DESC,
#include "DiagnosticKinds.def"
};

//===----------------------------------------------------------------------===//
// DiagnosticEngine Implementation
//===----------------------------------------------------------------------===//

DiagnosticLevel DiagnosticEngine::getDiagnosticLevel(unsigned DiagID) {
  if (DiagID >= diag::NUM_BUILTIN_DIAGNOSTICS)
    return DiagnosticLevel::Error;
  return DiagnosticLevels[DiagID];
}

const char *DiagnosticEngine::getDiagnosticText(unsigned DiagID) {
  if (DiagID >= diag::NUM_BUILTIN_DIAGNOSTICS)
    return "unknown diagnostic";
  return DiagnosticTexts[DiagID];
}

void DiagnosticEngine::emitDiagnostic(SourceLocation Loc, DiagnosticLevel Level,
                                      const std::string &Message) {
  if (SuppressAllDiagnostics)
    return;

  // Treat warnings as errors if requested
  if (Level == DiagnosticLevel::Warning && WarningsAsErrors)
    Level = DiagnosticLevel::Error;

  // Update counters
  switch (Level) {
  case DiagnosticLevel::Warning:
    NumWarnings++;
    break;
  case DiagnosticLevel::Error:
  case DiagnosticLevel::Fatal:
    NumErrors++;
    break;
  default:
    break;
  }

  // Print the diagnostic
  const char *LevelStr = "";
  switch (Level) {
  case DiagnosticLevel::Note:
    LevelStr = "note";
    break;
  case DiagnosticLevel::Remark:
    LevelStr = "remark";
    break;
  case DiagnosticLevel::Warning:
    LevelStr = "warning";
    break;
  case DiagnosticLevel::Error:
    LevelStr = "error";
    break;
  case DiagnosticLevel::Fatal:
    LevelStr = "fatal error";
    break;
  case DiagnosticLevel::Ignored:
    return; // Don't print ignored diagnostics
  }

  // Calculate line and column
  int Line = 1, Column = 1;
  if (Loc.isValid() && SourceBuffer) {
    const char *Ptr = SourceBuffer;
    while (Ptr < Loc.getPointer()) {
      if (*Ptr == '\n') {
        Line++;
        Column = 1;
      } else {
        Column++;
      }
      Ptr++;
    }
  }

  // Print diagnostic header
  std::cerr << FileName << ":" << Line << ":" << Column << ": " << LevelStr
            << ": " << Message << std::endl;

  // Print source line and caret if location is valid
  if (Loc.isValid()) {
    printSourceLine(Loc);
    printCaretDiagnostic(Loc, SourceRange(Loc));
  }

  // Exit on fatal errors
  if (Level == DiagnosticLevel::Fatal) {
    std::exit(1);
  }
}

void DiagnosticEngine::printSourceLine(SourceLocation Loc) {
  if (!SourceBuffer || Loc.isInvalid())
    return;

  const char *LineStart = Loc.getPointer();
  const char *LineEnd = Loc.getPointer();

  // Find the start of the line
  while (LineStart > SourceBuffer && LineStart[-1] != '\n')
    LineStart--;

  // Find the end of the line
  while (*LineEnd && *LineEnd != '\n' && *LineEnd != '\r')
    LineEnd++;

  // Print the source line
  std::cerr << std::string(LineStart, LineEnd) << std::endl;
}

void DiagnosticEngine::printCaretDiagnostic(SourceLocation Loc,
                                            SourceRange Range) {
  if (!SourceBuffer || Loc.isInvalid())
    return;

  const char *LineStart = Loc.getPointer();
  while (LineStart > SourceBuffer && LineStart[-1] != '\n')
    LineStart--;

  // Calculate the column position
  int Column = Loc.getPointer() - LineStart;

  // Print spaces up to the caret position
  for (int i = 0; i < Column; ++i) {
    if (LineStart[i] == '\t')
      std::cerr << '\t';
    else
      std::cerr << ' ';
  }

  // Print the caret
  std::cerr << '^';

  // If we have a range, print tildes for the rest
  if (Range.isValid() && Range.getEnd().getPointer() > Loc.getPointer()) {
    int RangeLen = Range.getEnd().getPointer() - Loc.getPointer();
    for (int i = 1; i < RangeLen; ++i) {
      std::cerr << '~';
    }
  }

  std::cerr << std::endl;
}

void DiagnosticEngine::report(SourceLocation Loc, unsigned DiagID,
                              const std::string &Message) {
  DiagnosticLevel Level = getDiagnosticLevel(DiagID);
  emitDiagnostic(Loc, Level, Message);
}

void DiagnosticEngine::report(SourceRange Range, unsigned DiagID,
                              const std::string &Message) {
  DiagnosticLevel Level = getDiagnosticLevel(DiagID);
  emitDiagnostic(Range.getBegin(), Level, Message);
}

void DiagnosticEngine::reportError(SourceLocation Loc,
                                   const std::string &Message) {
  emitDiagnostic(Loc, DiagnosticLevel::Error, Message);
}

void DiagnosticEngine::reportWarning(SourceLocation Loc,
                                     const std::string &Message) {
  emitDiagnostic(Loc, DiagnosticLevel::Warning, Message);
}

void DiagnosticEngine::reportNote(SourceLocation Loc,
                                  const std::string &Message) {
  emitDiagnostic(Loc, DiagnosticLevel::Note, Message);
}

void DiagnosticEngine::reportFatal(SourceLocation Loc,
                                   const std::string &Message) {
  emitDiagnostic(Loc, DiagnosticLevel::Fatal, Message);
}

} // namespace chibcpp
