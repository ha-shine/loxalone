//
// Created by Htet Aung Shine on 9/12/22.
//

#include "Interpreter.h"

#include <memory>

#include "LoxCallable.h"

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

auto Interpreter::operator()(const BinaryPtr& expr) -> lox_literal {
  if (!expr)
    return std::monostate{};

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

auto Interpreter::operator()(const GroupingPtr& expr) -> lox_literal {
  if (!expr)
    return std::monostate{};

  return visit<lox_literal>(*this, expr->expression_m);
}

auto Interpreter::operator()(const LiteralPtr& expr) -> lox_literal {
  if (!expr)
    return std::monostate{};

  return expr->value_m;
}

auto Interpreter::operator()(const UnaryPtr& expr) -> lox_literal {
  if (!expr)
    return std::monostate{};

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

auto Interpreter::operator()(const VariablePtr& expr) -> lox_literal {
  if (!expr)
    return std::monostate{};

  // TODO: This returns the copied value (and create a variant object).
  //       Profile this function.
  return env.get(expr->name_m);
}

auto Interpreter::operator()(const AssignPtr& expr) -> lox_literal {
  if (!expr)
    return std::monostate{};
  lox_literal value = visit(*this, expr->value_m);
  env.assign(expr->name_m, value);
  return value;
}

auto Interpreter::operator()(const LogicalPtr& expr) -> lox_literal {
  if (!expr)
    return std::monostate{};
  lox_literal left = visit(*this, expr->left_m);
  check_is_boolean(expr->oper_m, left);
  if (expr->oper_m.type == TokenType::OR) {
    if (std::get<bool>(left))
      return true;
  } else {
    if (!std::get<bool>(left))
      return false;
  }
  return visit(*this, expr->right_m);
}

auto Interpreter::operator()(const CallPtr& expr) -> lox_literal {
  // The interpreter needs to interpret this expression into a callable object
  lox_literal callee = visit(*this, expr->callee_m);
  std::vector<lox_literal> args{};
  for (const auto& arg : expr->arguments_m) {
    args.emplace_back(visit(*this, arg));
  }

  if (!std::holds_alternative<CallablePtr>(callee)) {
    throw RuntimeError{expr->paren_m, "Can only call functions and classes"};
  }

  auto& ptr = std::get<CallablePtr>(callee);
  if (args.size() != ptr->arity()) {
    throw RuntimeError{expr->paren_m,
                       fmt::format("Expected {} arguments but got {}.",
                                   ptr->arity(), args.size())};
  }

  return ptr->call_func(args);
}

auto Interpreter::operator()(const BlockPtr& stmt) -> void {
  if (!stmt)
    return;

  Environment previous = std::move(env);

  // Dumb RAII handler to restore the parent's scope after exit
  std::shared_ptr<std::monostate> cleaner{nullptr, [&](std::monostate* ptr) {
                                            env = std::move(previous);
                                          }};

  env = Environment{&previous};
  for (const auto& statement : stmt->statements_m) {
    visit(*this, statement);
  }
}

auto Interpreter::operator()(const ExpressionPtr& stmt) -> void {
  if (!stmt)
    return;
  visit(*this, stmt->expression_m);
}

auto Interpreter::operator()(const PrintPtr& stmt) -> void {
  if (!stmt)
    return;
  lox_literal value = visit(*this, stmt->expression_m);
  fmt::print("{}", value);
}

auto Interpreter::operator()(const VarPtr& stmt) -> void {
  if (!stmt)
    return;
  lox_literal val = visit(*this, stmt->initializer_m);
  env.define(stmt->name_m.lexeme, val);
}

auto Interpreter::operator()(const IfPtr& stmt) -> void {
  if (!stmt)
    return;
  lox_literal condition = visit(*this, stmt->expression_m);
  if (!std::holds_alternative<bool>(condition))
    throw RuntimeError{stmt->token_m,
                       "If condition must be a boolean expression."};

  if (std::get<bool>(condition)) {
    visit(*this, stmt->then_branch_m);
  } else {
    visit(*this, stmt->else_branch_m);
  }
}

auto Interpreter::operator()(const WhilePtr& stmt) -> void {
  lox_literal condition = std::visit(*this, stmt->condition_m);
  if (!std::holds_alternative<bool>(condition))
    throw RuntimeError{stmt->token_m,
                       "While condition must be a boolean expression."};

  while (std::get<bool>(condition)) {
    visit(*this, stmt->body_m);
    condition = std::visit(*this, stmt->condition_m);
    if (!std::holds_alternative<bool>(condition))
      throw RuntimeError{stmt->token_m,
                         "While condition must be a boolean expression."};
  }
}

auto Interpreter::interpret(const std::vector<Stmt>& stmts) -> bool {
  try {
    for (const auto& stmt : stmts) {
      visit(*this, stmt);
    }
    return true;
  } catch (const RuntimeError& err) {
    report_error(err);
    return false;
  }
}

auto Interpreter::check_is_number(const Token& oper, const lox_literal& operand)
    -> void {
  if (!std::holds_alternative<double>(operand))
    throw RuntimeError{oper, "Operand must be a number."};
}

auto Interpreter::check_is_boolean(const Token& oper,
                                   const lox_literal& operand) -> void {
  if (!std::holds_alternative<bool>(operand))
    throw RuntimeError{oper, "Operand must be a boolean."};
}

auto Interpreter::check_are_numbers(const Token& oper, const lox_literal& left,
                                    const lox_literal& right) -> void {
  if (!std::holds_alternative<double>(left) ||
      !std::holds_alternative<double>(right))
    throw RuntimeError{oper, "Operands must be numbers."};
}
