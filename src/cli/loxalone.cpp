#include <fmt/format.h>
#include <sysexits.h>

#include <fstream>
#include <iostream>
#include <string_view>

#include "../Interpreter.h"
#include "../Parser.h"
#include "../Resolver.h"
#include "../Scanner.h"

const auto USAGE = "Usage: loxalone [script]";

// Probably should return result rather than boolean
// And also each line is run once here, the interpreter probably needs to
// maintain internal state (e.g. variables set, classes defined, etc.)
// and probably needs to move the run function out of this
auto run(Interpreter& interpreter, const std::string_view& source) -> bool {
  Scanner scanner{source};

  std::optional<std::vector<Token>> tokens{scanner.scan_tokens()};
  if (!tokens.has_value()) return false;

  Parser parser{tokens.value()};
  std::vector<Stmt> statements = parser.parse();

  try {
    Resolver resolver{interpreter};
    resolver.resolve(statements);

    return interpreter.interpret(statements);
  } catch (const ParserError& err) {
    report(err.token.line, "", err.msg);
  } catch (const RuntimeError& err) {
    report(err.token.line, "", err.msg);
  }

  return false;
}

// TODO: Non-existent files are not being reported here, fix this.
auto run_file(const std::string_view& file) -> int {
  try {
    std::ifstream fs{file};
    std::string source{};

    char c;
    while (fs.get(c))
      source.push_back(c);

    Scanner scanner{source};
    std::optional<std::vector<Token>> tokens{scanner.scan_tokens()};
    if (!tokens.has_value()) return false;

    Parser parser{tokens.value()};
    std::vector<Stmt> statements = parser.parse();

    Interpreter interpreter{};
    Resolver resolver{interpreter};
    resolver.resolve(statements);
    return interpreter.interpret(statements);
  } catch (const ParserError& err) {
    report(err.token.line, "", err.msg);
  } catch (const RuntimeError& err) {
    report(err.token.line, "", err.msg);
  }

  return 1;
}

auto run_prompt() -> int {
  Interpreter interpreter{};

  std::string input;

  while (true) {
    fmt::print("> ");
    std::getline(std::cin, input);

    if (input.length() > 0) {
      run(interpreter, input);
    }
  }
}

int main(int argc, char** argv) {
  if (argc > 2) {
    fmt::print("{}\n", USAGE);
    return EX_USAGE;
  } else if (argc == 2) {
    return run_file(argv[1]);
  } else {
    return run_prompt();
  }
}
