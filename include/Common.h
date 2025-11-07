#ifndef CHIBCC_COMMON_H
#define CHIBCC_COMMON_H

#include <cassert>
#include <cctype>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

namespace chibcc {

//===----------------------------------------------------------------------===//
// Forward Declarations
//===----------------------------------------------------------------------===//

class DiagnosticEngine;
class SourceLocation;

//===----------------------------------------------------------------------===//
// Legacy Error Handling (for compatibility)
//===----------------------------------------------------------------------===//

void error(const char *Fmt, ...);
void errorAt(const char *Loc, const char *Fmt, ...);

} // namespace chibcc

#endif // CHIBCC_COMMON_H