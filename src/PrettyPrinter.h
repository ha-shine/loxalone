//
// Created by Htet Aung Shine on 30/11/22.
//

#ifndef LOXALONE_PRETTYPRINTER_H
#define LOXALONE_PRETTYPRINTER_H

#include <string>
#include "Expr.h"

// TODO: Probably better to use a string builder sort of type here
class PrettyPrinter {
 public:
  auto operator()(const BinaryExprPtr&) -> std::string;
  auto operator()(const GroupingExprPtr&) -> std::string;
  auto operator()(const LiteralValPtr&) -> std::string;
  auto operator()(const UnaryExprPtr&) -> std::string;

 private:
  // TODO: Can these two be written with variadic template?
  auto parenthesize(const std::string_view&, const Expr&) -> std::string;
  auto parenthesize(const std::string_view&, const Expr&, const Expr&) -> std::string;
};

static_assert(ExprVisitor<PrettyPrinter, std::string>);

#endif  //LOXALONE_PRETTYPRINTER_H
