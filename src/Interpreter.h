//
// Created by Htet Aung Shine on 9/12/22.
//

#ifndef LOXALONE_INTERPRETER_H
#define LOXALONE_INTERPRETER_H

#include <utility>

#include "Expr.h"
#include "Token.h"

class RuntimeError : std::exception {
 public:
  RuntimeError(Token token, const std::string_view& msg)
      : token{std::move(token)}, msg{msg} {}

  const std::string msg;
  const Token token;
};

class Interpreter {
 public:
  auto operator()(const BinaryExprPtr&) -> lox_literal;
  auto operator()(const GroupingExprPtr&) -> lox_literal;
  auto operator()(const LiteralValPtr&) -> lox_literal;
  auto operator()(const UnaryExprPtr&) -> lox_literal;
  auto interpret(const Expr&) -> void;

 private:
  auto check_is_number(const Token&, const lox_literal&) -> void;
  auto check_is_boolean(const Token&, const lox_literal&) -> void;
  auto check_are_numbers(const Token&, const lox_literal&, const lox_literal&) -> void;
};

static_assert(ExprVisitor<Interpreter, lox_literal>);

#endif  //LOXALONE_INTERPRETER_H
