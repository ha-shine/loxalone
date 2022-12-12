#ifndef LOXALONE_Expr_H
#define LOXALONE_Expr_H

#include <memory>
#include <string>
#include <variant>

#include "Token.h"

class BinaryExpr;
class GroupingExpr;
class LiteralVal;
class UnaryExpr;
class Variable;

using BinaryExprPtr = std::unique_ptr<BinaryExpr>;
using GroupingExprPtr = std::unique_ptr<GroupingExpr>;
using LiteralValPtr = std::unique_ptr<LiteralVal>;
using UnaryExprPtr = std::unique_ptr<UnaryExpr>;
using VariablePtr = std::unique_ptr<Variable>;

using Expr = std::variant<BinaryExprPtr, GroupingExprPtr, LiteralValPtr,
                          UnaryExprPtr, VariablePtr>;

template <typename V, typename Out>
concept ExprVisitor =
    requires(V v, const BinaryExprPtr& arg_0, const GroupingExprPtr& arg_1,
             const LiteralValPtr& arg_2, const UnaryExprPtr& arg_3,
             const VariablePtr& arg_4) {
  { v(arg_0) } -> std::convertible_to<Out>;
  { v(arg_1) } -> std::convertible_to<Out>;
  { v(arg_2) } -> std::convertible_to<Out>;
  { v(arg_3) } -> std::convertible_to<Out>;
  { v(arg_4) } -> std::convertible_to<Out>;
};

class BinaryExpr {
 public:
  const Expr left_m;
  const Token oper_m;
  const Expr right_m;

  BinaryExpr(Expr&& left, Token&& oper, Expr&& right)
      : left_m{std::move(left)},
        oper_m{std::move(oper)},
        right_m{std::move(right)} {}
  ~BinaryExpr() = default;
};

class GroupingExpr {
 public:
  const Expr expression_m;

  explicit GroupingExpr(Expr&& expression)
      : expression_m{std::move(expression)} {}
  ~GroupingExpr() = default;
};

class LiteralVal {
 public:
  const lox_literal value_m;

  explicit LiteralVal(lox_literal&& value) : value_m{std::move(value)} {}
  ~LiteralVal() = default;
};

class UnaryExpr {
 public:
  const Token oper_m;
  const Expr right_m;

  UnaryExpr(Token&& oper, Expr&& right)
      : oper_m{std::move(oper)}, right_m{std::move(right)} {}
  ~UnaryExpr() = default;
};

class Variable {
 public:
  const Token name_m;

  explicit Variable(Token&& name) : name_m{std::move(name)} {}
  ~Variable() = default;
};

#endif
