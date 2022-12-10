#include <fmt/format.h>
#include <sysexits.h>
#include <iostream>
#include <string_view>

#include "../Interpreter.h"
#include "../Parser.h"
#include "../PrettyPrinter.h"
#include "../Scanner.h"

const auto USAGE = "Usage: loxalone [script]";

// Probably should return result rather than boolean
// And also each line is run once here, the interpreter probably needs to
// maintain internal state (e.g variables set, classes defined, etc.)
// and probably needs to move the run function out of this
auto run(const std::string_view& source) -> bool {
  Scanner scanner{source};
  std::optional<std::vector<Token>> tokens{scanner.scan_tokens()};

  if (!tokens.has_value())
    return false;

  Parser parser{tokens.value()};
  std::optional<Expr> expression = parser.parse();

  if (!expression.has_value())
    return false;

  Interpreter interpreter{};
  interpreter.interpret(expression.value());
  return true;
}

auto run_file(const std::string_view& file) -> int {
  return 0;
}

auto run_prompt() -> int {
  std::string input;

  while (true) {
    fmt::print("> ");
    std::getline(std::cin, input);

    if (input.length() > 0) {
      run(input);
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
