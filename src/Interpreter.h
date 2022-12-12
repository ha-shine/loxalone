//
// Created by Htet Aung Shine on 9/12/22.
//

#ifndef LOXALONE_INTERPRETER_H
#define LOXALONE_INTERPRETER_H

#include <utility>
#include <vector>

#include "Expr.h"
#include "Stmt.h"
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
  // Expression visitor
  auto operator()(const BinaryExprPtr&) -> lox_literal;
  auto operator()(const GroupingExprPtr&) -> lox_literal;
  auto operator()(const LiteralValPtr&) -> lox_literal;
  auto operator()(const UnaryExprPtr&) -> lox_literal;
  auto operator()(const VariablePtr&) -> lox_literal;

  // Statement visitors
  auto operator()(const ExpressionPtr&) -> void;
  auto operator()(const PrintPtr&) -> void;
  auto operator()(const VarPtr&) -> void;

  // Entry point for interpreter, returns true if it's successful
  // or false if there's a runtime error
  auto interpret(const std::vector<Stmt>&) -> bool;

 private:
  auto check_is_number(const Token&, const lox_literal&) -> void;
  auto check_is_boolean(const Token&, const lox_literal&) -> void;
  auto check_are_numbers(const Token&, const lox_literal&, const lox_literal&) -> void;
};

static_assert(ExprVisitor<Interpreter, lox_literal>);
static_assert(StmtVisitor<Interpreter, void>);

#endif  //LOXALONE_INTERPRETER_H
