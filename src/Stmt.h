#ifndef LOXALONE_Stmt_H
#define LOXALONE_Stmt_H

#include <memory>
#include <string>
#include <variant>

#include "Token.h"

#include "Expr.h"

class Block;
class Expression;
class If;
class While;
class Print;
class Var;

using BlockPtr = std::unique_ptr<Block>;
using ExpressionPtr = std::unique_ptr<Expression>;
using IfPtr = std::unique_ptr<If>;
using WhilePtr = std::unique_ptr<While>;
using PrintPtr = std::unique_ptr<Print>;
using VarPtr = std::unique_ptr<Var>;

using Stmt =
    std::variant<BlockPtr, ExpressionPtr, IfPtr, WhilePtr, PrintPtr, VarPtr>;

static auto stmt_is_null(const Stmt& stmt) {
  if (std::holds_alternative<BlockPtr>(stmt))
    return std::get<BlockPtr>(stmt) == nullptr;
  if (std::holds_alternative<ExpressionPtr>(stmt))
    return std::get<ExpressionPtr>(stmt) == nullptr;
  if (std::holds_alternative<IfPtr>(stmt))
    return std::get<IfPtr>(stmt) == nullptr;
  if (std::holds_alternative<WhilePtr>(stmt))
    return std::get<WhilePtr>(stmt) == nullptr;
  if (std::holds_alternative<PrintPtr>(stmt))
    return std::get<PrintPtr>(stmt) == nullptr;
  if (std::holds_alternative<VarPtr>(stmt))
    return std::get<VarPtr>(stmt) == nullptr;
  return false;
}

template <typename V, typename Out>
concept StmtVisitor = requires(
    V v, const BlockPtr& arg_0, const ExpressionPtr& arg_1, const IfPtr& arg_2,
    const WhilePtr& arg_3, const PrintPtr& arg_4, const VarPtr& arg_5) {
  { v(arg_0) } -> std::convertible_to<Out>;
  { v(arg_1) } -> std::convertible_to<Out>;
  { v(arg_2) } -> std::convertible_to<Out>;
  { v(arg_3) } -> std::convertible_to<Out>;
  { v(arg_4) } -> std::convertible_to<Out>;
  { v(arg_5) } -> std::convertible_to<Out>;
};

class Block {
 public:
  const std::vector<Stmt> statements_m;

  explicit Block(std::vector<Stmt>&& statements)
      : statements_m{std::move(statements)} {}
  ~Block() = default;
};

class Expression {
 public:
  const Expr expression_m;

  explicit Expression(Expr&& expression)
      : expression_m{std::move(expression)} {}
  ~Expression() = default;
};

class If {
 public:
  const Expr expression_m;
  const Token token_m;
  const Stmt then_branch_m;
  const Stmt else_branch_m;

  If(Expr&& expression, Token&& token, Stmt&& then_branch, Stmt&& else_branch)
      : expression_m{std::move(expression)},
        token_m{std::move(token)},
        then_branch_m{std::move(then_branch)},
        else_branch_m{std::move(else_branch)} {}
  ~If() = default;
};

class While {
 public:
  const Expr condition_m;
  const Stmt body_m;
  const Token token_m;

  While(Expr&& condition, Stmt&& body, Token&& token)
      : condition_m{std::move(condition)},
        body_m{std::move(body)},
        token_m{std::move(token)} {}
  ~While() = default;
};

class Print {
 public:
  const Expr expression_m;

  explicit Print(Expr&& expression) : expression_m{std::move(expression)} {}
  ~Print() = default;
};

class Var {
 public:
  const Token name_m;
  const Expr initializer_m;

  Var(Token&& name, Expr&& initializer)
      : name_m{std::move(name)}, initializer_m{std::move(initializer)} {}
  ~Var() = default;
};

#endif
