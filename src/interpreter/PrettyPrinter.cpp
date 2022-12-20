//
// Created by Htet Aung Shine on 30/11/22.
//

#include "PrettyPrinter.h"

#include <sstream>

#include "Token.h"

namespace loxalone {

auto PrettyPrinter::operator()(const BinaryPtr& expr) -> std::string {
  return parenthesize(expr->oper_m.lexeme, expr->left_m, expr->right_m);
}

auto PrettyPrinter::operator()(const GroupingPtr& expr) -> std::string {
  return parenthesize("group", expr->expression_m);
}

auto PrettyPrinter::operator()(const LiteralPtr& expr) -> std::string {
  return std::visit(
      [](auto&& arg) -> std::string { return fmt::format("{}", arg); },
      expr->value_m);
}

auto PrettyPrinter::operator()(const UnaryPtr& expr) -> std::string {
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

auto PrettyPrinter::operator()(const LogicalPtr& expr) -> std::string {
  return fmt::format("({} {} {})", tt_to_string(expr->oper_m.type),
                     visit(*this, expr->left_m),
                     std::visit(*this, expr->right_m));
}

auto PrettyPrinter::operator()(const CallPtr& expr) -> std::string {
  std::stringstream oss{};
  oss << fmt::format("({}", visit(*this, expr->callee_m));
  for (const auto& arg : expr->arguments_m) {
    oss << fmt::format(" {}", std::visit(*this, arg));
  }
  oss << ")";
  return oss.str();
}

}  // namespace loxalone