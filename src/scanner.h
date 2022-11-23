//
// Created by Htet Aung Shine on 22/11/2022.
//

#ifndef LOXALONE_SCANNER_H
#define LOXALONE_SCANNER_H

#include <string>
#include <vector>
#include <optional>

#include "tokens.h"

class Scanner {
 public:
  explicit Scanner(const std::string_view& source) : source{source}, tokens{} {}

  auto scan_tokens() -> std::vector<Token>;

 private:
  auto scan_token() -> void;
  auto is_at_end() -> bool;
  auto advance() -> char;
  auto match(char c) -> bool;
  auto peek() -> char;
  auto peek_next() -> char;

  auto string() -> void;
  auto number() -> void;

  auto add_token(TokenType) -> void;
  auto add_token(TokenType, std::optional<lox_literal>&&) -> void;

  const std::string_view& source;
  std::vector<Token> tokens;

  int start = 0;
  int current = 0;
  int line = 1;
};

#endif  //LOXALONE_SCANNER_H
