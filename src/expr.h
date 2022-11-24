#ifndef LOXALONE_EXPR_H
#define LOXALONE_EXPR_H

#include <string>
#include <variant>
#include "tokens.h"

class Expr {
 public:
  virtual ~Expr() = default;
};

class BinaryExpr : Expr {
 public:
  BinaryExpr(Expr&& left, Token&& oper, Expr&& right)
      : left_m{left}, oper_m{oper}, right_m{right} {}
  ~BinaryExpr() override = default;

 private:
  const Expr left_m;
  const Token oper_m;
  const Expr right_m;
};

class GroupingExpr : Expr {
 public:
  explicit GroupingExpr(Expr&& expression) : expression_m{expression} {}
  ~GroupingExpr() override = default;

 private:
  const Expr expression_m;
};

class LiteralVal : Expr {
 public:
  explicit LiteralVal(lox_literal&& value) : value_m{value} {}
  ~LiteralVal() override = default;

 private:
  const lox_literal value_m;
};

class UnaryExpr : Expr {
 public:
  UnaryExpr(Token&& oper, Expr&& right) : oper_m{oper}, right_m{right} {}
  ~UnaryExpr() override = default;

 private:
  const Token oper_m;
  const Expr right_m;
};

#endif
