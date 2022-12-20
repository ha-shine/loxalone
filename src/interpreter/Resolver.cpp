//
// Created by Htet Aung Shine on 19/12/2022.
//

#include "Resolver.h"

#include "Error.h"

namespace loxalone {
auto Resolver::operator()(const BlockPtr &stmt) -> void {
  begin_scope();
  resolve(stmt->statements_m);
  end_scope();
}

auto Resolver::operator()(const FunctionPtr &stmt) -> void {
  declare(stmt->name_m);
  define(stmt->name_m);

  resolve_function(stmt, FunctionType::FUNCTION);
}

auto Resolver::operator()(const VarPtr &stmt) -> void {
  declare(stmt->name_m);
  if (!expr_is_null(stmt->initializer_m)) {
    resolve(stmt->initializer_m);
  }
  define(stmt->name_m);
}

auto Resolver::operator()(const VariablePtr &expr) -> void {
  // Check if variable is being accessed in its own initializer, which means
  // the variable is defined, but value not bound yet
  if (!scopes.empty()) {
    const auto &last = scopes.back();
    if (auto find = last.find(expr->name_m.lexeme);
        find != last.end() && !find->second) {
      throw RuntimeError{expr->name_m,
                         "Can't read local variable in its own initializer."};
    }
  }

  resolve_local(expr, expr->name_m);
}

auto Resolver::operator()(const AssignPtr &expr) -> void {
  // Resolve the assigned value if it contains references to other variables
  resolve(expr->value_m);

  // Resolve the variable that's being assigned to
  resolve_local(expr, expr->name_m);
}

auto Resolver::resolve(const std::vector<Stmt> &stmts) -> void {
  for (const auto &stmt : stmts) {
    resolve(stmt);
  }
}

auto Resolver::resolve(const Stmt &stmt) -> void { visit(*this, stmt); }

auto Resolver::resolve_function(const FunctionPtr &stmt, FunctionType type)
    -> void {
  FunctionType enclosing = current;
  current = type;

  begin_scope();
  for (const auto &param : stmt->params_m) {
    declare(param);
    define(param);
  }
  resolve(stmt->body_m);
  end_scope();

  current = enclosing;
}

auto Resolver::resolve(const Expr &expr) -> void { visit(*this, expr); }

template <IsExpr T>
auto Resolver::resolve_local(const T &expr, const Token &token) -> void {
  for (int i = static_cast<int>(scopes.size() - 1); i >= 0; i--) {
    if (scopes[i].find(token.lexeme) != scopes[i].end()) {
      interpreter.resolve(expr, scopes.size() - 1 - i);
      return;
    }
  }
}

auto Resolver::begin_scope() -> void { scopes.emplace_back(); }

auto Resolver::end_scope() -> void { scopes.pop_back(); }

auto Resolver::declare(const Token &token) -> void {
  if (scopes.empty()) return;

  const auto &last = scopes.back();
  if (auto find = last.find(token.lexeme); find != last.end())
    throw RuntimeError{token,
                       "Already a variable with this name in this scope"};
  scopes.back()[token.lexeme] = false;
}

auto Resolver::define(const Token &token) -> void {
  if (scopes.empty()) return;
  scopes.back()[token.lexeme] = true;
}

auto Resolver::operator()(const BinaryPtr &expr) -> void {
  resolve(expr->left_m);
  resolve(expr->right_m);
}

auto Resolver::operator()(const GroupingPtr &expr) -> void {
  resolve(expr->expression_m);
}

auto Resolver::operator()(const LiteralPtr &expr) -> void {}

auto Resolver::operator()(const UnaryPtr &expr) -> void {
  resolve(expr->right_m);
}

auto Resolver::operator()(const LogicalPtr &expr) -> void {
  resolve(expr->left_m);
  resolve(expr->right_m);
}

auto Resolver::operator()(const CallPtr &expr) -> void {
  resolve(expr->callee_m);
  for (const auto &arg : expr->arguments_m) {
    resolve(arg);
  }
}

auto Resolver::operator()(const ExpressionPtr &stmt) -> void {
  resolve(stmt->expression_m);
}

auto Resolver::operator()(const PrintPtr &stmt) -> void {
  resolve(stmt->expression_m);
}

auto Resolver::operator()(const IfPtr &stmt) -> void {
  resolve(stmt->expression_m);
  resolve(stmt->then_branch_m);
  if (!stmt_is_null(stmt->else_branch_m)) resolve(stmt->else_branch_m);
}

auto Resolver::operator()(const WhilePtr &stmt) -> void {
  resolve(stmt->condition_m);
  resolve(stmt->body_m);
}

auto Resolver::operator()(const ReturnPtr &stmt) -> void {
  if (current == FunctionType::NONE)
    throw RuntimeError{stmt->keyword_m, "Can't return from top-level code."};

  if (!expr_is_null(stmt->value_m)) resolve(stmt->value_m);
}
}  // namespace loxalone