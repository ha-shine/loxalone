#include <sysexits.h>
#include <iostream>
#include <string_view>
#include <fmt/format.h>

#include "../Scanner.h"

const auto USAGE = "Usage: loxalone [script]";

// Probably should return result rather than boolean
auto run(const std::string_view& source) -> bool {
  Scanner scanner{source};
  std::optional<std::vector<Token>> tokens{scanner.scan_tokens()};

  if (tokens.has_value()) {
    for (const auto& token : tokens.value()) {
      fmt::print("{} ", token);
    }
    fmt::print("\n");
  }
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
      if (bool ok = run(input); !ok) {
        return EX_DATAERR;
      }
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
