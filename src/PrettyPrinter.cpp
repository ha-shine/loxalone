//
// Created by Htet Aung Shine on 30/11/22.
//

#include "PrettyPrinter.h"

#include <sstream>
#include "Token.h"

auto PrettyPrinter::operator()(const BinaryExprPtr& expr) -> std::string {
  return parenthesize(expr->oper_m.lexeme, expr->left_m, expr->right_m);
}

auto PrettyPrinter::operator()(const GroupingExprPtr& expr) -> std::string {
  return parenthesize("group", expr->expression_m);
}

auto PrettyPrinter::operator()(const LiteralValPtr& expr) -> std::string {
  return std::visit(
      [](auto&& arg) -> std::string { return fmt::format("{}", arg); },
      expr->value_m);
}

auto PrettyPrinter::operator()(const UnaryExprPtr& expr) -> std::string {
  return parenthesize(expr->oper_m.lexeme, expr->right_m);
}

auto PrettyPrinter::parenthesize(const std::string_view& name, const Expr& expr)
    -> std::string {
  return fmt::format("({} {})", name, visit(*this, expr));
}

auto PrettyPrinter::parenthesize(const std::string_view& name,
                                 const Expr& expr1, const Expr& expr2)
    -> std::string {
  return fmt::format("({} {} {})", name, visit(*this, expr1),
                     visit(*this, expr2));
}

auto PrettyPrinter::operator()(const VariablePtr& expr) -> std::string {
  return fmt::format("(var {}={})", expr->name_m.lexeme,
                     expr->name_m.literal.value());
}

auto PrettyPrinter::operator()(const AssignPtr& expr) -> std::string {
  return fmt::format("(assign {}={})", expr->name_m.lexeme,
                     visit(*this, expr->value_m));
}
