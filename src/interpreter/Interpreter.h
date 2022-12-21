//
// Created by Htet Aung Shine on 9/12/22.
//

#ifndef LOXALONE_INTERPRETER_H
#define LOXALONE_INTERPRETER_H

#include <unordered_map>
#include <utility>
#include <vector>

#include "Environment.h"
#include "Error.h"
#include "Expr.h"
#include "LoxCallable.h"
#include "Stmt.h"
#include "Token.h"

namespace loxalone {
class Interpreter {
 public:
  Interpreter();

  // Expression visitor
  auto operator()(const BinaryPtr &expr) -> lox_literal;
  auto operator()(const GroupingPtr &expr) -> lox_literal;
  auto operator()(const LiteralPtr &expr) -> lox_literal;
  auto operator()(const UnaryPtr &expr) -> lox_literal;
  auto operator()(const VariablePtr &expr) -> lox_literal;
  auto operator()(const AssignPtr &expr) -> lox_literal;
  auto operator()(const LogicalPtr &expr) -> lox_literal;
  auto operator()(const CallPtr &expr) -> lox_literal;

  // Statement visitors
  auto operator()(const BlockPtr &stmt) -> void;
  auto operator()(const ExpressionPtr &stmt) -> void;
  auto operator()(const FunctionPtr &stmt) -> void;
  auto operator()(const PrintPtr &stmt) -> void;
  auto operator()(const VarPtr &stmt) -> void;
  auto operator()(const IfPtr &stmt) -> void;
  auto operator()(const WhilePtr &stmt) -> void;
  auto operator()(const ReturnPtr &stmt) -> void;
  auto operator()(const ClassPtr &stmt) -> void;

  // Entry point for interpreter, returns true if it's successful
  // or false if there's a runtime error
  auto interpret(const std::vector<Stmt> &) -> bool;

  // Resolver related methods
  template <IsExpr T>
  auto resolve(const T &expr, int depth) -> void {
    locals[static_cast<void *>(expr.get())] = depth;
  }

  template <IsExpr T>
  auto lookup_variable(const Token &token, const T &expr) -> lox_literal {
    void *ptr = static_cast<void *>(expr.get());
    auto find = locals.find(ptr);
    if (find != locals.end()) {
      return env->get_at(token, find->second);
    } else {
      return globals.get(token);
    }
  }

  // Other helper methods
  auto execute_block(const std::vector<Stmt> &, Environment *) -> void;
  auto get_globals() const -> const Environment &;

 private:
  auto check_is_number(const Token &, const lox_literal &) -> void;
  auto check_is_boolean(const Token &, const lox_literal &) -> void;
  auto check_are_numbers(const Token &, const lox_literal &,
                         const lox_literal &) -> void;

  Environment *env;
  Environment globals;
  std::unordered_map<void *, int> locals;
};

static_assert(ExprVisitor<Interpreter, lox_literal>);
static_assert(StmtVisitor<Interpreter, void>);

// `ReturnObject` encapsulates a literal value and used as a return object
// from lox functions. The interpreter will throw `ReturnObject` which should
// be caught by the function object and return the literal value from inside.
class ReturnObject : public std::exception {
 public:
  lox_literal value;

  explicit ReturnObject(lox_literal value) : std::exception{}, value{value} {}
};
}  // namespace loxalone

#endif  // LOXALONE_INTERPRETER_H
