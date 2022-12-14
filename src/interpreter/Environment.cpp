//
// Created by Htet Aung Shine on 13/12/2022.
//

#include "Environment.h"

#include <fmt/format.h>

#include <utility>

#include "Error.h"

namespace loxalone {

auto Environment::define(std::string_view key, lox_literal value) -> void {
  values[std::string{key}] = std::move(value);
}

// The distance to the scope is known thanks to the resolver, which means
// this function is not needed anymore? Same goes for assign below
// TODO: Revisit
auto Environment::get(const Token& token) const -> const lox_literal& {
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

auto Environment::get_at(const Token& token, int distance) const
    -> const lox_literal& {
  return ancestor(distance)->values[token.lexeme];
}

auto Environment::ancestor(int distance) const -> const Environment* {
  const Environment* env = this;
  for (int i = 0; i < distance; i++) {
    env = env->enclosing;
  }
  return env;
}

auto Environment::assign(const Token& token, lox_literal val) -> void {
  auto ptr = values.find(token.lexeme);
  if (ptr != values.end()) {
    ptr->second = std::move(val);
    return;
  }
  if (enclosing != nullptr) {
    enclosing->assign(token, std::move(val));
    return;
  }

  throw RuntimeError{token,
                     fmt::format("Undefined variable '{}'.", token.lexeme)};
}

auto Environment::assign_at(const Token& token, int distance,
                            lox_literal&& value) -> void {
  ancestor(distance)->values[token.lexeme] = value;
}

}