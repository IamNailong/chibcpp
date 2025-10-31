#ifndef CHIBCC_DIAGNOSTIC_H
#define CHIBCC_DIAGNOSTIC_H

#include "Common.h"

namespace chibcc {

//===----------------------------------------------------------------------===//
// Diagnostic Levels
//===----------------------------------------------------------------------===//

enum class DiagnosticLevel {
  Ignored = 0,
  Note,
  Remark,
  Warning,
  Error,
  Fatal
};

//===----------------------------------------------------------------------===//
// Diagnostic IDs
//===----------------------------------------------------------------------===//

namespace diag {
enum {
#define DIAG(ENUM, LEVEL, DESC) ENUM,
#include "DiagnosticKinds.def"
  NUM_BUILTIN_DIAGNOSTICS
};
} // namespace diag

//===----------------------------------------------------------------------===//
// Source Location
//===----------------------------------------------------------------------===//

class SourceLocation {
private:
  const char *Ptr;

public:
  SourceLocation() : Ptr(nullptr) {}
  explicit SourceLocation(const char *Loc) : Ptr(Loc) {}

  bool isValid() const { return Ptr != nullptr; }
  bool isInvalid() const { return Ptr == nullptr; }

  const char *getPointer() const { return Ptr; }

  bool operator==(const SourceLocation &RHS) const { return Ptr == RHS.Ptr; }
  bool operator!=(const SourceLocation &RHS) const { return Ptr != RHS.Ptr; }
};

//===----------------------------------------------------------------------===//
// Source Range
//===----------------------------------------------------------------------===//

class SourceRange {
private:
  SourceLocation Begin, End;

public:
  SourceRange() = default;
  SourceRange(SourceLocation Loc) : Begin(Loc), End(Loc) {}
  SourceRange(SourceLocation Begin, SourceLocation End)
      : Begin(Begin), End(End) {}

  SourceLocation getBegin() const { return Begin; }
  SourceLocation getEnd() const { return End; }

  void setBegin(SourceLocation Loc) { Begin = Loc; }
  void setEnd(SourceLocation Loc) { End = Loc; }

  bool isValid() const { return Begin.isValid() && End.isValid(); }
  bool isInvalid() const { return !isValid(); }
};

//===----------------------------------------------------------------------===//
// Diagnostic Engine
//===----------------------------------------------------------------------===//

class DiagnosticEngine {
private:
  const char *SourceBuffer;
  std::string FileName;
  unsigned NumWarnings;
  unsigned NumErrors;
  bool SuppressAllDiagnostics;
  bool WarningsAsErrors;

  void emitDiagnostic(SourceLocation Loc, DiagnosticLevel Level,
                      const std::string &Message);
  void printSourceLine(SourceLocation Loc);
  void printCaretDiagnostic(SourceLocation Loc, SourceRange Range);

public:
  DiagnosticEngine(const char *Buffer, const std::string &File = "<input>")
      : SourceBuffer(Buffer), FileName(File), NumWarnings(0), NumErrors(0),
        SuppressAllDiagnostics(false), WarningsAsErrors(false) {}

  /// \brief Report a diagnostic at the given location.
  void report(SourceLocation Loc, unsigned DiagID, const std::string &Message);

  /// \brief Report a diagnostic with a source range.
  void report(SourceRange Range, unsigned DiagID, const std::string &Message);

  /// \brief Convenience methods for common diagnostic levels
  void reportError(SourceLocation Loc, const std::string &Message);
  void reportWarning(SourceLocation Loc, const std::string &Message);
  void reportNote(SourceLocation Loc, const std::string &Message);
  void reportFatal(SourceLocation Loc, const std::string &Message);

  /// \brief Get diagnostic counts
  unsigned getNumWarnings() const { return NumWarnings; }
  unsigned getNumErrors() const { return NumErrors; }
  bool hasErrorOccurred() const { return NumErrors > 0; }

  /// \brief Control diagnostic behavior
  void setSuppressAllDiagnostics(bool Val = true) {
    SuppressAllDiagnostics = Val;
  }
  void setWarningsAsErrors(bool Val = true) { WarningsAsErrors = Val; }

  /// \brief Get the diagnostic level for a given diagnostic ID
  static DiagnosticLevel getDiagnosticLevel(unsigned DiagID);

  /// \brief Get the diagnostic description for a given diagnostic ID
  static const char *getDiagnosticText(unsigned DiagID);
};

//===----------------------------------------------------------------------===//
// Diagnostic Builder
//===----------------------------------------------------------------------===//

class DiagnosticBuilder {
private:
  DiagnosticEngine *Engine;
  SourceLocation Loc;
  SourceRange Range;
  unsigned DiagID;
  std::string Message;
  bool IsActive;

public:
  DiagnosticBuilder(DiagnosticEngine *Engine, SourceLocation Loc,
                    unsigned DiagID)
      : Engine(Engine), Loc(Loc), Range(Loc), DiagID(DiagID), IsActive(true) {}

  DiagnosticBuilder(DiagnosticBuilder &&Other)
      : Engine(Other.Engine), Loc(Other.Loc), Range(Other.Range),
        DiagID(Other.DiagID), Message(std::move(Other.Message)),
        IsActive(Other.IsActive) {
    Other.IsActive = false;
  }

  ~DiagnosticBuilder() {
    if (IsActive && Engine) {
      Engine->report(Range, DiagID, Message);
    }
  }

  /// \brief Add a string to the diagnostic message
  DiagnosticBuilder &operator<<(const std::string &Str) {
    Message += Str;
    return *this;
  }

  DiagnosticBuilder &operator<<(const char *Str) {
    Message += Str;
    return *this;
  }

  DiagnosticBuilder &operator<<(int Val) {
    Message += std::to_string(Val);
    return *this;
  }

  /// \brief Add a source range to highlight
  DiagnosticBuilder &addRange(SourceRange R) {
    Range = R;
    return *this;
  }

  /// \brief Add a fix-it hint
  DiagnosticBuilder &addFixItHint(SourceRange, const std::string &Text) {
    // For now, just add to message - could be enhanced later
    Message += " (fix: replace with '" + Text + "')";
    return *this;
  }
};

} // namespace chibcc

#endif // CHIBCC_DIAGNOSTIC_H