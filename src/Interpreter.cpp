//
// Created by Htet Aung Shine on 9/12/22.
//

#include "Interpreter.h"

#include <chrono>
#include <memory>

#include "LiteralFormatter.h"
#include "LoxCallable.h"

namespace loxalone {

Interpreter::Interpreter() : globals{}, env{&globals}, locals{} {
  globals.define("clock", std::make_shared<NativeCallable>(
                              "clock", 0, [](const auto& args) -> lox_literal {
                                using namespace std::chrono;

                                auto time =
                                    system_clock::now().time_since_epoch();
                                auto to_s = duration_cast<seconds>(time);
                                return 1.0 * static_cast<double>(to_s.count());
                              }));
}

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
  if (!expr) return std::monostate{};

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
  if (!expr) return std::monostate{};

  return visit<lox_literal>(*this, expr->expression_m);
}

auto Interpreter::operator()(const LiteralPtr& expr) -> lox_literal {
  if (!expr) return std::monostate{};

  return expr->value_m;
}

auto Interpreter::operator()(const UnaryPtr& expr) -> lox_literal {
  if (!expr) return std::monostate{};

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
  if (!expr) return std::monostate{};
  return lookup_variable(expr->name_m, expr);
}

auto Interpreter::operator()(const AssignPtr& expr) -> lox_literal {
  if (!expr) return std::monostate{};

  lox_literal value = visit(*this, expr->value_m);
  void* ptr = static_cast<void*>(expr.get());
  auto find = locals.find(ptr);
  if (find != locals.end()) {
    env->assign_at(expr->name_m, find->second, lox_literal{value});
  } else {
    globals.assign(expr->name_m, value);
  }

  return value;
}

auto Interpreter::operator()(const LogicalPtr& expr) -> lox_literal {
  if (!expr) return std::monostate{};

  lox_literal left = visit(*this, expr->left_m);
  check_is_boolean(expr->oper_m, left);
  if (expr->oper_m.type == TokenType::OR) {
    if (std::get<bool>(left)) return true;
  } else {
    if (!std::get<bool>(left)) return false;
  }
  return visit(*this, expr->right_m);
}

auto Interpreter::operator()(const CallPtr& expr) -> lox_literal {
  if (!expr) return std::monostate{};

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

  return ptr->execute(*this, args);
}

auto Interpreter::operator()(const BlockPtr& stmt) -> void {
  if (!stmt) return;

  Environment* previous = env;

  // Dumb RAII handler to restore the parent's scope after exit
  std::shared_ptr<std::monostate> cleaner{
      nullptr, [&](std::monostate* ptr) { env = previous; }};

  Environment new_env = Environment{previous};
  env = &new_env;
  for (const auto& statement : stmt->statements_m) {
    visit(*this, statement);
  }
}

auto Interpreter::operator()(const ExpressionPtr& stmt) -> void {
  if (!stmt) return;

  visit(*this, stmt->expression_m);
}

auto Interpreter::operator()(const FunctionPtr& stmt) -> void {
  // This feels hacky const casting away stuffs :\
  // But I think this should be fine since the parsed statements are not
  // used after being interpreted. Another way to achieve this would be to
  // use non-const stmt or fields, but I don't want to lose the nice-ity of
  // having const values everywhere.
  auto& ptr = const_cast<FunctionPtr&>(stmt);

  // The current environment is captured as a closure so that the created
  // function object still have access to current environment. Otherwise,
  // the variables defined could be lost if this function object is escaped
  // from the current scope. e.g.
  //
  // fun makeFn() {
  //   var i = 1;
  //   fun returnI() { return i; }
  //   return returnI;
  // }
  //
  // var fn = makeFn(); fn(); <- undefined variable i
  //
  LoxFunction function{
      Function::create(
          Token{ptr->name_m},
          std::move(const_cast<std::vector<Token>&>(ptr->params_m)),
          std::move(const_cast<std::vector<Stmt>&>(ptr->body_m))),
      *env};
  env->define(stmt->name_m.lexeme,
              std::make_shared<LoxFunction>(std::move(function)));
}

auto Interpreter::operator()(const PrintPtr& stmt) -> void {
  if (!stmt) return;

  lox_literal value = visit(*this, stmt->expression_m);
  fmt::print("{}", value);
}

auto Interpreter::operator()(const VarPtr& stmt) -> void {
  if (!stmt) return;

  lox_literal val = visit(*this, stmt->initializer_m);
  env->define(stmt->name_m.lexeme, val);
}

auto Interpreter::operator()(const IfPtr& stmt) -> void {
  if (!stmt) return;

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
  if (!stmt) return;

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

auto Interpreter::operator()(const ReturnPtr& stmt) -> void {
  if (!stmt) return;

  lox_literal value = visit(*this, stmt->value_m);
  if (!std::holds_alternative<std::monostate>(value)) throw ReturnObject{value};
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

auto Interpreter::execute_block(const std::vector<Stmt>& stmts,
                                Environment* environment) -> void {
  Environment* previous = env;

  // Dumb RAII handler to restore the parent's scope after exit
  std::shared_ptr<std::monostate> cleaner{
      nullptr, [&](std::monostate* ptr) { env = previous; }};

  env = environment;
  for (const auto& statement : stmts) {
    visit(*this, statement);
  }
}

auto Interpreter::get_globals() const -> const Environment& { return this->globals; }

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
}  // namespace loxalone
