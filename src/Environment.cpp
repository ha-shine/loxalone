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
  if (ptr != values.end()) {
    return ptr->second;
  }
  if (enclosing != nullptr) {
    return enclosing->get(token);
  }

  throw RuntimeError{token,
                     fmt::format("Undefined variable '{}'.", token.lexeme)};
}

auto Environment::assign(const Token& token, lox_literal val) -> void {
  auto ptr = values.find(token.lexeme);
  if (ptr != values.end()) {
    ptr->second = std::move(val);
    return;
  }
  if (enclosing != nullptr) {
    enclosing->assign(token, std::move(val));
  }

  throw RuntimeError{token,
                     fmt::format("Undefined variable '{}'.", token.lexeme)};
}
