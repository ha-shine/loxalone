//
// Created by Htet Aung Shine on 9/12/22.
//

#include "Interpreter.h"

// Report runtime scanner_error by printing to stderr
auto report_error(const RuntimeError& err) -> void {
  fmt::print(stderr, "{}\n[line {}]", err.msg, err.token.line);
}

auto is_equal(const lox_literal& left, const lox_literal& right) -> bool {
  if (std::holds_alternative<double>(left) &&
      std::holds_alternative<double>(right)) {
    return std::get<double>(left) == std::get<double>(right);
  } else if (std::holds_alternative<std::string>(left) &&
             std::holds_alternative<std::string>(right)) {
    return std::get<std::string>(left) == std::get<std::string>(right);
  } else if (std::holds_alternative<bool>(left) &&
             std::holds_alternative<bool>(right)) {
    return std::get<bool>(left) == std::get<bool>(right);
  } else if (std::holds_alternative<std::monostate>(left)) {
    return std::holds_alternative<std::monostate>(right);
  }

  return false;
}

// TODO: No scanner_error checking implemented for these methods yet?
auto Interpreter::operator()(const BinaryExprPtr& expr) -> lox_literal {
  auto left = visit<lox_literal>(*this, expr->left_m);
  auto right = visit<lox_literal>(*this, expr->right_m);

  switch (expr->oper_m.type) {
    case TokenType::MINUS:
      check_are_numbers(expr->oper_m, left, right);
      return std::get<double>(left) - std::get<double>(right);
    case TokenType::PLUS:
      if (std::holds_alternative<double>(left) &&
          std::holds_alternative<double>(right))
        return std::get<double>(left) + std::get<double>(right);
      else if (std::holds_alternative<std::string>(left) &&
               std::holds_alternative<std::string>(right))
        return std::get<std::string>(left) + std::get<std::string>(right);
      else
        throw RuntimeError{expr->oper_m,
                           "Operands must be either strings or numbers."};
    case TokenType::SLASH:
      check_are_numbers(expr->oper_m, left, right);
      return std::get<double>(left) / std::get<double>(right);
    case TokenType::STAR:
      check_are_numbers(expr->oper_m, left, right);
      return std::get<double>(left) * std::get<double>(right);
    case TokenType::GREATER:
      check_are_numbers(expr->oper_m, left, right);
      return std::get<double>(left) > std::get<double>(right);
    case TokenType::GREATER_EQUAL:
      check_are_numbers(expr->oper_m, left, right);
      return std::get<double>(left) >= std::get<double>(right);
    case TokenType::LESS:
      check_are_numbers(expr->oper_m, left, right);
      return std::get<double>(left) < std::get<double>(right);
    case TokenType::LESS_EQUAL:
      check_are_numbers(expr->oper_m, left, right);
      return std::get<double>(left) <= std::get<double>(right);
    case TokenType::EQUAL_EQUAL:
      return left == right;
    case TokenType::BANG_EQUAL:
      return left != right;
    default:
      return std::monostate{};
  }
}

auto Interpreter::operator()(const GroupingExprPtr& expr) -> lox_literal {
  return visit<lox_literal>(*this, expr->expression_m);
}

auto Interpreter::operator()(const LiteralValPtr& expr) -> lox_literal {
  return expr->value_m;
}

auto Interpreter::operator()(const UnaryExprPtr& expr) -> lox_literal {
  auto right = visit<lox_literal>(*this, expr->right_m);

  switch (expr->oper_m.type) {
    case TokenType::MINUS:
      check_is_number(expr->oper_m, right);
      return -(std::get<double>(right));
    case TokenType::BANG:
      check_is_boolean(expr->oper_m, right);
      return !(std::get<bool>(right));
    default:
      return std::monostate{};
  }
}

auto Interpreter::interpret(const Expr& expr) -> void {
  try {
    auto result = visit<lox_literal>(*this, expr);
    fmt::print("{}\n", result);
  } catch (const RuntimeError& err) {
    report_error(err);
  }
}

auto Interpreter::check_is_number(const Token& oper, const lox_literal& operand)
    -> void {
  if (!std::holds_alternative<double>(operand))
    throw RuntimeError{oper, "Operand must be a number."};
}

auto Interpreter::check_is_boolean(const Token& oper, const lox_literal& operand)
    -> void {
  if (!std::holds_alternative<bool>(operand))
    throw RuntimeError{oper, "Operand must be a boolean."};
}

auto Interpreter::check_are_numbers(const Token& oper, const lox_literal& left,
                                    const lox_literal& right) -> void {
  if (!std::holds_alternative<double>(left) ||
      !std::holds_alternative<double>(right))
    throw RuntimeError{oper, "Operands must be numbers."};
}
