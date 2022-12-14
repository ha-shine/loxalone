//
// Created by Htet Aung Shine on 13/12/2022.
//

#ifndef LOXALONE_ENVIRONMENT_H
#define LOXALONE_ENVIRONMENT_H

#include <optional>
#include <string>
#include <unordered_map>

#include "Token.h"

namespace loxalone {

/*
 * Environment class represents the state of the interpreter. The variables
 * defined will be stored in this as a map.
 * */
class Environment {
 public:
  explicit Environment(Environment *enclosing)
      : values{}, enclosing{enclosing} {}
  Environment() : Environment{nullptr} {}

  auto define(std::string_view key, lox_literal value) -> void;

  auto get(const Token &) const -> const lox_literal &;
  auto get_at(const Token &, int) const -> const lox_literal &;

  auto assign(const Token &, lox_literal) -> void;
  auto assign_at(const Token &token, int distance, lox_literal &&value) -> void;

 private:
  // Returns the ancestor at the given distance
  auto ancestor(int distance) const -> const Environment *;

  mutable std::unordered_map<std::string, lox_literal> values;

  // It's okay to use a pointer for the enclosing environment as the parent
  // environment will ALWAYS be alive when the current scope is active.
  Environment *enclosing;
};

}

#endif  // LOXALONE_ENVIRONMENT_H
