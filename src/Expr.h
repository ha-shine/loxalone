#ifndef LOXALONE_Expr_H
#define LOXALONE_Expr_H

#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "Token.h"

class Assign;
class Binary;
class Call;
class Grouping;
class Literal;
class Logical;
class Unary;
class Variable;

using AssignPtr = std::unique_ptr<Assign>;
using BinaryPtr = std::unique_ptr<Binary>;
using CallPtr = std::unique_ptr<Call>;
using GroupingPtr = std::unique_ptr<Grouping>;
using LiteralPtr = std::unique_ptr<Literal>;
using LogicalPtr = std::unique_ptr<Logical>;
using UnaryPtr = std::unique_ptr<Unary>;
using VariablePtr = std::unique_ptr<Variable>;

using Expr = std::variant<AssignPtr, BinaryPtr, CallPtr, GroupingPtr,
                          LiteralPtr, LogicalPtr, UnaryPtr, VariablePtr>;

static auto expr_is_null(const Expr& expr) {
  if (std::holds_alternative<AssignPtr>(expr))
    return std::get<AssignPtr>(expr) == nullptr;
  if (std::holds_alternative<BinaryPtr>(expr))
    return std::get<BinaryPtr>(expr) == nullptr;
  if (std::holds_alternative<CallPtr>(expr))
    return std::get<CallPtr>(expr) == nullptr;
  if (std::holds_alternative<GroupingPtr>(expr))
    return std::get<GroupingPtr>(expr) == nullptr;
  if (std::holds_alternative<LiteralPtr>(expr))
    return std::get<LiteralPtr>(expr) == nullptr;
  if (std::holds_alternative<LogicalPtr>(expr))
    return std::get<LogicalPtr>(expr) == nullptr;
  if (std::holds_alternative<UnaryPtr>(expr))
    return std::get<UnaryPtr>(expr) == nullptr;
  if (std::holds_alternative<VariablePtr>(expr))
    return std::get<VariablePtr>(expr) == nullptr;
  return false;
}

template <typename V, typename Out>
concept ExprVisitor = requires(
    V v, const AssignPtr& arg_0, const BinaryPtr& arg_1, const CallPtr& arg_2,
    const GroupingPtr& arg_3, const LiteralPtr& arg_4, const LogicalPtr& arg_5,
    const UnaryPtr& arg_6, const VariablePtr& arg_7) {
                        { v(arg_0) } -> std::convertible_to<Out>;
                        { v(arg_1) } -> std::convertible_to<Out>;
                        { v(arg_2) } -> std::convertible_to<Out>;
                        { v(arg_3) } -> std::convertible_to<Out>;
                        { v(arg_4) } -> std::convertible_to<Out>;
                        { v(arg_5) } -> std::convertible_to<Out>;
                        { v(arg_6) } -> std::convertible_to<Out>;
                        { v(arg_7) } -> std::convertible_to<Out>;
                      };

class Assign {
 public:
  const Token name_m;
  const Expr value_m;

  Assign(Token&& name, Expr&& value)
      : name_m{std::move(name)}, value_m{std::move(value)} {}
  ~Assign() = default;
};

class Binary {
 public:
  const Expr left_m;
  const Token oper_m;
  const Expr right_m;

  Binary(Expr&& left, Token&& oper, Expr&& right)
      : left_m{std::move(left)},
        oper_m{std::move(oper)},
        right_m{std::move(right)} {}
  ~Binary() = default;
};

class Call {
 public:
  const Expr callee_m;
  const Token paren_m;
  const std::vector<Expr> arguments_m;

  Call(Expr&& callee, Token&& paren, std::vector<Expr>&& arguments)
      : callee_m{std::move(callee)},
        paren_m{std::move(paren)},
        arguments_m{std::move(arguments)} {}
  ~Call() = default;
};

class Grouping {
 public:
  const Expr expression_m;

  explicit Grouping(Expr&& expression) : expression_m{std::move(expression)} {}
  ~Grouping() = default;
};

class Literal {
 public:
  const lox_literal value_m;

  explicit Literal(lox_literal&& value) : value_m{std::move(value)} {}
  ~Literal() = default;
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

class Unary {
 public:
  const Token oper_m;
  const Expr right_m;

  Unary(Token&& oper, Expr&& right)
      : oper_m{std::move(oper)}, right_m{std::move(right)} {}
  ~Unary() = default;
};

class Variable {
 public:
  const Token name_m;

  explicit Variable(Token&& name) : name_m{std::move(name)} {}
  ~Variable() = default;
};

#endif
