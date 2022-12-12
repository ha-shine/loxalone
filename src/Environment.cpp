//
// Created by Htet Aung Shine on 13/12/2022.
//

#include "Environment.h"

#include <fmt/format.h>
#include <utility>

#include "Error.h"

auto Environment::define(const std::string_view& key, lox_literal val) -> void {
  values[std::string{key}] = std::move(val);
}

auto Environment::get(const Token& token) -> const lox_literal& {
  auto ptr = values.find(token.lexeme);
  if (ptr == values.end()) {
    throw RuntimeError{token,
                       fmt::format("Undefined variable '{}'.", token.lexeme)};
  }

  return ptr->second;
}

auto Environment::assign(const Token& token, lox_literal val) -> void {
  if (values.find(token.lexeme) != values.end()) {
    values[token.lexeme] = std::move(val);
    return;
  }

  throw RuntimeError{token,
                     fmt::format("Undefined variable '{}'.", token.lexeme)};
}
