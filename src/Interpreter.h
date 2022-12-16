//
// Created by Htet Aung Shine on 9/12/22.
//

#ifndef LOXALONE_INTERPRETER_H
#define LOXALONE_INTERPRETER_H

#include <utility>
#include <vector>

#include "Environment.h"
#include "Error.h"
#include "Expr.h"
#include "Stmt.h"
#include "Token.h"
#include "LoxCallable.h"

class Interpreter {
 public:
  Interpreter() : env{} {}

  // Expression visitor
  auto operator()(const BinaryPtr&) -> lox_literal;
  auto operator()(const GroupingPtr&) -> lox_literal;
  auto operator()(const LiteralPtr&) -> lox_literal;
  auto operator()(const UnaryPtr&) -> lox_literal;
  auto operator()(const VariablePtr&) -> lox_literal;
  auto operator()(const AssignPtr&) -> lox_literal;
  auto operator()(const LogicalPtr&) -> lox_literal;
  auto operator()(const CallPtr&) -> lox_literal;

  // Statement visitors
  auto operator()(const BlockPtr&) -> void;
  auto operator()(const ExpressionPtr&) -> void;
  auto operator()(const PrintPtr&) -> void;
  auto operator()(const VarPtr&) -> void;
  auto operator()(const IfPtr&) -> void;
  auto operator()(const WhilePtr&) -> void;

  // Entry point for interpreter, returns true if it's successful
  // or false if there's a runtime error
  auto interpret(const std::vector<Stmt>&) -> bool;

 private:
  auto check_is_number(const Token&, const lox_literal&) -> void;
  auto check_is_boolean(const Token&, const lox_literal&) -> void;
  auto check_are_numbers(const Token&, const lox_literal&, const lox_literal&)
      -> void;

  Environment env;
};

static_assert(ExprVisitor<Interpreter, lox_literal>);
static_assert(StmtVisitor<Interpreter, void>);

#endif  //LOXALONE_INTERPRETER_H
