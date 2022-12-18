//
// Created by Htet Aung Shine on 30/11/22.
//

#ifndef LOXALONE_PRETTYPRINTER_H
#define LOXALONE_PRETTYPRINTER_H

#include <string>

#include "Expr.h"
#include "LiteralFormatter.h"

// TODO: Probably better to use a string builder sort of type here
class PrettyPrinter {
 public:
  auto operator()(const BinaryPtr &) -> std::string;
  auto operator()(const GroupingPtr &) -> std::string;
  auto operator()(const LiteralPtr &) -> std::string;
  auto operator()(const UnaryPtr &) -> std::string;
  auto operator()(const VariablePtr &) -> std::string;
  auto operator()(const AssignPtr &) -> std::string;
  auto operator()(const LogicalPtr &) -> std::string;
  auto operator()(const CallPtr &) -> std::string;

 private:
  // TODO: Can these two be written with variadic template?
  auto parenthesize(const std::string_view &, const Expr &) -> std::string;
  auto parenthesize(const std::string_view &, const Expr &, const Expr &)
      -> std::string;
};

static_assert(ExprVisitor<PrettyPrinter, std::string>);

#endif  // LOXALONE_PRETTYPRINTER_H
