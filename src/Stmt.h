#ifndef LOXALONE_Stmt_H
#define LOXALONE_Stmt_H

#include <memory>
#include <string>
#include <variant>

#include "Token.h"

#include "Expr.h"

class Expression;
class Print;
class Var;

using ExpressionPtr = std::unique_ptr<Expression>;
using PrintPtr = std::unique_ptr<Print>;
using VarPtr = std::unique_ptr<Var>;

using Stmt = std::variant<ExpressionPtr, PrintPtr, VarPtr>;

template <typename V, typename Out>
concept StmtVisitor = requires(V v, const ExpressionPtr& arg_0,
                               const PrintPtr& arg_1, const VarPtr& arg_2) {
  { v(arg_0) } -> std::convertible_to<Out>;
  { v(arg_1) } -> std::convertible_to<Out>;
  { v(arg_2) } -> std::convertible_to<Out>;
};

class Expression {
 public:
  const Expr expression_m;

  explicit Expression(Expr&& expression)
      : expression_m{std::move(expression)} {}
  ~Expression() = default;
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
