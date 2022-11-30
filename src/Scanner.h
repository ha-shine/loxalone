//
// Created by Htet Aung Shine on 22/11/2022.
//

#ifndef LOXALONE_SCANNER_H
#define LOXALONE_SCANNER_H

#include <string>
#include <vector>
#include <optional>

#include "Token.h"

class Scanner {
 public:
  explicit Scanner(const std::string_view& source) : source{source}, tokens{} {}

  auto scan_tokens() -> std::optional<std::vector<Token>>;

 private:
  auto scan_token() -> void;
  auto is_at_end() -> bool;
  auto advance() -> char;
  auto match(char c) -> bool;
  auto peek() -> char;
  auto peek_next() -> char;

  auto string() -> void;
  auto number() -> void;
  auto identifier() -> void;

  auto add_token(TokenType) -> void;
  auto add_token(TokenType, std::optional<lox_literal>&&) -> void;

  auto error(int line, const std::string_view& msg) -> void;

  const std::string_view& source;
  std::vector<Token> tokens;

  int start_m = 0;
  int current_m = 0;
  int line_m = 1;
  bool well_formed_m = true;
};

#endif  //LOXALONE_SCANNER_H
