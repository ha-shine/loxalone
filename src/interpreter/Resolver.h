//
// Created by Htet Aung Shine on 19/12/2022.
//

#ifndef LOXALONE_RESOLVER_H
#define LOXALONE_RESOLVER_H

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "Expr.h"
#include "Interpreter.h"
#include "Stmt.h"
#include "Token.h"

namespace loxalone {

class Resolver {
 public:
  explicit Resolver(Interpreter &interpreter)
      : interpreter{interpreter}, scopes{}, current{FunctionType::NONE} {}

  auto resolve(const std::vector<Stmt> &) -> void;

  // Expression visitor
  auto operator()(const BinaryPtr &expr) -> void;
  auto operator()(const GroupingPtr &expr) -> void;
  auto operator()(const LiteralPtr &expr) -> void;
  auto operator()(const UnaryPtr &expr) -> void;
  auto operator()(const VariablePtr &expr) -> void;
  auto operator()(const AssignPtr &expr) -> void;
  auto operator()(const LogicalPtr &expr) -> void;
  auto operator()(const CallPtr &expr) -> void;

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

 private:
  enum class FunctionType { NONE, FUNCTION };

  auto resolve(const Stmt &) -> void;
  auto resolve_function(const FunctionPtr &stmt, FunctionType type) -> void;

  auto resolve(const Expr &) -> void;

  template <IsExpr T>
  auto resolve_local(const T &, const Token &) -> void;

  auto declare(const Token &) -> void;
  auto define(const Token &) -> void;

  auto begin_scope() -> void;
  auto end_scope() -> void;

  Interpreter &interpreter;
  std::vector<std::unordered_map<std::string, bool>> scopes;
  FunctionType current;
};

static_assert(ExprVisitor<Resolver, void>);
static_assert(StmtVisitor<Resolver, void>);
}  // namespace loxalone

#endif  // LOXALONE_RESOLVER_H
