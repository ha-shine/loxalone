#ifndef LOXALONE_Stmt_H
#define LOXALONE_Stmt_H

#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "Expr.h"
#include "Token.h"

class Block;
class Expression;
class Function;
class If;
class While;
class Print;
class Return;
class Var;

using BlockPtr = std::unique_ptr<Block>;
using ExpressionPtr = std::unique_ptr<Expression>;
using FunctionPtr = std::unique_ptr<Function>;
using IfPtr = std::unique_ptr<If>;
using WhilePtr = std::unique_ptr<While>;
using PrintPtr = std::unique_ptr<Print>;
using ReturnPtr = std::unique_ptr<Return>;
using VarPtr = std::unique_ptr<Var>;

using Stmt = std::variant<BlockPtr, ExpressionPtr, FunctionPtr, IfPtr, WhilePtr,
                          PrintPtr, ReturnPtr, VarPtr>;

static auto stmt_is_null(const Stmt& stmt) {
  if (std::holds_alternative<BlockPtr>(stmt))
    return std::get<BlockPtr>(stmt) == nullptr;
  if (std::holds_alternative<ExpressionPtr>(stmt))
    return std::get<ExpressionPtr>(stmt) == nullptr;
  if (std::holds_alternative<FunctionPtr>(stmt))
    return std::get<FunctionPtr>(stmt) == nullptr;
  if (std::holds_alternative<IfPtr>(stmt))
    return std::get<IfPtr>(stmt) == nullptr;
  if (std::holds_alternative<WhilePtr>(stmt))
    return std::get<WhilePtr>(stmt) == nullptr;
  if (std::holds_alternative<PrintPtr>(stmt))
    return std::get<PrintPtr>(stmt) == nullptr;
  if (std::holds_alternative<ReturnPtr>(stmt))
    return std::get<ReturnPtr>(stmt) == nullptr;
  if (std::holds_alternative<VarPtr>(stmt))
    return std::get<VarPtr>(stmt) == nullptr;
  return false;
}

template <typename V, typename Out>
concept StmtVisitor = requires(
    V v, const BlockPtr& arg_0, const ExpressionPtr& arg_1,
    const FunctionPtr& arg_2, const IfPtr& arg_3, const WhilePtr& arg_4,
    const PrintPtr& arg_5, const ReturnPtr& arg_6, const VarPtr& arg_7) {
                        { v(arg_0) } -> std::convertible_to<Out>;
                        { v(arg_1) } -> std::convertible_to<Out>;
                        { v(arg_2) } -> std::convertible_to<Out>;
                        { v(arg_3) } -> std::convertible_to<Out>;
                        { v(arg_4) } -> std::convertible_to<Out>;
                        { v(arg_5) } -> std::convertible_to<Out>;
                        { v(arg_6) } -> std::convertible_to<Out>;
                        { v(arg_7) } -> std::convertible_to<Out>;
                      };

class Block {
 public:
  const std::vector<Stmt> statements_m;

  explicit Block(std::vector<Stmt>&& statements)
      : statements_m{std::move(statements)} {}
  ~Block() = default;

  static auto create(std::vector<Stmt>&& statements) -> Stmt {
    return std::make_unique<Block>(std::move(statements));
  }

  static auto empty() -> Stmt { return std::unique_ptr<Block>(nullptr); }
};

class Expression {
 public:
  const Expr expression_m;

  explicit Expression(Expr&& expression)
      : expression_m{std::move(expression)} {}
  ~Expression() = default;

  static auto create(Expr&& expression) -> Stmt {
    return std::make_unique<Expression>(std::move(expression));
  }

  static auto empty() -> Stmt { return std::unique_ptr<Expression>(nullptr); }
};

class Function {
 public:
  const Token name_m;
  const std::vector<Token> params_m;
  const std::vector<Stmt> body_m;

  Function(Token&& name, std::vector<Token>&& params, std::vector<Stmt>&& body)
      : name_m{std::move(name)},
        params_m{std::move(params)},
        body_m{std::move(body)} {}
  ~Function() = default;

  static auto create(Token&& name, std::vector<Token>&& params,
                     std::vector<Stmt>&& body) -> Stmt {
    return std::make_unique<Function>(std::move(name), std::move(params),
                                      std::move(body));
  }

  static auto empty() -> Stmt { return std::unique_ptr<Function>(nullptr); }
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

  static auto create(Expr&& expression, Token&& token, Stmt&& then_branch,
                     Stmt&& else_branch) -> Stmt {
    return std::make_unique<If>(std::move(expression), std::move(token),
                                std::move(then_branch), std::move(else_branch));
  }

  static auto empty() -> Stmt { return std::unique_ptr<If>(nullptr); }
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

  static auto create(Expr&& condition, Stmt&& body, Token&& token) -> Stmt {
    return std::make_unique<While>(std::move(condition), std::move(body),
                                   std::move(token));
  }

  static auto empty() -> Stmt { return std::unique_ptr<While>(nullptr); }
};

class Print {
 public:
  const Expr expression_m;

  explicit Print(Expr&& expression) : expression_m{std::move(expression)} {}
  ~Print() = default;

  static auto create(Expr&& expression) -> Stmt {
    return std::make_unique<Print>(std::move(expression));
  }

  static auto empty() -> Stmt { return std::unique_ptr<Print>(nullptr); }
};

class Return {
 public:
  const Token keyword_m;
  const Expr value_m;

  Return(Token&& keyword, Expr&& value)
      : keyword_m{std::move(keyword)}, value_m{std::move(value)} {}
  ~Return() = default;

  static auto create(Token&& keyword, Expr&& value) -> Stmt {
    return std::make_unique<Return>(std::move(keyword), std::move(value));
  }

  static auto empty() -> Stmt { return std::unique_ptr<Return>(nullptr); }
};

class Var {
 public:
  const Token name_m;
  const Expr initializer_m;

  Var(Token&& name, Expr&& initializer)
      : name_m{std::move(name)}, initializer_m{std::move(initializer)} {}
  ~Var() = default;

  static auto create(Token&& name, Expr&& initializer) -> Stmt {
    return std::make_unique<Var>(std::move(name), std::move(initializer));
  }

  static auto empty() -> Stmt { return std::unique_ptr<Var>(nullptr); }
};

#endif
