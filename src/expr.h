#ifndef LOXALONE_EXPR_H
#define LOXALONE_EXPR_H

#include <memory>
#include <string>
#include <variant>

#include "tokens.h"

class BinaryExpr;
class GroupingExpr;
class LiteralVal;
class UnaryExpr;
using Expr = std::unique_ptr<
    std::variant<BinaryExpr, GroupingExpr, LiteralVal, UnaryExpr>>;

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

#endif
