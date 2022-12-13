#ifndef LOXALONE_Expr_H
#define LOXALONE_Expr_H

#include <memory>
#include <string>
#include <variant>

#include "Token.h"

class Assign;
class BinaryExpr;
class GroupingExpr;
class LiteralVal;
class Logical;
class UnaryExpr;
class Variable;

using AssignPtr = std::unique_ptr<Assign>;
using BinaryExprPtr = std::unique_ptr<BinaryExpr>;
using GroupingExprPtr = std::unique_ptr<GroupingExpr>;
using LiteralValPtr = std::unique_ptr<LiteralVal>;
using LogicalPtr = std::unique_ptr<Logical>;
using UnaryExprPtr = std::unique_ptr<UnaryExpr>;
using VariablePtr = std::unique_ptr<Variable>;

using Expr = std::variant<AssignPtr, BinaryExprPtr, GroupingExprPtr,
                          LiteralValPtr, LogicalPtr, UnaryExprPtr, VariablePtr>;

template <typename V, typename Out>
concept ExprVisitor =
    requires(V v, const AssignPtr& arg_0, const BinaryExprPtr& arg_1,
             const GroupingExprPtr& arg_2, const LiteralValPtr& arg_3,
             const LogicalPtr& arg_4, const UnaryExprPtr& arg_5,
             const VariablePtr& arg_6) {
  { v(arg_0) } -> std::convertible_to<Out>;
  { v(arg_1) } -> std::convertible_to<Out>;
  { v(arg_2) } -> std::convertible_to<Out>;
  { v(arg_3) } -> std::convertible_to<Out>;
  { v(arg_4) } -> std::convertible_to<Out>;
  { v(arg_5) } -> std::convertible_to<Out>;
  { v(arg_6) } -> std::convertible_to<Out>;
};

class Assign {
 public:
  const Token name_m;
  const Expr value_m;

  Assign(Token&& name, Expr&& value)
      : name_m{std::move(name)}, value_m{std::move(value)} {}
  ~Assign() = default;
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

class Logical {
 public:
  const Expr left_m;
  const Token oper_m;
  const Expr right_m;

  Logical(Expr&& left, Token&& oper, Expr&& right)
      : left_m{std::move(left)},
        oper_m{std::move(oper)},
        right_m{std::move(right)} {}
  ~Logical() = default;
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
