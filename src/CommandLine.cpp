#include "CommandLine.h"
#include <cctype>
#include <cstring>
#include <iostream>

namespace chibcpp {
namespace cl {

void OptionRegistry::registerOption(Option *Opt) {
  getInstance().Options.push_back(Opt);
  if (Opt->getKind() == Option::Positional) {
    getInstance().Positionals.push_back(static_cast<opt_positional *>(Opt));
  }
}

bool OptionRegistry::parseCommandLine(int Argc, char **Argv) {
  auto &Registry = getInstance();
  Registry.ProgramName = Argv[0];

  int PositionalIndex = 0;

  for (int I = 1; I < Argc; ++I) {
    const char *Arg = Argv[I];

    // Check for help flag
    if (strcmp(Arg, "-help") == 0 || strcmp(Arg, "--help") == 0 ||
        strcmp(Arg, "-h") == 0) {
      printHelp();
      exit(0);
    }

    // Check if it's an option (starts with -)
    if (Arg[0] == '-' && Arg[1] != '\0') {
      // Check if it's a negative number (like -5, -123)
      bool IsNumber = true;
      for (int J = 1; Arg[J] != '\0'; ++J) {
        if (!std::isdigit(static_cast<unsigned char>(Arg[J]))) {
          IsNumber = false;
          break;
        }
      }

      // If it looks like a negative number, treat as positional
      if (IsNumber) {
        if (PositionalIndex < (int)Registry.Positionals.size()) {
          Registry.Positionals[PositionalIndex]->parse(Arg);
          ++PositionalIndex;
        } else {
          std::cerr << "Error: Too many positional arguments\n";
          return false;
        }
        continue;
      }

      // Otherwise, try to match with registered options
      bool Found = false;
      for (auto *Opt : Registry.Options) {
        if (Opt->getKind() == Option::Positional)
          continue;

        std::string OptName = "-" + Opt->getName();
        if (strcmp(Arg, OptName.c_str()) == 0) {
          Found = true;

          if (Opt->getKind() == Option::Flag) {
            Opt->parse(nullptr);
          } else if (Opt->getKind() == Option::String) {
            // Next argument should be the value
            if (I + 1 >= Argc) {
              std::cerr << "Error: Option '" << Arg
                        << "' requires an argument\n";
              return false;
            }
            Opt->parse(Argv[++I]);
          }
          break;
        }
      }

      if (!Found) {
        std::cerr << "Error: Unknown option '" << Arg << "'\n";
        printHelp();
        return false;
      }
    } else {
      // Positional argument
      if (PositionalIndex < (int)Registry.Positionals.size()) {
        Registry.Positionals[PositionalIndex]->parse(Arg);
        ++PositionalIndex;
      } else {
        std::cerr << "Error: Too many positional arguments\n";
        return false;
      }
    }
  }

  // Check if all required positional arguments are provided
  for (size_t I = PositionalIndex; I < Registry.Positionals.size(); ++I) {
    if (Registry.Positionals[I]->isRequired()) {
      std::cerr << "Error: Missing required argument '"
                << Registry.Positionals[I]->getName() << "'\n";
      printHelp();
      return false;
    }
  }

  return true;
}

void OptionRegistry::printHelp() {
  auto &Registry = getInstance();

  std::cerr << "OVERVIEW: " << Registry.Description << "\n\n";
  std::cerr << "USAGE: " << Registry.ProgramName;

  // Print positional arguments
  for (auto *Pos : Registry.Positionals) {
    if (Pos->isRequired()) {
      std::cerr << " <" << Pos->getName() << ">";
    } else {
      std::cerr << " [" << Pos->getName() << "]";
    }
  }

  std::cerr << " [options]\n\n";

  // Print options
  if (!Registry.Options.empty()) {
    std::cerr << "OPTIONS:\n";

    for (auto *Opt : Registry.Options) {
      if (Opt->getKind() == Option::Positional)
        continue;

      std::cerr << "  -" << Opt->getName();

      if (Opt->getKind() == Option::String) {
        std::cerr << " <value>";
      }

      std::cerr << "\n";
      std::cerr << "      " << Opt->getDescription() << "\n";
    }

    std::cerr << "  -help\n";
    std::cerr << "      Display this help message\n";
  }
}

bool ParseCommandLineOptions(int Argc, char **Argv,
                              const std::string &Description) {
  if (!Description.empty()) {
    OptionRegistry::setDescription(Description);
  }
  return OptionRegistry::parseCommandLine(Argc, Argv);
}

} // namespace cl
} // namespace chibcpp
