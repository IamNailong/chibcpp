#ifndef CHIBCC_COMMANDLINE_H
#define CHIBCC_COMMANDLINE_H

#include <string>
#include <vector>

namespace chibcc {
namespace cl {

//===----------------------------------------------------------------------===//
// CommandLine - Simple command line option parser inspired by LLVM
//===----------------------------------------------------------------------===//

class Option {
public:
  enum OptionKind {
    Flag,      // Boolean flag (e.g., -dump-ast)
    String,    // String value (e.g., -o output.s)
    Positional // Positional argument
  };

protected:
  std::string Name;
  std::string Description;
  OptionKind Kind;

public:
  Option(const std::string &N, const std::string &Desc, OptionKind K)
      : Name(N), Description(Desc), Kind(K) {}

  virtual ~Option() = default;

  const std::string &getName() const { return Name; }
  const std::string &getDescription() const { return Description; }
  OptionKind getKind() const { return Kind; }

  virtual bool parse(const char *Arg) = 0;
  virtual void reset() = 0;
};

// Forward declarations
class opt_positional;

// Parser registry - defined before option classes
class OptionRegistry {
  std::vector<Option *> Options;
  std::vector<opt_positional *> Positionals;
  std::string ProgramName;
  std::string Description;

  static OptionRegistry &getInstance() {
    static OptionRegistry Instance;
    return Instance;
  }

public:
  static void registerOption(Option *Opt);
  static void setDescription(const std::string &Desc) {
    getInstance().Description = Desc;
  }

  static bool parseCommandLine(int Argc, char **Argv);
  static void printHelp();
};

// Boolean flag option
class opt_bool : public Option {
  bool &Value;
  bool Default;

public:
  opt_bool(const std::string &Name, const std::string &Desc, bool &Storage,
           bool DefaultVal = false)
      : Option(Name, Desc, Flag), Value(Storage), Default(DefaultVal) {
    Value = Default;
    OptionRegistry::registerOption(this);
  }

  bool parse(const char *Arg) override {
    Value = true;
    return true;
  }

  void reset() override { Value = Default; }
};

// String option
class opt_string : public Option {
  std::string &Value;
  std::string Default;

public:
  opt_string(const std::string &Name, const std::string &Desc,
             std::string &Storage, const std::string &DefaultVal = "")
      : Option(Name, Desc, String), Value(Storage), Default(DefaultVal) {
    Value = Default;
    OptionRegistry::registerOption(this);
  }

  bool parse(const char *Arg) override {
    if (Arg) {
      Value = Arg;
      return true;
    }
    return false;
  }

  void reset() override { Value = Default; }
};

// Positional argument
class opt_positional : public Option {
  std::string &Value;
  bool Required;

public:
  opt_positional(const std::string &Name, const std::string &Desc,
                 std::string &Storage, bool Req = true)
      : Option(Name, Desc, Positional), Value(Storage), Required(Req) {
    OptionRegistry::registerOption(this);
  }

  bool parse(const char *Arg) override {
    if (Arg) {
      Value = Arg;
      return true;
    }
    return false;
  }

  void reset() override { Value = ""; }

  bool isRequired() const { return Required; }
};

// Helper function to parse command line
bool ParseCommandLineOptions(int Argc, char **Argv,
                              const std::string &Description = "");

} // namespace cl
} // namespace chibcc

#endif // CHIBCC_COMMANDLINE_H
